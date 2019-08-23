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

enum HTMParamKey {
	IDX,	// Index 
	ADY	// Activation Density
};

struct HTMParam {
	std::string name;
	std::string precision;
	float value;
};


inline void printControlBar(WINDOW *ctrlwin, const std::vector<std::string> & commands, const int cmdidx, const int avrows, const int avcols) 
{
	int x,y,i;
	x = 0; 
	y = 0;
	box(ctrlwin,0,0); 
	for(i=0;i<commands.size();i++) {
		if(cmdidx == i) {
			wattron(ctrlwin,A_REVERSE); 
			mvprintw(y,x,"%s",commands.at(i).c_str());	
			wattroff(ctrlwin,A_REVERSE);
		} else {
			mvprintw(y,x,"%s",commands.at(i).c_str());
		}
		x += 5;	
	}
	wrefresh(ctrlwin);
}

inline void printStatusBar(WINDOW *statwin, const std::map<HTMParamKey, HTMParam> & params, const int avrows, const int avcols) {
	attron(COLOR_PAIR(1));
	std::stringstream ss; 
	auto pos = params.find(HTMParamKey::IDX);
	if(pos != params.end()) {
		ss << pos->second.name << " = " << pos->second.precision; 
		const std::string& tmp = ss.str();   
		mvprintw(avrows-1,0,tmp.c_str(), pos->second.value);
	}
	attroff(COLOR_PAIR(1));
	wrefresh(statwin);
	wrefresh(statwin);
}	


inline void printSDR(const xt::xarray<bool> & sdr, const int maxrows, const int maxcols, const int xoff, const int yoff){
	int i; 
	int xi,yi;
	for(i = 0; i < sdr.size(); i++) {
                xi = (i << 1) % maxcols + xoff;
                yi = (i << 1) / maxcols + yoff;

                if(sdr[i]) {
                        mvprintw(yi,xi,ACTIVE.c_str());
                        mvaddch(yi,xi-1,' ');
                }
                else {
                        mvprintw(yi,xi,INACTIVE.c_str());
                        mvaddch(yi,xi-1,' ');
                }
	}	
}



}
