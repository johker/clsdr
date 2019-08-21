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
	std::map<ph::HTMParamKey, ph::HTMParam> status;
	
	// Initialize View	
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
	ph::HTMParam idxParam = {"IDX","%.0f",0};
	status.insert({ph::HTMParamKey::IDX, idxParam}); 

	// HTM Test 	
	size_t numcat = 2;
	size_t enclen = 4;
	size_t phorizon = 10; 

	th::CategoryEncoder encoder(numcat, enclen);
	th::TemporalMemory tm({numcat*4}, 6);
	ph::printControlBar(avrows,avcols);
	for(int i=0; i<phorizon; i++) {
		auto sdr = encoder.encode(i%numcat);
		auto pos = status.find(ph::HTMParamKey::IDX);
		if(pos != status.end()) {
			pos->second.value +=1;
			ph::printStatusBar(status,avrows,avcols);
		}
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
