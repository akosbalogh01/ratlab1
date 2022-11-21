---------------------------------------------------------------------------------------------
-- \file	ov7670_controller.vhd
-- \author	Megyesi, Balazs Karoly
-- \date	30.06.2015.
--
-- \brief	Controls the initialization of the camera. I2C-like SCCB interface is used.
--          Register settings can be found in ov7670_registers.vhd.
---------------------------------------------------------------------------------------------
 
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity ov7670_controller is
    Port ( clk             : in    STD_LOGIC;
           sioc            : out   STD_LOGIC;
           siod            : inout STD_LOGIC;
           pwdn            : out   STD_LOGIC
);
end ov7670_controller;

architecture Behavioral of ov7670_controller is
    COMPONENT ov7670_registers
    PORT(
            clk      : IN  std_logic;
            advance  : IN  std_logic;
            command  : OUT std_logic_vector(15 downto 0);
            finished : out std_logic;
            wait_reset : OUT std_logic
            );
    END COMPONENT;

    COMPONENT i2c_sender
    PORT(
            clk   : IN    std_logic;
            send  : IN    std_logic;
            taken : out   std_logic;
            id    : IN    std_logic_vector(7 downto 0);
            reg   : IN    std_logic_vector(7 downto 0);
            value : IN    std_logic_vector(7 downto 0);
            siod  : INOUT std_logic;
            sioc  : OUT   std_logic
            );
    END COMPONENT;

    signal sys_clk          : std_logic := '0';
    signal command          : std_logic_vector(15 downto 0);
    signal finished         : std_logic := '0';
    signal taken            : std_logic := '0';
    signal send             : std_logic;
    signal wait_reset       : std_logic := '0';
    
    signal wait_send    : std_logic;
    signal wait_counter : unsigned(15 downto 0) := (others => '0');

    -- Device write ID - see top of page 11 of data sheet
    constant camera_address : std_logic_vector(7 downto 0) := x"42"; 
    
begin

    send <= (not finished) and (not wait_send);
    -- Power device up
    pwdn  <= '0';
    
    Inst_i2c_sender: i2c_sender PORT MAP(
            clk   => clk,
            taken => taken,
            siod  => siod,
            sioc  => sioc,
            send  => send,
            id    => camera_address,
            reg   => command(15 downto 8),
            value => command(7 downto 0)
    );
            
    Inst_ov7670_registers: ov7670_registers PORT MAP(
            clk      => clk,
            advance  => taken,
            command  => command,
            finished => finished,
            wait_reset => wait_reset
    );
        
    -- After hard or soft reset we should wait at least 1 ms before sending configuration data
    INCREMENT_WAIT_COUNTER: process(clk)
    begin
        if rising_edge(clk) then
            if (wait_reset = '1') then
                wait_counter <= (others => '0');
            elsif (wait_counter < X"FFFF") then
                wait_counter <= wait_counter + 1;
            end if;
        end if;
    end process INCREMENT_WAIT_COUNTER;
    
    with wait_counter select wait_send <= '0' when x"FFFF", '1' when others;
    
end Behavioral;