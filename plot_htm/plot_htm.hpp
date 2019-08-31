#pragma once

#include <string>
#include <xtensor/xarray.hpp>
#include <chrono>
#include <thread>
#include <ncursesw/ncurses.h>
#include <locale.h>

#define NCOLS 16
#define SDR_SIZE 256

namespace ph {


const std::string ACTIVE{"\u25A0"};
const std::string INACTIVE{"\u25A1"};

enum HTMParamKey {
	IDX,	// Index 
	ADY	// Activation Density
};

struct HTMParam {
	std::string name;
	std::string precision;
	float value;
};


inline void printControlBar(WINDOW *ctrlwin, const std::vector<std::string> & commands, const int cmdidx) 
{
	int x,y,i;
	x = 3; 
	y = 1;
	box(ctrlwin,0,0); 
	for(i=0;i<commands.size();i++) {
		if(cmdidx == i) {
			wattron(ctrlwin,COLOR_PAIR(1)); 
			mvwprintw(ctrlwin,y,x,"%s",commands.at(i).c_str());	
			wattroff(ctrlwin,COLOR_PAIR(1));
		} else {
			mvwprintw(ctrlwin,y,x,"%s",commands.at(i).c_str());
		}
		x += 5;	
	}
	wrefresh(ctrlwin);
}

inline void printStatusBar(WINDOW *statwin, const std::map<HTMParamKey, HTMParam> & params) {
	int x,y,i; 
	x = 3;
	y = 1;	
	box(statwin,0,0); 
	wattron(statwin,COLOR_PAIR(1));
	std::stringstream ss; 
	auto pos = params.find(HTMParamKey::IDX);
	if(pos != params.end()) {
		ss << pos->second.name << " = " << pos->second.precision; 
		const std::string& tmp = ss.str();   
		mvwprintw(statwin,y,x,tmp.c_str(), pos->second.value);
	}
	wattroff(statwin,COLOR_PAIR(1));
	wrefresh(statwin);
}	


inline void printSDR(WINDOW *contwin, const xt::xarray<bool> & sdr, const int avrows, const int avcols){
	int i; 
	int xi,yi;
	int xoff,yoff;
	int maxrows, maxcols;

	// Offset depending on HTM topology
	maxcols = avcols < NCOLS << 1 ? avcols : NCOLS << 1;    	// We need NCOLS << 1 for whitespaces
	maxrows = (SDR_SIZE << 1) / maxcols; 
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



}
