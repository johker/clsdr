#include <iostream>
#include <string>
#include <xtensor/xarray.hpp>
#include <chrono>
#include <thread>
#include <panel.h>
#include <ncursesw/ncurses.h>
#include <locale.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "../tiny_htm/tiny_htm/tiny_htm.hpp"

#include "dhtm/stringConstants.hpp"
#include "dhtm/terminalOutput.hpp"
#include "dhtm/htmController.hpp"


// VIM like HTM interface:
// INSERT mode: Keyboard entries are directly encoded 
// NORMAL mode: Commands to select view, exit program etc.
// Input should be decoupled from HTM output so a different sensor can be used easily

int updateScreen(WINDOW **win, const std::shared_ptr<dh::HtmController>& htmCtrl); 
void initNcurses();

int main(){

	int i = 0;						// main loop index	
	int avrows,avcols;					// Available rows / cols
	int avrowstmo = 0, avcolstmo = 0;			// Available rows / cols in t minus one
	WINDOW *win[4];						// Windows: std, control, status, content
	int cmdidx = 0;						// command index

	// Initialize View	
	setlocale(LC_ALL, "");					// Unicode support
	initscr();						// Init screen		
	clear();						// Clear terminal	 
	timeout(0);						// Non-blocking input
	noecho();						// No automatic printing
	cbreak();						// Immediate key input
	curs_set(0);						// Hide real cursor
	use_default_colors();					// Transparent background
	start_color();						// Use colors
	keypad(stdscr, 1);					// Fix keypad
	nonl();							// Get return key
	leaveok(stdscr,1);					// Dont care where cursor is left
	intrflush(stdscr,0);					// Avoid potential graphical issues
	
	// HTM 
	size_t numcat = 2;
	size_t encLen = 256;
	size_t actBits = 2;
	size_t phorizon = 10; 
	size_t min = 0; 
	size_t max = 100; 

	th::ScalarEncoder scalarEncoder(min, max, encLen, actBits);
	th::CategoryEncoder categoryEncoder(numcat, encLen);
	th::TemporalMemory tm({numcat*4}, 6);
	
	// Initialize controller
	std::shared_ptr<dh::HtmController> htmCtrl = std::make_shared<dh::HtmController>();
	htmCtrl->setScalarEncoder(&scalarEncoder);
	
	// Get initial screen dimensions
	getmaxyx(stdscr,avrows,avcols);	
	htmCtrl->avrows = avrows;
	htmCtrl->avcols = avcols;

	// Initialize view objects
	dh::TerminalOutput terminalOutput(htmCtrl); 

	// Initialize windows
	win[0] = stdscr;
	win[1] = newwin(3,avcols-2,0,1);			// Control window
	win[2] = newwin(3,avcols-2,avrows-3,1);			// Status window
	win[3] = newwin(avrows-6,avcols-2,3,1);			// Content window

	// Draw borders
	for(int wi = 1; wi < 4; wi++) {
		box(win[wi],0,0);
	}

	// Initialize colors
	init_pair(1, COLOR_BLACK, COLOR_WHITE);

				
	while(1) {
		// User Input
		while(1) { 
			int key = getch();
			if(key == ERR) break;
			if (key == KEY_F(2)) {
				htmCtrl->setMode(dh::SELECT);		// F2: Switch to select mode
				terminalOutput.collapse(win[1]);
			} else if(key == KEY_F(3)) {
				htmCtrl->setMode(dh::INSERT);		// F3: Switch to insert mode
				terminalOutput.collapse(win[1]);
			}
			if(htmCtrl->getMode()==dh::EDIT) {		
				if(key== KEY_DOT || key == KEY_BCKSPACE || key >= KEY_ZERO && key <= KEY_NINE) {
					terminalOutput.numEntry(win[1],key);
				}				
			}
		 	if(htmCtrl->getMode() == dh::SELECT || htmCtrl->getMode() == dh::EDIT) {
				switch(key) {
					case KEY_LEFT:
						terminalOutput.selLeft(win[1]);	
						break;
					case KEY_RIGHT:
						terminalOutput.selRight(win[1]);
						break;
					case KEY_UP:
						terminalOutput.selUp(win[1]);
						break;
					case KEY_DOWN:
						terminalOutput.selDown(win[1]);
						break;
					case KEY_ENTER: 
						terminalOutput.enter(win[1]);
						break;
					case KEY_LINE_FEED: 
						terminalOutput.enter(win[1]);
						break;
					case KEY_CARRIAGE_RETURN: 
						terminalOutput.enter(win[1]);
						break;
					default:
						htmCtrl->setStatusTxt(std::to_string(key));
						break;
					} 
				}
			if(htmCtrl->getMode() == dh::INSERT) {
				
			}
		}
		// HTM Update 
		auto sdr = scalarEncoder.encode(i%max);

		// Update screen dimension
		auto updt = updateScreen(win, htmCtrl);

		// Update status and control bar
		terminalOutput.printControlBar(win[1]);	
		terminalOutput.printStatusBar(win[2]);		
		terminalOutput.printContentPane(win[3],sdr);

		i += 1;
		// TODO Replace for better performing solution
		std::this_thread::sleep_for(std::chrono::milliseconds(100));	

	} // End of main loop 

	clrtoeol();
	refresh();
	endwin();
	return 0;
}


int updateScreen(WINDOW **win, const std::shared_ptr<dh::HtmController>& htmCtrl) {
	
	getmaxyx(win[0],htmCtrl->avrows,htmCtrl->avcols);	// Get screen dimensions
	if(htmCtrl->avrows != htmCtrl->avrowstmo || htmCtrl->avcols != htmCtrl->avcolstmo) {
		wresize(win[1],3,htmCtrl->avcols-2);
		wresize(win[2],3,htmCtrl->avcols-2);
		wresize(win[3],htmCtrl->avrows-6,htmCtrl->avcols-2);
		mvwin(win[2],htmCtrl->avrows-3,1); 
		wclear(win[0]);
		for(int wi = 1; wi < 4; wi++) {
			wclear(win[wi]);
			box(win[wi],0,0);
			wrefresh(win[wi]);
		}
		htmCtrl->avrowstmo = htmCtrl->avrows;
		htmCtrl->avcolstmo = htmCtrl->avcols;
			
		return 1;
	}
	return 0;
}
