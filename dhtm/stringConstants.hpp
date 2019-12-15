#pragma once

#define MIN(x, y) (((x) < (y)) ? (x) : (y))     
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
	        
#define KEY_LINE_FEED 0x0A
#define KEY_CARRIAGE_RETURN 0x0D
#define KEY_ZERO 0x2F
#define KEY_NINE 0x39
#define KEY_DOT 0x2E
#define KEY_BCKSPACE 0x7F

namespace dh {
	                                                                                                                                                                                                            
constexpr char SYM_ACTIVE[]{"\u25A0"};          // Active Minicolumn 
constexpr char SYM_INACTIVE[]{"\u25A1"};        // Inactive minicolumn
	
constexpr char SEP_MEN[]{"\u25B6"};             // Menu Separator
constexpr char SEP_SEL[]{"\u25BC"};             // Selection Separator
constexpr char SEP_PRM[]{":"};                  // Parameter Separator
	
constexpr char KEY_MENU[]{"MENU"};              // Menu
constexpr char KEY_VIEW[]{"VIEW"};              // View
constexpr char KEY_LAYER[]{"LAYER"};            // Layer
constexpr char KEY_PARAMS[]{"PARMS"};           // Params
constexpr char KEY_INPUT[]{"INPUT"};            // Input
constexpr char KEY_SPOOL[]{"SPOOL"};            // Spatial Pooler
constexpr char KEY_NUMCOL[]{"NUMCOL"};          // Displayed number of Columns 
constexpr char KEY_MINVAL[]{"MINVAL"};          // Minimal encoded value
constexpr char KEY_MAXVAL[]{"MAXVAL"};          // Maximal encoded value
constexpr char KEY_SDRLEN[]{"SDRLEN"};          // SDR Length
constexpr char KEY_ACTBTS[]{"ACTBTS"};          // Active Bits 
		        
}
