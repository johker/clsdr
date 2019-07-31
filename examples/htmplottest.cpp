#include "../../tiny_htm/tiny_htm/tiny_htm.hpp"
#include <iostream>
#include <string>
#include <xtensor/xarray.hpp>
#include <chrono>
#include <thread>
#include <ncursesw/ncurses.h>
#include <locale.h>

#define NCOLS 16 

void printSDR(const xt::xarray<bool> & sdr, const int maxrows, const int maxcols, const int xoff, const int yoff);

const std::string ACTIVE{"\u25A0"};
const std::string INACTIVE{"\u25A1"};



int main(){

	int i;
	int sdrsize;
	int avrows,avcols;
	int xoff,yoff; 
	int maxrows,maxcols;

	setlocale(LC_ALL, "");
	initscr();
	getmaxyx(stdscr,avrows,avcols); 

	sdrsize = 256;
	th::ScalarEncoder scalarEncoder(0,100,sdrsize,12);

	maxcols = avcols < NCOLS << 1 ? avcols : NCOLS << 1;
	maxrows = (sdrsize << 1) / maxcols; 

	yoff = (avrows - maxrows) >> 1; 
	xoff = (avcols - maxcols) >> 1; 
	
	//mvprintw(avrows-2,0,"xoff = %d yoff = %d \n", xoff, yoff); 
	//mvprintw(avrows-3,0,"avcols= %d avrows = %d \n", avcols,avrows); 
	//mvprintw(avrows-4,0,"maxcols= %d maxrows = %d \n", maxcols,maxrows); 

	for(i = 0; i<100; i++) {
		auto sdr = scalarEncoder.encode(std::rand()%100); 
		printSDR(sdr,maxrows,maxcols,xoff,yoff);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
        	refresh();
	}
	
        getch();
        endwin();

	return 0;
}

void printSDR(const xt::xarray<bool> & sdr, const int maxrows, const int maxcols, const int xoff, const int yoff){
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







