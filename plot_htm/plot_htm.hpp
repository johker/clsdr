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
	IDX,
	ADY

};

struct HTMParam {
	std::string name;
	std::string precision;
	float value;
};

inline void printStatus(const std::map<HTMParamKey, HTMParam> & status, const int avrows, const int avcols) {
	attron(COLOR_PAIR(1));
	std::stringstream ss; 
	auto pos = status.find(HTMParamKey::IDX);
	if(pos != status.end()) {
		ss << pos->second.name << " = " << pos->second.precision; 
		const std::string& tmp = ss.str();   
		mvprintw(avrows-2,0,tmp.c_str(), pos->second.value);
	}
	attroff(COLOR_PAIR(1));
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
