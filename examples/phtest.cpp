#include "../../tiny_htm/tiny_htm/tiny_htm.hpp"
#include "../plot_htm/plot_htm.hpp"
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

// VIM like HTM interface:
// INSERT mode: Keyboard entries are directly encoded 
// NORMAL mode: Commands to select view, exit program etc.
// Input should be decoupled from HTM output so a different sensor can be used easily

#define KEY_LINE_FEED 0x0A
#define KEY_CARRIAGE_RETURN 0x0D

//const std::string ACTIVE{"\u25A0"};

int updateScreen(WINDOW **win, const std::shared_ptr<ph::HtmController>& htmCtrl); 
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
	size_t enclen = 4;
	size_t phorizon = 10; 

	th::CategoryEncoder encoder(numcat, enclen);
	th::TemporalMemory tm({numcat*4}, 6);
	
	// Initialize controller
	std::shared_ptr<ph::HtmController> htmCtrl = std::make_shared<ph::HtmController>(encoder);
	
	// Get initial screen dimensions
	getmaxyx(stdscr,avrows,avcols);	
	htmCtrl->avrows = avrows;
	htmCtrl->avcols = avcols;

	// Initialize view objects
	ph::ControlBar ctrlBr(htmCtrl); 
	ph::StatusBar stBr(htmCtrl);
	ph::ContentPane cntPn(htmCtrl);

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
				htmCtrl->setModeIdx(1);		// F2: Switch to select mode
				ctrlBr.collapse(win[1]);
			} else if(key == KEY_F(3)) {
				htmCtrl->setModeIdx(0);		// F3: Switch to insert mode
				ctrlBr.collapse(win[1]);
			}
			if(htmCtrl->getModeIdx()==2) {		// Editing mode
				if(key== 46 || key >= 47 && key <= 57) {
					ctrlBr.numEntry(win[1],key);
				}				
			}
		 	if(htmCtrl->getModeIdx()==1 || htmCtrl->getModeIdx()==2) {
				switch(key) {
					case KEY_LEFT:
						ctrlBr.selLeft(win[1]);	
						break;
					case KEY_RIGHT:
						ctrlBr.selRight(win[1]);
						break;
					case KEY_UP:
						ctrlBr.selUp(win[1]);
						break;
					case KEY_DOWN:
						ctrlBr.selDown(win[1]);
						break;
					case KEY_ENTER: 
						ctrlBr.enter(win[1]);
						break;
					case KEY_LINE_FEED: 
						ctrlBr.enter(win[1]);
						break;
					case KEY_CARRIAGE_RETURN: 
						ctrlBr.enter(win[1]);
						break;
					default:
						htmCtrl->setStatusTxt(std::to_string(key));
						break;
					} 
				}
		}
		// HTM Update 
		auto sdr = encoder.encode(i%numcat);

		// Update screen dimension
		auto updt = updateScreen(win, htmCtrl);

		// Update status and control bar
		ctrlBr.print(win[1]);	
		stBr.print(win[2]);		
		cntPn.print(win[3],sdr);

		i += 1;
		// TODO Replace for better performing solution
		std::this_thread::sleep_for(std::chrono::milliseconds(100));	

	} // End of main loop 

	clrtoeol();
	refresh();
	endwin();
	return 0;
}


int updateScreen(WINDOW **win, const std::shared_ptr<ph::HtmController>& htmCtrl) {
	
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
