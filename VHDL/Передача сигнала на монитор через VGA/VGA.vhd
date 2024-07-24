library IEEE; 
use IEEE.STD_LOGIC_1164.all; 
use IEEE.NUMERIC_STD.all;

entity VGA is

generic(
		  SYNC_H		: integer := 79;
		  BP_H		: integer := 157;
		  FP_H		: integer := 15;
		  DIS_INT_H	: integer := 794;
		  
		  SYNC_V		: integer := 147;
		  BP_V		: integer := 1029;
		  FP_V		: integer := 49;
		  DIS_INT_V	: integer := 29400;
		  
		  RGB_CNG	: integer := 100
		  );

port(
	  pixel_clk		: in  std_logic;
	  
	  vga_r			: out std_logic_vector(7 downto 0);
	  vga_g			: out std_logic_vector(7 downto 0);
	  vga_b			: out std_logic_vector(7 downto 0);
	  
	  vga_hs			: out std_logic;
	  vga_vs			: out std_logic
	 );
end;

architecture rtl of VGA is

	type hs_statetype is (H_DISP, H_FRONT, H_SYNC, H_BACK);
	type vs_statetype is (V_DISP, V_FRONT, V_SYNC, V_BACK);
	type rgb_statetype is (RED, GREEN, BLUE);
	signal hs_state: hs_statetype := H_DISP;
	signal vs_state: vs_statetype := V_DISP;
	signal rgb_st: rgb_statetype;
	
	signal cnt_h: 		integer range 0 to DIS_INT_H-1 := 0;
	signal cnt_v: 		integer range 0 to DIS_INT_V-1 := 0;
	
	signal cnt_rgb: 	integer range 0 to RGB_CNG-1 := 0;
	
	signal r: std_logic_vector(7 downto 0) := (others => '0');
	signal g: std_logic_vector(7 downto 0) := (others => '0');
	signal b: std_logic_vector(7 downto 0) := (others => '0');
	
	signal vh_disp: std_logic := '1';
	
begin
	
	rgb_chng: process(pixel_clk)
	begin
		if rising_edge(pixel_clk) then
			case rgb_st is
				when RED => 
					r <= (others => '1');
					g <= (others => '0');
					b <= (others => '0');
					if cnt_rgb = RGB_CNG-1 then
						cnt_rgb <= 0;
						rgb_st <= GREEN;
					else
						cnt_rgb <= cnt_rgb + 1;
					end if;
				when GREEN => 
					r <= (others => '0');
					g <= (others => '1');
					b <= (others => '0');
					if cnt_rgb = RGB_CNG-1 then
						cnt_rgb <= 0;
						rgb_st <= BLUE;
					else
						cnt_rgb <= cnt_rgb + 1;
					end if;
				when BLUE => 
					r <= (others => '0');
					g <= (others => '0');
					b <= (others => '1');
					if cnt_rgb = RGB_CNG-1 then
						cnt_rgb <= 0;
						rgb_st <= RED;
					else
						cnt_rgb <= cnt_rgb + 1;
					end if;
			end case;
		end if;
	end process rgb_chng;
	
	hs_sync: process(pixel_clk)
	begin
		if rising_edge(pixel_clk) and vh_disp = '1' then
		
			case hs_state is
			
				when H_DISP =>
					vga_hs <= '1';
					if cnt_h = DIS_INT_H-1 then
						cnt_h <= 0;
						hs_state <= H_FRONT;
						vga_r <= (others => '0');
						vga_g <= (others => '0');
						vga_b <= (others => '0');
					else
						cnt_h <= cnt_h + 1;
						vga_r <= r;
						vga_g <= g;
						vga_b <= b;
					end if;
						
				when H_FRONT =>
					if cnt_h = FP_H-1 then
						cnt_h <= 0;
						vga_hs <= '0';
						hs_state <= H_SYNC;
					else
						cnt_h <= cnt_h + 1;
						vga_hs <= '1';
					end if;
						
				when H_SYNC =>
					if cnt_h = SYNC_H-1 then
						cnt_h <= 0;
						vga_hs <= '1';
						hs_state <= H_BACK;
					else
						cnt_h <= cnt_h + 1;
						vga_hs <= '0';
					end if;
				
				when H_BACK =>
					vga_hs <= '1';
					if cnt_h = BP_H-1 then
						cnt_h <= 0;
						hs_state <= H_DISP;
					else
						cnt_h <= cnt_h + 1;
					end if;
			end case;
		end if;
	end process hs_sync;
	
	vs_sync: process(pixel_clk)
	begin
		if rising_edge(pixel_clk) then
		
			case vs_state is
			
				when V_DISP =>
					vga_vs <= '1';
					if cnt_v = DIS_INT_V-1 then
						cnt_v <= 0;
						vh_disp <= '0';
						vs_state <= V_FRONT;
					else
						cnt_v <= cnt_v + 1;
						vh_disp <= '1';
					end if;
						
				when V_FRONT =>
					if cnt_v = FP_V-1 then
						cnt_v <= 0;
						vga_vs <= '0';
						vs_state <= V_SYNC;
					else
						cnt_v <= cnt_v + 1;
						vga_vs <= '1';
					end if;
						
				when V_SYNC =>
					if cnt_v = SYNC_V-1 then
						cnt_v <= 0;
						vga_vs <= '1';
						vs_state <= V_BACK;
					else
						cnt_v <= cnt_v + 1;
						vga_vs <= '0';
					end if;
				
				when V_BACK =>
					vga_vs <= '1';
					if cnt_v = BP_V-1 then
						cnt_v <= 0;
						vs_state <= V_DISP;
					else
						cnt_v <= cnt_v + 1;
					end if;
			end case;
		end if;
	end process vs_sync;
end;