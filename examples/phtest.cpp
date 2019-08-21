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
	std::vector<ph::HTMParam> status;
	

        setlocale(LC_ALL, "");
        initscr();
	use_default_colors();
	start_color();		
        getmaxyx(stdscr,avrows,avcols);

        maxcols = avcols < NCOLS << 1 ? avcols : NCOLS << 1;
        maxrows = (SDR_SIZE << 1) / maxcols; 

        yoff = (avrows - maxrows) >> 1;
        xoff = (avcols - maxcols) >> 1; 

	init_pair(1, COLOR_BLACK, COLOR_WHITE);
	
//        mvprintw(avrows-2,0,"xoff = %d yoff = %d \n", xoff, yoff); 
//        mvprintw(avrows-3,0,"avcols= %d avrows = %d \n", avcols,avrows); 
//        mvprintw(avrows-4,0,"maxcols= %d maxrows = %d \n", maxcols,maxrows); 

	ph::HTMParam idxParam = {"IDX","%.0f",0};
	status.push_back(idxParam); 
	
	size_t numcat = 2;
	size_t enclen = 4;
	size_t phorizon = 10; 

	th::CategoryEncoder encoder(numcat, enclen);
	for(int i=0; i<phorizon; i++) {
		auto sdr = encoder.encode(i%numcat);
		status[0].value +=1;
		ph::printStatus(status,avrows,avcols);
		ph::printSDR(sdr,maxrows,maxcols,xoff,yoff);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));	
		refresh();
	}

	if(has_colors() == FALSE)
	{	endwin();
		printf("Your terminal does not support color\n");
		exit(1);
	}
	

//	simpleDemo(maxrows, maxcols, xoff,yoff); 

        getch();
        endwin();

        return 0;
}

int simpleDemo(int maxrows, int maxcols, int xoff, int yoff) {
	int i;
	th::ScalarEncoder scalarEncoder(0,100,SDR_SIZE,12);
	for(i = 0; i<100; i++) {
		xt::xarray<bool> sdr = scalarEncoder.encode(std::rand()%100);
        	ph::printSDR(sdr,maxrows,maxcols,xoff,yoff);
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                refresh();
        }
	return 0;
}      
