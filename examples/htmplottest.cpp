#include "../../tiny_htm/tiny_htm/tiny_htm.hpp"
#include <iostream>
#include <string>
#include <xtensor/xarray.hpp>
#include <chrono>
#include <thread>
#include <ncursesw/ncurses.h>
#include <locale.h>

#define NCOLS 16 

void printSdr(const xt::xarray<bool> sdri, const int maxcols);

const std::string ACTIVE{"\u25A0"};
const std::string INACTIVE{"\u25A1"};



int main(){
	int avrows,avcols;
	int i; 
	int xi,yi;
	int xoff,yoff; 
	int maxrows,maxcols;

	setlocale(LC_ALL, "");
	initscr();
	getmaxyx(stdscr,avrows,avcols); 


	th::ScalarEncoder scalarEncoder(0,100,256,12);
	auto sdr = scalarEncoder.encode(54); 

	maxcols = avcols < NCOLS << 1 ? avcols : NCOLS << 1;
	maxrows = (sdr.size() << 1) / maxcols; 

	yoff = (avrows - maxrows) >> 1; 
	xoff = (avcols - maxcols) >> 1; 
	
	
	mvprintw(avrows-2,0,"xoff = %d yoff = %d \n", xoff, yoff); 
	mvprintw(avrows-3,0,"avcols= %d avrows = %d \n", avcols,avrows); 
	mvprintw(avrows-4,0,"maxcols= %d maxrows = %d \n", maxcols,maxrows); 

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
        
        refresh();
        getch();
        endwin();

	return 0;
}

void printSdr(const xt::xarray<bool> sdr, const int maxcols) {
	std::string status;
	int i, xi, yi;
	for(i = 0; i < sdr.size(); i++) {
		xi = i % maxcols; 
		yi = i / maxcols;
		if(sdr[i]) {
			mvaddch(yi,xi,'*');
			refresh();
			//status = "0";//ACTIVE;
		}	
		else {
			//status = "1";//INACTIVE;
			mvaddch(yi,xi,'0');
			refresh();
		}
		// mvaddstr(yi,xi,status.c_str());
		refresh();
	}
}








