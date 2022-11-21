/*
 * \file	capture_video_v1_0.v
 * \author	Megyesi, Balazs Karoly
 * \date	30.06.2015.
 *
 * \brief	Top module of the capture video IP.
 */

`timescale 1 ns / 1 ps

	module capture_video_v1_0 #
	(
		// Users to add parameters here

		// User parameters ends
		// Do not modify the parameters beyond this line


		// Parameters of Axi Slave Bus Interface S_AXI
		parameter integer C_S_AXI_DATA_WIDTH	= 32,
		parameter integer C_S_AXI_ADDR_WIDTH	= 4,

		// Parameters of Axi Master Bus Interface M_AXIS_S2MM
		parameter integer C_M_AXIS_S2MM_TDATA_WIDTH	= 32
	)
	(
		// Users to add ports here
        input  wire clk_24,
          
        output wire ov7670_sioc,
        inout  wire ov7670_siod,
        output wire ov7670_resetn,
        output wire ov7670_pwdn,
        input  wire ov7670_vsync,
        input  wire ov7670_href,
        input  wire ov7670_pclk,
        input  wire [7:0] ov7670_data,
        
        output wire fsync,
		// User ports ends
		// Do not modify the ports beyond this line


		// Ports of Axi Slave Bus Interface S_AXI
		input wire  s_axi_aclk,
		input wire  s_axi_aresetn,
		input wire [C_S_AXI_ADDR_WIDTH-1 : 0] s_axi_awaddr,
		input wire [2 : 0] s_axi_awprot,
		input wire  s_axi_awvalid,
		output wire  s_axi_awready,
		input wire [C_S_AXI_DATA_WIDTH-1 : 0] s_axi_wdata,
		input wire [(C_S_AXI_DATA_WIDTH/8)-1 : 0] s_axi_wstrb,
		input wire  s_axi_wvalid,
		output wire  s_axi_wready,
		output wire [1 : 0] s_axi_bresp,
		output wire  s_axi_bvalid,
		input wire  s_axi_bready,
		input wire [C_S_AXI_ADDR_WIDTH-1 : 0] s_axi_araddr,
		input wire [2 : 0] s_axi_arprot,
		input wire  s_axi_arvalid,
		output wire  s_axi_arready,
		output wire [C_S_AXI_DATA_WIDTH-1 : 0] s_axi_rdata,
		output wire [1 : 0] s_axi_rresp,
		output wire  s_axi_rvalid,
		input wire  s_axi_rready,

		// Ports of Axi Master Bus Interface M_AXIS_S2MM
		input wire  m_axis_s2mm_aclk,
		input wire  m_axis_s2mm_aresetn,
		output wire  m_axis_s2mm_tvalid,
		output wire [C_M_AXIS_S2MM_TDATA_WIDTH-1 : 0] m_axis_s2mm_tdata,
		output wire [(C_M_AXIS_S2MM_TDATA_WIDTH/8)-1 : 0] m_axis_s2mm_tstrb,
		output wire  m_axis_s2mm_tlast,
		input wire  m_axis_s2mm_tready
	);
	
	// Wires for registers
	wire [C_S_AXI_DATA_WIDTH - 1 : 0] register_control;
	wire [C_S_AXI_DATA_WIDTH - 1 : 0] register_brightness;
	wire [C_S_AXI_DATA_WIDTH - 1 : 0] register_contrast;
	
    // Instantiation of Axi Bus Interface S_AXI
	axi4_lite_interface # ( 
		.C_S_AXI_DATA_WIDTH(C_S_AXI_DATA_WIDTH),
		.C_S_AXI_ADDR_WIDTH(C_S_AXI_ADDR_WIDTH)
	) axi4_lite_interface_inst (
	    .register_control(register_control),
	    .register_brightness(register_brightness),
	    .register_contrast(register_contrast),
	
		.S_AXI_ACLK(s_axi_aclk),
		.S_AXI_ARESETN(s_axi_aresetn),
		.S_AXI_AWADDR(s_axi_awaddr),
		.S_AXI_AWPROT(s_axi_awprot),
		.S_AXI_AWVALID(s_axi_awvalid),
		.S_AXI_AWREADY(s_axi_awready),
		.S_AXI_WDATA(s_axi_wdata),
		.S_AXI_WSTRB(s_axi_wstrb),
		.S_AXI_WVALID(s_axi_wvalid),
		.S_AXI_WREADY(s_axi_wready),
		.S_AXI_BRESP(s_axi_bresp),
		.S_AXI_BVALID(s_axi_bvalid),
		.S_AXI_BREADY(s_axi_bready),
		.S_AXI_ARADDR(s_axi_araddr),
		.S_AXI_ARPROT(s_axi_arprot),
		.S_AXI_ARVALID(s_axi_arvalid),
		.S_AXI_ARREADY(s_axi_arready),
		.S_AXI_RDATA(s_axi_rdata),
		.S_AXI_RRESP(s_axi_rresp),
		.S_AXI_RVALID(s_axi_rvalid),
		.S_AXI_RREADY(s_axi_rready)
	);

    ov7670_capture #
    (
        .C_M_AXIS_TDATA_WIDTH(C_M_AXIS_S2MM_TDATA_WIDTH),
        .C_S_AXI_DATA_WIDTH(C_S_AXI_DATA_WIDTH)
    )
    capture
    (
        .pclk(ov7670_pclk),
        .resetn(ov7670_resetn),
        .vsync(ov7670_vsync),
        .href(ov7670_href),
        .data(ov7670_data),
        .fsync(fsync),
        
        .m_axis_aclk(m_axis_s2mm_aclk),
        .m_axis_aresetn(m_axis_s2mm_aresetn),
        .m_axis_tvalid(m_axis_s2mm_tvalid),
        .m_axis_tdata(m_axis_s2mm_tdata),
        .m_axis_tstrb(m_axis_s2mm_tstrb),
        .m_axis_tlast(m_axis_s2mm_tlast),
        .m_axis_tready(m_axis_s2mm_tready),
        
	    .register_control(register_control),
        .register_brightness(register_brightness),
        .register_contrast(register_contrast)
    );
    
    ov7670_controller controller
    (
        .clk(clk_24),
        .sioc(ov7670_sioc),
        .siod(ov7670_siod),
        .pwdn(ov7670_pwdn)
    );
    
    assign ov7670_resetn = m_axis_s2mm_aresetn;
        
	endmodule
