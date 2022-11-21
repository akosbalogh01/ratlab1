/*
 * \file	ov7670_capture.v
 * \author	Megyesi, Balazs Karoly
 * \date	30.06.2015.
 *
 * \brief	Captures the image recorded by OV7670 camera. Resolution: 640x480, format: RGB565.
 */
 
`timescale 1 ns / 1 ps

    module ov7670_capture #
    (
		parameter integer C_M_AXIS_TDATA_WIDTH	= 32,
		parameter integer C_S_AXI_DATA_WIDTH = 32
    )
    (
        // Camera signals
        input  wire pclk,
        input  wire resetn,
        input  wire vsync,
        input  wire href,
        input  wire [7:0] data,
        
        // Synchronization output for AXI VDMA
        output wire fsync,
        
        // AXI-Stream signals
        input  wire m_axis_aclk,
        input  wire m_axis_aresetn,
        output wire m_axis_tvalid,
        output wire [C_M_AXIS_TDATA_WIDTH - 1 : 0] m_axis_tdata,
		// TSTRB is the byte qualifier that indicates whether the content of the associated byte of TDATA is processed as a data byte or a position byte.
        output wire [(C_M_AXIS_TDATA_WIDTH / 8) - 1 : 0] m_axis_tstrb,
        output wire m_axis_tlast,
        input  wire m_axis_tready,
        
        // Registers
        input  wire [C_S_AXI_DATA_WIDTH - 1 : 0] register_control,
        input  wire [C_S_AXI_DATA_WIDTH - 1 : 0] register_brightness,
        input  wire [C_S_AXI_DATA_WIDTH - 1 : 0] register_contrast       
    );
        
    // Raw data read from the camera
    wire [15:0] data_raw;
    // Wire for connection the output of the image processor with the data of AXI-Stream IF
    wire [15:0] data_processed;

    // Instantiate image processor module
    image_processor #
    (
        .C_S_AXI_DATA_WIDTH(C_S_AXI_DATA_WIDTH),
        .C_PIXEL_DATA_WIDTH(16)
    )
    image_processor_inst
    (
        .data_in(data_raw),
        .data_out(data_processed),
        
	    .register_control(register_control),
        .register_brightness(register_brightness),
        .register_contrast(register_contrast)
    );
    
    reg [15:0] shift_reg;
    reg  [1:0] data_valid;

	// Resolution is 640 x 480
	localparam HORIZONTAL_SIZE = 640;                                               
	localparam VERTICAL_SIZE = 480;                                               
	                                                                                     
	// function called clogb2 that returns an integer which has the                      
	// value of the ceiling of the log base 2.                                           
	function integer clogb2 (input integer bit_depth);                                   
	  begin                                                                              
	    for(clogb2=0; bit_depth>0; clogb2=clogb2+1)                                      
	      bit_depth = bit_depth >> 1;                                                    
	  end                                                                                
	endfunction                                                                          
	                                                                                     
	// Minimum number of bits needed for horizontal counters
	localparam horizontal_counter_bit_number  = clogb2(HORIZONTAL_SIZE);                                
	                                                                                         
    // Signals whether current frame should be transferred to the AXI VDMA
    reg capture_run;
    
	// Transmission can be performed
	wire tx_en;
	
	// Horizontal counters
	reg [horizontal_counter_bit_number-1:0] horizontal_counter;                                                      

    // Auxiliary state for detecting falling edge of vsync
    reg vsync_prev;

  
	// I/O Connections assignments
    assign data_raw = shift_reg;

    // Transmission is enabled when data is valid and slave is ready 
    assign tx_en = m_axis_tready && m_axis_tvalid;   
                                                         
    
    /////////////////////////////////////////////////////
    // AXI-Stream signals
    /////////////////////////////////////////////////////
    

                                              
                                                                    
    always @ (posedge pclk)                                               
    begin
        if (!m_axis_aresetn)
        begin
            data_valid <= 2'b00;
            shift_reg <= 0;    
        end
    
        // This is a bit tricky href starts a pixel transfer that takes 3 cycles
        //        Input   | state after clock tick
        //         href   | data_valid    shift_reg           d                 we  address  address_next
        // cycle -1  x    |    xx      xxxxxxxxxxxxxxxx  xxxxxxxxxxxxxxxx  x   xxxx     xxxx
        // cycle 0   1    |    x1      xxxxxxxxRRRRRGGG  xxxxxxxxxxxxxxxx  x   xxxx     addr
        // cycle 1   0    |    10      RRRRRGGGGGGBBBBB  xxxxxxxxRRRRRGGG  x   addr     addr
        // cycle 2   x    |    0x      GGGBBBBBxxxxxxxx  RRRRRGGGGGGBBBBB  1   addr     addr+1
        
        else if (vsync)
            data_valid <= 2'b00;
        else
        begin
            data_valid      <= {data_valid[0], href && ~data_valid[0]};
            shift_reg       <= {shift_reg[7:0], data};
        end
    end
   
	always @ (posedge m_axis_aclk )
    begin
        if (!m_axis_aresetn | vsync)
        begin
            horizontal_counter <= 0;
        end
        else if (horizontal_counter == HORIZONTAL_SIZE)
        begin
            horizontal_counter <= 0;
        end
        
        else if (data_valid == 2'b10)
        begin
            horizontal_counter <= horizontal_counter + 1;
        end
   end
   
   assign m_axis_tstrb = 4'b1111;
   assign m_axis_tvalid = (data_valid == 2'b10) & capture_run;
   assign m_axis_tlast = (horizontal_counter == HORIZONTAL_SIZE-1) & m_axis_tvalid;
   assign m_axis_tdata = {8'b00000000, data_processed[15:11], 3'b000, data_processed[10:5], 2'b00, data_processed[4:0], 3'b000};

   
    // Fsync is active at the falling edge of vsync
    assign fsync = !vsync && vsync_prev && capture_run;

    always @ (posedge pclk)
    begin
        if (!resetn)
           vsync_prev <= 1'b0;
        else
           vsync_prev <= vsync;
    end
    
    // When vsync is high it is checked if capture is active (LSB of control register)
    // If capture is inactive fsync and AXIS data valid should remain inactive to prevent
    // SOFLateErr flag to be set in AXI VDMA
    always @ (posedge pclk)
    begin
        if (!resetn)
           capture_run <= 1'b0;
        else if (vsync)
           capture_run <= register_control[0];
    end
    
endmodule