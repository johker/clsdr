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

#define NCOLS 16
#define SDR_SIZE 256

const std::string ACTIVE{"\u25A0"};


int main(){

	int i = 0;		// main loop index	
	int sdrsize;
	int avrows,avcols;
	int xoff,yoff; 
	int maxrows,maxcols;
	WINDOW *controlwin, *statuswin, *contentwin;
	int cmdidx = 0;		// command index

	std::map<ph::HTMParamKey, ph::HTMParam> params;
	std::vector<std::string> commands;

	// Initialize View	
	setlocale(LC_ALL, "");		// Unicode support
	initscr();			// Init screen		
	clear();			// Clear terminal	 
	timeout(0);			// Non-blocking input
	noecho();			// No automatic printing
	cbreak();			// Immediate key input
	curs_set(0);			// Hide real cursor
	use_default_colors();		// Transparent background
	start_color();			// Use colors
	keypad(stdscr, 1);		// Fix keypad
	nonl();				// Get return key
	leaveok(stdscr,1);		// Dont care where cursor is left
	intrflush(stdscr,0);		// Avoid potential graphical issues

	getmaxyx(stdscr,avrows,avcols);	// Get screen dimensions

	// Initialize windows
	controlwin = newwin(3,avcols-2,0,1);
	statuswin = newwin(3,avcols-2,avrows-3,1);
	contentwin = newwin(avrows-6,avcols-2,3,1);

	box(controlwin,0,0);
	box(statuswin,0,0);
	box(contentwin,0,0);

	// Offset depending on HTM topology
	maxcols = avcols < NCOLS << 1 ? avcols : NCOLS << 1;
	maxrows = (SDR_SIZE << 1) / maxcols; 

	yoff = (avrows - maxrows) >> 1;
	xoff = (avcols - maxcols) >> 1; 

	init_pair(1, COLOR_BLACK, COLOR_WHITE);


	// Initialize Parameters
	ph::HTMParam idxParam = {"IDX","%.0f",0};
	params.insert({ph::HTMParamKey::IDX, idxParam}); 

	// Initialize Commands
	commands.push_back("IPL");	// Input Layer
	commands.push_back("SPL");	// Spacial Pooling Layer
	commands.push_back("MC");	// Mini Column

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
					ph::printControlBar(controlwin,commands,cmdidx); 
					break;
				case KEY_RIGHT:
					if (cmdidx == commands.size()-1) {
						cmdidx = 0;
					} else {
						cmdidx += 1;
					}
					ph::printControlBar(controlwin,commands,cmdidx); 
					break;
				default:
					//mvaddch(2,10,key);
					break;
				} 
			}
		}
	  
		//const std::string sidx = "cmdidx = %d";
		//mvprintw(10,10,ACTIVE.c_str());
		auto sdr = encoder.encode(i%numcat);
		auto pos = params.find(ph::HTMParamKey::IDX);
		if(pos != params.end()) {
			pos->second.value +=1;
		}		
		ph::printStatusBar(statuswin,params);
		if(i==1) {
			ph::printControlBar(controlwin,commands,cmdidx);	
		}
		ph::printSDR(contentwin,sdr,avrows-6,avcols-2,yoff,xoff);
		i += 1;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));	

	} // End of while

	endwin();
	clrtoeol();
	refresh();
	endwin();
	return 0;
}



























































































































