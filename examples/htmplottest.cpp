#include "../../tiny_htm/tiny_htm/tiny_htm.hpp"
#include <iostream>
#include <xtensor/xarray.hpp>
#include <chrono>
#include <thread>
#include <ncurses.h>

#define NCOLS 16

void countdown(tm *ltm); 
void printSdr(const xt::xarray<bool> sdri, const int maxcols);
void manipulatePrevLine();
const std::string ACTIVE{"\u25A0"};
const std::string INACTIVE{"\u25A1"};



int main(){
	int maxlines, maxcols; 
	th::ScalarEncoder scalarEncoder(0,100,256,12);

	
	// Initialize curses
	initscr();
	cbreak();
	noecho();
	clear();  
	
	maxcols = COLS -1 ;  	
	printSdr(scalarEncoder.encode(54), maxcols);


}

void countdown(tm *ltm)
{
	int s = ltm->tm_sec;
	std::string status;
	if(s%2==0) {
		status = ACTIVE;
	} else {
		status = INACTIVE;
	}

	std::cout << '\r'
		  << status
		  << std::flush;
}

void printSdr(const xt::xarray<bool> sdr, const int maxcols) {
	std::string status;
	int i, xi, yi;
	for(i = 0; i < sdr.size(); i++) {
		xi = i % maxcols; 
		yi = i / maxcols;
		if(sdr[i])	
			status = ACTIVE;
		else 
			status = INACTIVE;
		mvaddstr(xi,yi,status.c_str());
	}
}

void manipulatePrevLine() {

	int i; 
	std::cout << std::to_string(0) << std::endl; 
	std::cout << std::to_string(0) << std::endl; 
	for (i = 0; i < 100; i++) {
		std::cout << "\e[A" << " up " << std::to_string(i) << std::endl; 
		std::cout << "\e[A" << " down " << std::to_string(i) << std::endl; 
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}
