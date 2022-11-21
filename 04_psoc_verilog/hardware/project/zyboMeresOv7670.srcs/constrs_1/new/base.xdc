# Buttons
set_property PACKAGE_PIN R18 [get_ports {BTNs_4Bits_tri_i[0]}]
set_property PACKAGE_PIN P16 [get_ports {BTNs_4Bits_tri_i[1]}]
set_property PACKAGE_PIN V16 [get_ports {BTNs_4Bits_tri_i[2]}]
set_property PACKAGE_PIN Y16 [get_ports {BTNs_4Bits_tri_i[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {BTNs_4Bits_tri_i[*]}]

# LEDs
set_property PACKAGE_PIN M14 [get_ports {LEDs_4Bits_tri_o[0]}]
set_property PACKAGE_PIN M15 [get_ports {LEDs_4Bits_tri_o[1]}]
set_property PACKAGE_PIN G14 [get_ports {LEDs_4Bits_tri_o[2]}]
set_property PACKAGE_PIN D18 [get_ports {LEDs_4Bits_tri_o[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {LEDs_4Bits_tri_o[*]}]

# Switches
set_property PACKAGE_PIN G15 [get_ports {SWs_4Bits_tri_i[0]}]
set_property PACKAGE_PIN P15 [get_ports {SWs_4Bits_tri_i[1]}]
set_property PACKAGE_PIN W13 [get_ports {SWs_4Bits_tri_i[2]}]
set_property PACKAGE_PIN T16 [get_ports {SWs_4Bits_tri_i[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {SWs_4Bits_tri_i[*]}]

# OV7670 Camera
# Control
set_property PACKAGE_PIN T17 [get_ports OV7670_RESETN]
set_property PACKAGE_PIN J15 [get_ports OV7670_PWDN]
set_property PACKAGE_PIN V12 [get_ports OV7670_VSYNC]
set_property PACKAGE_PIN W16 [get_ports OV7670_HREF]
set_property IOSTANDARD LVCMOS33 [get_ports OV7670_RESETN]
set_property IOSTANDARD LVCMOS33 [get_ports OV7670_PWDN]
set_property IOSTANDARD LVCMOS33 [get_ports OV7670_VSYNC]
set_property IOSTANDARD LVCMOS33 [get_ports OV7670_HREF]

# Clocks
set_property PACKAGE_PIN T14 [get_ports OV7670_XCLK]
set_property PACKAGE_PIN U14 [get_ports OV7670_PCLK]
set_property IOSTANDARD LVCMOS33 [get_ports OV7670_XCLK]
set_property IOSTANDARD LVCMOS33 [get_ports OV7670_PCLK]
# Lowest drive is set on XCLK to reduce noise
set_property DRIVE 4 [get_ports OV7670_XCLK]

# Data lines
set_property PACKAGE_PIN T20 [get_ports {OV7670_DATA[0]}]
set_property PACKAGE_PIN V20 [get_ports {OV7670_DATA[1]}]
set_property PACKAGE_PIN Y18 [get_ports {OV7670_DATA[2]}]
set_property PACKAGE_PIN W18 [get_ports {OV7670_DATA[3]}]
set_property PACKAGE_PIN V15 [get_ports {OV7670_DATA[4]}]
set_property PACKAGE_PIN T11 [get_ports {OV7670_DATA[5]}]
set_property PACKAGE_PIN W14 [get_ports {OV7670_DATA[6]}]
set_property PACKAGE_PIN T12 [get_ports {OV7670_DATA[7]}]
set_property IOSTANDARD LVCMOS33 [get_ports {OV7670_DATA[*]}]

# SCCB interface
set_property PACKAGE_PIN V13 [get_ports OV7670_SIOC]
set_property PACKAGE_PIN U17 [get_ports OV7670_SIOD]
set_property IOSTANDARD LVCMOS33 [get_ports OV7670_SIOC]
set_property IOSTANDARD LVCMOS33 [get_ports OV7670_SIOD]

# VGA ports
set_property PACKAGE_PIN M19 [get_ports {RED_O[0]}]
set_property PACKAGE_PIN L20 [get_ports {RED_O[1]}]
set_property PACKAGE_PIN J20 [get_ports {RED_O[2]}]
set_property PACKAGE_PIN G20 [get_ports {RED_O[3]}]
set_property PACKAGE_PIN F19 [get_ports {RED_O[4]}]
set_property IOSTANDARD LVCMOS33 [get_ports {RED_O[*]}]
set_property SLEW FAST [get_ports {RED_O[*]}]

set_property PACKAGE_PIN H18 [get_ports {GREEN_O[0]}]
set_property PACKAGE_PIN N20 [get_ports {GREEN_O[1]}]
set_property PACKAGE_PIN L19 [get_ports {GREEN_O[2]}]
set_property PACKAGE_PIN J19 [get_ports {GREEN_O[3]}]
set_property PACKAGE_PIN H20 [get_ports {GREEN_O[4]}]
set_property PACKAGE_PIN F20 [get_ports {GREEN_O[5]}]
set_property IOSTANDARD LVCMOS33 [get_ports {GREEN_O[*]}]
set_property SLEW FAST [get_ports {GREEN_O[*]}]

set_property PACKAGE_PIN P20 [get_ports {BLUE_O[0]}]
set_property PACKAGE_PIN M20 [get_ports {BLUE_O[1]}]
set_property PACKAGE_PIN K19 [get_ports {BLUE_O[2]}]
set_property PACKAGE_PIN J18 [get_ports {BLUE_O[3]}]
set_property PACKAGE_PIN G19 [get_ports {BLUE_O[4]}]
set_property IOSTANDARD LVCMOS33 [get_ports {BLUE_O[*]}]
set_property SLEW FAST [get_ports {BLUE_O[*]}]

set_property PACKAGE_PIN R19 [get_ports VSYNC_O]
set_property PACKAGE_PIN P19 [get_ports HSYNC_O]
set_property IOSTANDARD LVCMOS33 [get_ports VSYNC_O]
set_property IOSTANDARD LVCMOS33 [get_ports HSYNC_O]
set_property SLEW FAST [get_ports VSYNC_O]
set_property SLEW FAST [get_ports HSYNC_O]

# This constraint ensures the MMCM located in the clock region connected to the ZYBO's HDMI port
# is used for the axi_dispctrl core driving the HDMI port
set_property LOC MMCME2_ADV_X0Y0 [get_cells system_i/axi_dispctrl_0/inst/DONT_USE_BUFR_DIV5.Inst_mmcme2_drp/mmcm_adv_inst]

# axi_dispctrl constraints

#Note these constraints require that REFCLK be driven by the axi_lite clock (clk_fpga_0)
create_generated_clock -name vga_pxlclk -source [get_pins {system_i/processing_system7_0/inst/PS7_i/FCLKCLK[0]}] -multiply_by 1 [get_pins system_i/axi_dispctrl_0/inst/DONT_USE_BUFR_DIV5.BUFG_inst/O]
set_false_path -from [get_clocks vga_pxlclk] -to [get_clocks clk_fpga_0]
set_false_path -from [get_clocks clk_fpga_0] -to [get_clocks vga_pxlclk]

# capture_video constraints

# Timing for pixel data and href
create_clock -period 40.000 -name pixel_clk [get_ports OV7670_PCLK]

set_input_delay -clock pixel_clk -max 26.000 [get_ports {OV7670_DATA[*]}]
set_input_delay -clock pixel_clk -min 7.000 [get_ports {OV7670_DATA[*]}]

set_input_delay -clock pixel_clk -max 26.000 [get_ports OV7670_HREF]
set_input_delay -clock pixel_clk -min 14.000 [get_ports OV7670_HREF]

# Paths should be ignored from AXI-Lite signals to capture video logic
set_false_path -from [get_clocks clk_fpga_0] -to [get_clocks pixel_clk]

connect_debug_port u_ila_0/probe5 [get_nets [list system_i/capture_video_0/inst/capture/image_processor_inst/data_out_zero]]
connect_debug_port u_ila_0/probe6 [get_nets [list system_i/capture_video_0/inst/capture/image_processor_inst/data_out_filter13_out]]



connect_debug_port u_ila_0/probe0 [get_nets [list {system_i/capture_video_0/inst/capture/image_processor_inst/m_axis_s2mm_tready_0[0]}]]
connect_debug_port u_ila_0/probe1 [get_nets [list {system_i/capture_video_0/inst/capture/image_processor_inst/row_counter[0]} {system_i/capture_video_0/inst/capture/image_processor_inst/row_counter[1]} {system_i/capture_video_0/inst/capture/image_processor_inst/row_counter[2]} {system_i/capture_video_0/inst/capture/image_processor_inst/row_counter[3]} {system_i/capture_video_0/inst/capture/image_processor_inst/row_counter[4]} {system_i/capture_video_0/inst/capture/image_processor_inst/row_counter[5]} {system_i/capture_video_0/inst/capture/image_processor_inst/row_counter[6]} {system_i/capture_video_0/inst/capture/image_processor_inst/row_counter[7]} {system_i/capture_video_0/inst/capture/image_processor_inst/row_counter[8]} {system_i/capture_video_0/inst/capture/image_processor_inst/row_counter[9]}]]
connect_debug_port u_ila_0/probe2 [get_nets [list {system_i/capture_video_0/inst/capture/image_processor_inst/column_counter[0]} {system_i/capture_video_0/inst/capture/image_processor_inst/column_counter[1]} {system_i/capture_video_0/inst/capture/image_processor_inst/column_counter[2]} {system_i/capture_video_0/inst/capture/image_processor_inst/column_counter[3]} {system_i/capture_video_0/inst/capture/image_processor_inst/column_counter[4]} {system_i/capture_video_0/inst/capture/image_processor_inst/column_counter[5]} {system_i/capture_video_0/inst/capture/image_processor_inst/column_counter[6]} {system_i/capture_video_0/inst/capture/image_processor_inst/column_counter[7]} {system_i/capture_video_0/inst/capture/image_processor_inst/column_counter[8]} {system_i/capture_video_0/inst/capture/image_processor_inst/column_counter[9]}]]
connect_debug_port u_ila_0/probe3 [get_nets [list system_i/capture_video_0/inst/capture/image_processor_inst/data_in_valid]]
connect_debug_port u_ila_0/probe4 [get_nets [list system_i/capture_video_0/inst/capture/image_processor_inst/data_out_zero]]
connect_debug_port u_ila_0/probe5 [get_nets [list {system_i/capture_video_0/inst/capture/image_processor_inst/line_buffer_wea[0]_17}]]
connect_debug_port u_ila_0/probe6 [get_nets [list {system_i/capture_video_0/inst/capture/image_processor_inst/line_buffer_wea[1]_18}]]
connect_debug_port u_ila_0/probe7 [get_nets [list {system_i/capture_video_0/inst/capture/image_processor_inst/line_buffer_wea[2]_16}]]

