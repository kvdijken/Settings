#ifndef _st7735_properties_h_
#define _st7735_properties_h_

// Properties of the TFT display
#define TFT_HEIGHT 128
#define TFT_WIDTH 160
// Width of one character in pixels
#define CHAR_WIDTH 6
// Height of one character in pixels
#define CHAR_HEIGHT 8

#define TFT_LINES (TFT_HEIGHT / CHAR_HEIGHT)
#define TFT_CHARS 26

// Color definitions
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF

#endif
