library IEEE; 
use IEEE.STD_LOGIC_1164.all; 
use IEEE.NUMERIC_STD.all;

entity TX is

generic(
		  DEL_BTN: integer := 2500;
		  LEAD_H: integer := 450;
		  LEAD_L: integer := 225;
		  T1H: integer := 28;
		  T1L: integer := 84;
		  T0H: integer := 28;
		  T0L: integer := 28;
		  T:   integer := 3
		  );

port(
	  clk:        		in  std_logic;
	  rst:        		in  std_logic;
	  rdy:				in  std_logic;
	  
	  data_in:    		in  std_logic_vector(31 downto 0);
	  
	  btn_mode:   		in  std_logic;
	  sw_adrs:        	in  std_logic_vector(7 downto 0);
	  sw_cmnd:       	in  std_logic_vector(7 downto 0);
	  
	  led_0:        	out std_logic;
	  led_1:        	out std_logic;
	  ir_data_out:    	out std_logic
	 );
end;

architecture rtl of TX is
	
	signal data_adrs: std_logic_vector(7 downto 0) := (others => '0');
	signal data_cmnd: std_logic_vector(7 downto 0) := (others => '0');
	
	signal count_btn: integer range 0 to DEL_BTN := 0;
	signal count_pls: integer range 0 to T := 0;
	signal count_bit: integer range 0 to LEAD_H+LEAD_L := 0;
	signal prev_btn: std_logic := '0';
	signal mode: std_logic := '0';
	signal rpt: std_logic := '0';
	
	signal count_shift: integer range 0 to 8 := 0;
	signal shift: std_logic_vector(7 downto 0) := (others => '0');
	signal d: std_logic := '0';
	
	type   statetype 	  is (D_START, D_LEAD, D_ADRS, D_ADRS_INV, D_CMND, D_CMND_INV, D_IN, D_SEND);
	type   prev_statetype is (D_ADRS, D_ADRS_INV, D_CMND, D_CMND_INV);
	signal state: statetype;
	signal prev_state: prev_statetype;
	
begin
	button: process(clk, rst)
	begin
		if rst = '1' then
			count_btn <= 0;
			mode <= '0';
		elsif rising_edge(clk) then
			if btn_mode = '1' and rpt = '0' then
				rpt <= '1';
				count_btn <= count_btn + 1;
			elsif rpt = '1' and prev_btn = '0' then
				if count_btn = DEL_BTN-1 then
					if btn_mode = '1' then
						count_btn <= 0;
						rpt <= '0';
						prev_btn <= '1';
						mode <= not mode;
					else
						count_btn <= 0;
					end if;
				else
					count_btn <= count_btn + 1;
				end if;
			else
				prev_btn <= '0';
			end if;
		end if;
	end process button;
	
	state_machine: process(clk, rst)
	begin
		if rst = '1' then
			state <= D_START;
			shift <= (others => '0');
			ir_data_out <= '0';
			count_pls <= 0;
			count_bit <= 0;
			led_0 <= '1';
			led_1 <= '0';
		elsif rising_edge(clk) then
			case state is
			
				when D_START =>
					if mode = '0' then
						led_0 <= '1';
						led_1 <= '0';
						if rdy = '1' then
							data_adrs <= data_in(31 downto 24);
							data_cmnd <= data_in(15 downto 8);
							count_bit <= count_bit + 1;
							ir_data_out <= '1';
							state <= D_LEAD;
						end if;
					else
						led_0 <= '0';
						led_1 <= '1';
						data_adrs <= sw_adrs;
						data_cmnd <= sw_cmnd;
						count_bit <= count_bit + 1;
						ir_data_out <= '1';
						state <= D_LEAD;
					end if;
					
				when D_LEAD =>
					if count_bit = LEAD_H+LEAD_L-1 then
						count_bit <= 0;
						state <= D_ADRS;
					elsif count_bit > LEAD_H-1 then
						count_bit <= count_bit + 1;
						count_pls <= 0;
						ir_data_out <= '0';
					else
						count_bit <= count_bit + 1;
						if count_pls = T-1 then
							count_pls <= 0;
							ir_data_out <= not ir_data_out;
						else
							count_pls <= count_pls + 1;
						end if;
					end if;
				
				when D_ADRS =>
					shift <= data_adrs;
					state <= D_IN;
					prev_state<= D_ADRS;
					ir_data_out <= '1';
					count_pls <= count_pls + 1;
					
				when D_ADRS_INV =>
					shift <= not data_adrs;
					state <= D_IN;
					prev_state<= D_ADRS_INV;
					ir_data_out <= '1';
					count_pls <= count_pls + 1;
					
				when D_CMND =>
					shift <= data_cmnd;
					state <= D_IN;
					prev_state<= D_CMND;
					ir_data_out <= '1';
					count_pls <= count_pls + 1;
					
				when D_CMND_INV =>
					shift <= not data_cmnd;
					state <= D_IN;
					prev_state<= D_CMND_INV;
					ir_data_out <= '1';
					count_pls <= count_pls + 1;
					
				when D_IN =>
					if count_shift /= 8 then
						d <= shift(7);
						shift <= shift(6 downto 0) & '0';
						state <= D_SEND;
						count_pls <= count_pls + 1;
						count_shift <= count_shift + 1;
					else
						case prev_state is
							when D_ADRS     => state <= D_ADRS_INV;
							when D_ADRS_INV => state <= D_CMND;
							when D_CMND     => state <= D_CMND_INV;
							when D_CMND_INV => state <= D_START;
						end case;
						count_shift <= 0;
					end if;
					
				when D_SEND =>
					if d = '1' then
						if count_bit = T1H+T1L-1 then
							count_bit <= 0;
							state <= D_IN;
						elsif count_bit > T1H-1 then
							count_bit <= count_bit + 1;
							count_pls <= 0;
							ir_data_out <= '0';
						else
							count_bit <= count_bit + 1;
							if count_pls = T-1 then
								count_pls <= 0;
								ir_data_out <= not ir_data_out;
							else
								count_pls <= count_pls + 1;
							end if;
						end if;
					else
						if count_bit = T0H+T0L-1 then
							count_bit <= 0;
							state <= D_IN;
						elsif count_bit > T0H-1 then
							count_bit <= count_bit + 1;
							count_pls <= 0;
							ir_data_out <= '0';
						else
							count_bit <= count_bit + 1;
							if count_pls = T-1 then
								count_pls <= 0;
								ir_data_out <= not ir_data_out;
							else
								count_pls <= count_pls + 1;
							end if;
						end if;
					end if;
			end case;	
		end if;
	end process state_machine;
end;
				
				
			