#pragma once

#include <string>
#include <xtensor/xarray.hpp>
#include <chrono>
#include <thread>
#include <ncursesw/ncurses.h>
#include <locale.h>

namespace ph {


const std::string ACTIVE{"\u25A0"};
const std::string INACTIVE{"\u25A1"};


struct ControlBar {
	
	ControlBar()  {
		cmdidx = 0;
		cmds.push_back("IPL");			// Input Layer
		cmds.push_back("SPL");			// Spacial Pooling Layer
		cmds.push_back("MC");			// Mini Column
	}

	void selLeft(WINDOW *ctrlwin) {
		cmdidx = (cmdidx == 0) ? cmds.size()-1 : cmdidx-1;
		print(ctrlwin);
	}

	void selRight(WINDOW *ctrlwin) {
		cmdidx = (cmdidx == cmds.size()-1) ? 0 : cmdidx+1;
		print(ctrlwin);
	}

	void print(WINDOW *ctrlwin) 
	{
		int x,y,i;
		x = 3; 
		y = 1;
		box(ctrlwin,0,0); 
		for(i=0;i<cmds.size();i++) {
			if(cmdidx == i) {
				wattron(ctrlwin,COLOR_PAIR(1)); 
				mvwprintw(ctrlwin,y,x,"%s",cmds.at(i).c_str());	
				wattroff(ctrlwin,COLOR_PAIR(1));
			} else {
				mvwprintw(ctrlwin,y,x,"%s",cmds.at(i).c_str());
			}
			x += 5;	
		}
		wrefresh(ctrlwin);
	}

protected:
	std::vector<std::string> cmds;
	int cmdidx;

};

struct StatusBar {

	StatusBar() {
	}

	void print(WINDOW *statwin, const std::string& msg)  {
		int x,y; 
		x = 3;
		y = 1;	
		box(statwin,0,0); 
		mvwprintw(statwin,y,x,msg.c_str());
		wrefresh(statwin);
	}	

};

struct ContentPane {

	ContentPane(size_t argncols, size_t argSdrSize) 
	: ncols(argncols), sdrSize(argSdrSize){}

	void print(WINDOW *contwin, const xt::xarray<bool> & sdr, const int avrows, const int avcols){
		int i; 
		int xi,yi;
		int xoff,yoff;
		int maxrows, maxcols;

		// Offset depending on HTM topology
		// We need NCOLS << 1 for whitespaces
		maxcols = avcols < ncols << 1 ? avcols : ncols << 1;   	
		maxrows = (sdrSize << 1) / maxcols; 

		yoff = (avrows - maxrows) >> 1;
		xoff = (avcols - maxcols) >> 1; 

		for(i = 0; i < sdr.size(); i++) {
			xi = (i << 1) % maxcols + xoff;
			yi = (i << 1) / maxcols + yoff;

			if(sdr[i]) {
				mvwprintw(contwin,yi,xi,ACTIVE.c_str());
				mvwaddch(contwin,yi,xi-1,' ');
			}
			else {
				mvwprintw(contwin,yi,xi,INACTIVE.c_str());
				mvwaddch(contwin,yi,xi-1,' ');
			}
		}	
		wrefresh(contwin);
	}

protected:	
	size_t ncols;
	size_t sdrSize;

	

};

}
