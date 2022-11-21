/*
 * \file	image_processor.v
 * \author	Megyesi, Balazs Karoly
 * \date	30.06.2015.
 *
 * \brief	Performs low-level image processing functions, such as modifying brightness or contrast.
 *          Image can also be simply bypassed. Pixels have RGB565 format, but the incoming and 
 *          outgoing data is handled as RGB888.
 */
 
`timescale 1ns / 1ps

    module image_processor #
    (
		parameter integer C_S_AXI_DATA_WIDTH = 32,
        parameter C_PIXEL_DATA_WIDTH = 16
    )
    (       
        // Incoming and outgoing pixel data
        input  wire [C_PIXEL_DATA_WIDTH - 1 : 0] data_in,
        output reg [C_PIXEL_DATA_WIDTH - 1 : 0]  data_out,
        
        // Registers
        input  wire [C_S_AXI_DATA_WIDTH - 1 : 0] register_control,
        input  wire [C_S_AXI_DATA_WIDTH - 1 : 0] register_brightness,
        input  wire [C_S_AXI_DATA_WIDTH - 1 : 0] register_contrast       
    );
    
    // Bit positions for different image processor modes
    localparam BIT_CONTROL_MODE_BYPASS      = 32'h00000010;
    localparam BIT_CONTROL_MODE_SAMPLE      = 32'h00000020;
    localparam BIT_CONTROL_MODE_BRIGHTNESS  = 32'h00000040;
    localparam BIT_CONTROL_MODE_CONTRAST    = 32'h00000080;
    
    // Colour channel has a width of 8
    localparam COLOUR_WIDTH = 8;
    
    // Pixel data format (5:6:5) -> RRRRRGGGGGGBBBBB
    localparam RED_WIDTH   = 5;
    localparam GREEN_WIDTH = 6;
    localparam BLUE_WIDTH  = 5;
    
    // Wires for colour channels
    wire [COLOUR_WIDTH - 1 : 0] red;
    wire [COLOUR_WIDTH - 1 : 0] green;
    wire [COLOUR_WIDTH - 1 : 0] blue;
    
    // Signed variables for colour channels
    // Two bits should be added because of correct sign handling assuming that brightening value is in the range of -255..255
    reg signed [COLOUR_WIDTH + 1 : 0] red_added;
    reg signed [COLOUR_WIDTH + 1 : 0] green_added;
    reg signed [COLOUR_WIDTH + 1 : 0] blue_added;
    
    // Signed variables for colour channels
    // Assumption: factor is less then 10.0
    reg signed [COLOUR_WIDTH + 3 : 0] red_temp;
    reg signed [COLOUR_WIDTH + 3 : 0] green_temp;
    reg signed [COLOUR_WIDTH + 3 : 0] blue_temp;
    
    // Registers for brightness algorithm
    reg [COLOUR_WIDTH - 1 : 0] red_brightness;
    reg [COLOUR_WIDTH - 1 : 0] green_brightness;
    reg [COLOUR_WIDTH - 1 : 0] blue_brightness;    
    
    // Registers for contrast algorithm
    reg [COLOUR_WIDTH - 1 : 0] red_contrast;
    reg [COLOUR_WIDTH - 1 : 0] green_contrast;
    reg [COLOUR_WIDTH - 1 : 0] blue_contrast; 
       
    // Colour channel separation, values are extended to 8 bits
    assign red   = {data_in[C_PIXEL_DATA_WIDTH - 1 : C_PIXEL_DATA_WIDTH - RED_WIDTH], {(COLOUR_WIDTH - RED_WIDTH){1'b0}}};
    assign green = {data_in[C_PIXEL_DATA_WIDTH - RED_WIDTH - 1 : BLUE_WIDTH], {(COLOUR_WIDTH - GREEN_WIDTH){1'b0}}};
    assign blue  = {data_in[BLUE_WIDTH - 1 : 0], {(COLOUR_WIDTH - BLUE_WIDTH){1'b0}}};
    
    // Merge processed colour channels according to image processor mode
    always @ (*)
    case (register_control & 32'h00F0)
    
        BIT_CONTROL_MODE_BYPASS:
            data_out = data_in;
            
        BIT_CONTROL_MODE_SAMPLE:
            begin
                data_out[C_PIXEL_DATA_WIDTH - 1 : C_PIXEL_DATA_WIDTH - RED_WIDTH] <= data_in[C_PIXEL_DATA_WIDTH - 1 : C_PIXEL_DATA_WIDTH - RED_WIDTH];
                data_out[C_PIXEL_DATA_WIDTH - RED_WIDTH - 1 : BLUE_WIDTH] <= 6'b000000;
                data_out[BLUE_WIDTH - 1 : 0] <= data_in[BLUE_WIDTH - 1 : 0];
            end            
            
        BIT_CONTROL_MODE_BRIGHTNESS:
            data_out = {red_brightness  [COLOUR_WIDTH - 1 : COLOUR_WIDTH - RED_WIDTH  ],
                        green_brightness[COLOUR_WIDTH - 1 : COLOUR_WIDTH - GREEN_WIDTH],
                        blue_brightness [COLOUR_WIDTH - 1 : COLOUR_WIDTH - BLUE_WIDTH ]
                       };
                       
        BIT_CONTROL_MODE_CONTRAST:
            data_out = {red_contrast  [COLOUR_WIDTH - 1 : COLOUR_WIDTH - RED_WIDTH  ],
                        green_contrast[COLOUR_WIDTH - 1 : COLOUR_WIDTH - GREEN_WIDTH],
                        blue_contrast [COLOUR_WIDTH - 1 : COLOUR_WIDTH - BLUE_WIDTH ]
                       };
        default:
            data_out = data_in;
    endcase
            
/////////////////////////////////////////////////////
// Brightness
///////////////////////////////////////////////////// 
    // Extract brightness value from register -> 9 lower bits (range: -255..255) + sign extension
    wire signed [COLOUR_WIDTH + 1 : 0] brightness;    
    assign brightness = $signed(register_brightness[COLOUR_WIDTH : 0]); 
	
	always @(*)
    begin
        red_added <= (brightness*8)+red;
        
        if(red_added<0)
        begin
            red_brightness <= 0;
        end
        else if(red_added>255)
        begin
            red_brightness <= 255;
        end
        else
        begin
            red_brightness <= red_added[7:0];
        end
        
        green_added <= (brightness*4)+green;
        
        if(green_added<0)
        begin
            green_brightness <= 0;
        end
        else if(green_added>255)
        begin
            green_brightness <= 255;
        end
        else
        begin
            green_brightness <= green_added[7:0];
        end
        
        blue_added <= (brightness*8)+blue;
        
        if(blue_added<0)
        begin
            blue_brightness <= 0;
        end
        else if(blue_added>255)
        begin
            blue_brightness <= 255;
        end
        else
        begin
            blue_brightness <= blue_added[7:0];
        end
    end

    
/////////////////////////////////////////////////////
// Contrast
///////////////////////////////////////////////////// 
    // Extract contrast value from register -> 8 lower bits (range: 0.0625..15.9375)
    wire [7:0] contrast;    
    assign contrast = register_contrast[7:0];

	always @(*)
    begin
        red_temp <= (((red-128)*(contrast))>>4)+128;
        
        if(red_temp<0)
        begin
            red_contrast <= 0;
        end
        else if(red_temp>255)
        begin
            red_contrast <= 255;
        end
        else
        begin
            red_contrast <= red_temp[7:0];
        end
        
        green_temp <= (((green-128)*(contrast))>>4)+128;
        
        if(green_temp<0)
        begin
            green_contrast <= 0;
        end
        else if(green_temp>255)
        begin
            green_contrast <= 255;
        end
        else
        begin
            green_contrast <= green_temp[7:0];
        end
        
        blue_temp <= (((blue-128)*(contrast))>>4)+128;
        
        if(blue_temp<0)
        begin
            blue_contrast <= 0;
        end
        else if(blue_temp>255)
        begin
            blue_contrast <= 255;
        end
        else
        begin
            blue_contrast <= blue_temp[7:0];
        end
        
    end

    endmodule
