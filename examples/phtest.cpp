#include "../../tiny_htm/tiny_htm/tiny_htm.hpp"
#include "../plot_htm/plot_htm.hpp"
#include <iostream>
#include <string>
#include <xtensor/xarray.hpp>
#include <chrono>
#include <thread>
#include <ncursesw/ncurses.h>
#include <locale.h>

#define NCOLS 16
#define SDR_SIZE 256

int simpleDemo(int maxrows, int maxcols, int xoff, int yoff);

int main(){

        int sdrsize;
        int avrows,avcols;
        int xoff,yoff; 
        int maxrows,maxcols;

        setlocale(LC_ALL, "");
        initscr();
        getmaxyx(stdscr,avrows,avcols);

        maxcols = avcols < NCOLS << 1 ? avcols : NCOLS << 1;
        maxrows = (SDR_SIZE << 1) / maxcols; 

        yoff = (avrows - maxrows) >> 1;
        xoff = (avcols - maxcols) >> 1; 

        //mvprintw(avrows-2,0,"xoff = %d yoff = %d \n", xoff, yoff); 
        //mvprintw(avrows-3,0,"avcols= %d avrows = %d \n", avcols,avrows); 
        //mvprintw(avrows-4,0,"maxcols= %d maxrows = %d \n", maxcols,maxrows); 



	simpleDemo(maxrows, maxcols, xoff,yoff); 




        getch();
        endwin();

        return 0;
}

int simpleDemo(int maxrows, int maxcols, int xoff, int yoff) {
	int i;
	th::ScalarEncoder scalarEncoder(0,100,SDR_SIZE,12);
	for(i = 0; i<100; i++) {
                auto sdr = scalarEncoder.encode(std::rand()%100);
        	ph::printSDR(sdr,maxrows,maxcols,xoff,yoff);
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                refresh();
        }
	return 0;
}      
