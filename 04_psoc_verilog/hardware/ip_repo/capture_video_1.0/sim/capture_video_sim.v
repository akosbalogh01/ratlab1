`timescale 1ns / 1ps

////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:   11:24:33 06/11/2015
// Design Name:   capture_video_v1_0
// Module Name:   D:/User/Balazs/Egyetem/RatLab/Zynq/CaptureVideo/capture_video_sim.v
// Project Name:  CaptureVideo
// Target Device:  
// Tool versions:  
// Description: 
//
// Verilog Test Fixture created by ISE for module: capture_video_v1_0
//
// Dependencies:
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////

module capture_video_sim;

	// Inputs
	reg ov7670_vsync;
	reg ov7670_href;
	reg ov7670_pclk;
	reg ov7670_resetn;
	reg [7:0] ov7670_data;
	reg m00_axis_aclk;
	reg m00_axis_aresetn;
	reg m00_axis_tready;
    reg [31:0] register_control;
    reg [31:0] register_brightness;
    reg [31:0] register_contrast;

	// Outputs
	wire fsync;
	wire m00_axis_tvalid;
	wire [31:0] m00_axis_tdata;
	wire [3:0] m00_axis_tstrb;
	wire m00_axis_tlast;

	// Instantiate the Unit Under Test (UUT)
	ov7670_capture uut (
	    .pclk(ov7670_pclk),
	    .resetn(ov7670_resetn),
	    .vsync(ov7670_vsync),
	    .href(ov7670_href),
	    .data(ov7670_data),
		.fsync(fsync), 
		.m_axis_aclk(m00_axis_aclk), 
		.m_axis_aresetn(m00_axis_aresetn), 
		.m_axis_tvalid(m00_axis_tvalid), 
		.m_axis_tdata(m00_axis_tdata), 
		.m_axis_tstrb(m00_axis_tstrb), 
		.m_axis_tlast(m00_axis_tlast), 
		.m_axis_tready(m00_axis_tready),
		.register_control(register_control),
		.register_brightness(register_brightness),		
		.register_contrast(register_contrast)
	);


	// Generate clocks
	initial begin
		forever begin
		  #5 ov7670_pclk = ~ov7670_pclk; 
		  m00_axis_aclk = ~m00_axis_aclk;		  
		end  
	end
	
	// Generate data
	initial begin
        #5;
        forever begin
          #20 ov7670_data = 8'hFF;
          #20 ov7670_data = 8'h00;
        end  
    end
			
	initial begin
		// Initialize Inputs
		m00_axis_aresetn = 0;
		m00_axis_aclk = 1;		
		m00_axis_tready = 1;
				
		ov7670_resetn = 0;	
		ov7670_pclk = 1;
		ov7670_href = 0;
		ov7670_vsync = 0;
		
	    register_control = 32'h00000021;
		register_brightness = -32'd5;
		register_contrast = 32'd16;

        // Wait 100 ns for global reset to finish
		#100 m00_axis_aresetn = 1;
        ov7670_resetn = 1;        
                
		// Add stimulus here		
		#100 ov7670_vsync = 1;
		#100 ov7670_vsync = 0;
		
		#5;
		repeat (10) begin
		  #100 ov7670_href = 1;
		  #12800 ov7670_href = 0;		  
		end
		
	end
      
endmodule

