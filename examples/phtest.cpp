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




const std::string ACTIVE{"\u25A0"};

void updateScreen(WINDOW **win, int &avrows, int &avcols, int &avrowstmo, int &avcolstmo); 
void initNcurses();

int main(){

	int i = 0;						// main loop index	
	int sdrsize;
	int avrows,avcols;					// Available rows / cols
	int avrowstmo = 0, avcolstmo = 0;			// Available rows / cols in t minus one
	int xoff,yoff;						// Offset 
	int maxrows,maxcols;
	WINDOW *win[4];						// Windows: std, control, status, content
	WINDOW *controlwin, *statuswin, *contentwin;
	int cmdidx = 0;						// command index

	std::map<ph::HTMParamKey, ph::HTMParam> params;
	std::vector<std::string> commands;

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

	// Get initial screen dimensions
	getmaxyx(stdscr,avrows,avcols);	

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

	// Initialize Parameters
	ph::HTMParam idxParam = {"IDX","%.0f",0};
	params.insert({ph::HTMParamKey::IDX, idxParam}); 

	// Initialize Commands
	commands.push_back("IPL");				// Input Layer
	commands.push_back("SPL");				// Spacial Pooling Layer
	commands.push_back("MC");				// Mini Column

	// HTM Test	
	size_t numcat = 2;
	size_t enclen = 4;
	size_t phorizon = 10; 

	th::CategoryEncoder encoder(numcat, enclen);
	th::TemporalMemory tm({numcat*4}, 6);

				
	while(1) {
	
		// Get input non-blocking
		while(1) 
		{
		int key = getch();
		if(key == ERR) 
		{
			break;
		} else {

			switch(key)
				{
				case KEY_LEFT:
					if (cmdidx == 0) {
						cmdidx = commands.size()-1;
					} else {
						cmdidx -= 1;
					}
					ph::printControlBar(win[1],commands,cmdidx); 
					break;
				case KEY_RIGHT:
					if (cmdidx == commands.size()-1) {
						cmdidx = 0;
					} else {
						cmdidx += 1;
					}
					ph::printControlBar(win[1],commands,cmdidx); 
					break;
				default:
					//mvaddch(2,10,key);
					break;
				} 
			}
		}
		// HTM Update 
		auto sdr = encoder.encode(i%numcat);
		auto pos = params.find(ph::HTMParamKey::IDX);
		if(pos != params.end()) {
			pos->second.value +=1;
		}		
		// Update screen dimension
		updateScreen(win, avrows,avcols,avrowstmo,avcolstmo);


		// Update status and control bar
		ph::printStatusBar(win[2],params);
		if(i==1) {
			ph::printControlBar(win[1],commands,cmdidx);	
		}
		ph::printSDR(win[3],sdr,avrows-6,avcols-2);

		i += 1;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));	

	} // End of while

	endwin();
	clrtoeol();
	refresh();
	endwin();
	return 0;
}


void updateScreen(WINDOW **win, int &avrows, int &avcols, int &avrowstmo, int &avcolstmo) {
	
	getmaxyx(win[0],avrows,avcols);	// Get screen dimensions
	if(avrows != avrowstmo || avcols != avcolstmo) {
		// Redraw everything
		// wresize(, lines, colums)
	}
}
