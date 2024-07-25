library IEEE; 
use IEEE.STD_LOGIC_1164.all; 
use IEEE.NUMERIC_STD.all;

entity TX_tb is
end;

architecture sim of TX_tb is
	component TX
		generic(
				  DEL_BTN: integer := 2500;
				  LEAD_H: integer := 450;
				  LEAD_L: integer := 225;
				  T1H: integer := 28;
				  T1L: integer := 84;
				  T0H: integer := 28;
				  T0L: integer := 28;
				  T:   integer := 2
				  );
		port(
			  clk:        		in  std_logic;
			  rst:        		in  std_logic;
			  rdy:				in  std_logic;
			  data_in:    		in  std_logic_vector(31 downto 0);
			  btn_mode:   		in  std_logic;
			  sw_adrs:        in  std_logic_vector(7 downto 0);
			  sw_cmnd:       	in  std_logic_vector(7 downto 0);
			  led_0:        	out std_logic;
			  led_1:        	out std_logic;
			  ir_data_out:    out std_logic
			  );
	end component TX;
	
   signal clk: 		  std_logic := '0';
   signal rst: 		  std_logic := '0';
   signal rdy: 		  std_logic := '0';
   signal data_in: 	  std_logic_vector(31 downto 0) := (others => '0');
   signal btn_mode: 	  std_logic := '0';
   signal sw_adrs: 	  std_logic_vector(7 downto 0) := (others => '0');
   signal sw_cmnd: 	  std_logic_vector(7 downto 0) := (others => '0');
   signal led_0: 		  std_logic;
   signal led_1: 		  std_logic;
   signal ir_data_out: std_logic;

begin
	
	dut: TX 
		port map(
				   clk => clk,      		
				   rst => rst,     		
				   rdy => rdy,			
				   data_in => data_in,  		
				   btn_mode => btn_mode,	
				   sw_adrs => sw_adrs,      
				   sw_cmnd => sw_adrs,
				   led_0 => led_0,					
				   led_1 => led_1,    	
				   ir_data_out => ir_data_out
				  );
	
	process begin
		wait for 5 ns;
		clk <= not clk;
	end process;
	
	process begin
		wait for 50 ns;
		data_in <= 32X"F5F5E1E1";
		wait for 50 ns;
		rdy <= '1';
		wait for 68 us;
		data_in <= (others => '0');
		wait;
	end process;
	
	process begin
		wait for 669 us;
		sw_adrs <= "01100110";
		sw_cmnd <= "10101010";
		wait for 1 us;
		btn_mode <= '1';
		wait for 50 us;
		btn_mode <= '0';
		wait;
	end process;
end;