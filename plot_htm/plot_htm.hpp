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
		ph::HTMParam idxParam = {"IDX","%.0f",0};
		params.insert({ph::HTMParamKey::IDX, idxParam}); 
	}

	int getStatus(HTMParamKey &key, float value) {
		auto pos = params.find(key);
		if(pos != params.end()) {
			value =	pos->second.value;
			return 0;
		} else return 1;
	}

	int setStatus(WINDOW *statwin, HTMParamKey key, float value) {
		auto pos = params.find(key);
		if(pos != params.end()) {
			pos->second.value = value;
			print(statwin);
			return 0;
		} else return 1;
	}

	void print(WINDOW *statwin)  {
		int x,y,i; 
		x = 3;
		y = 1;	
		box(statwin,0,0); 
		std::stringstream ss; 
		for(auto const& p : params) {
			ss << p.second.name << " = " << p.second.precision; 
			const std::string& tmp = ss.str();   
			mvwprintw(statwin,y,x,tmp.c_str(), p.second.value);
		}
		wrefresh(statwin);
	}	

protected: 
	std::map<HTMParamKey, HTMParam> params;

};

struct ContentPane {

	ContentPane() {

	}

	void print(WINDOW *contwin, const xt::xarray<bool> & sdr, const int avrows, const int avcols){
		int i; 
		int xi,yi;
		int xoff,yoff;
		int maxrows, maxcols;

		// Offset depending on HTM topology
		// We need NCOLS << 1 for whitespaces
		maxcols = avcols < NCOLS << 1 ? avcols : NCOLS << 1;   	
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

};

}
