/*
 * \file	image_processor_sim.v
 * \author	Megyesi, Balazs Karoly
 * \date	30.06.2015.
 *
 * \brief	Simulation file for image processor module.
 */

`timescale 1ns / 1ps

module image_processor_sim;

    // Inputs
    reg [15:0] data_in;
    reg [31:0] register_control;
    reg [31:0] register_brightness;
    reg [31:0] register_contrast;
    
    // Outputs
    wire [15:0] data_out;

	// Instantiate the Unit Under Test (UUT)
	image_processor #
	(
	   .C_S_AXI_DATA_WIDTH(32),
	   .C_PIXEL_DATA_WIDTH(16)
	)
	uut
	(
	    .data_in(data_in),
	    .data_out(data_out),
	    
	    .register_control(register_control),
	    .register_brightness(register_brightness),
	    .register_contrast(register_contrast)
	);	


endmodule
