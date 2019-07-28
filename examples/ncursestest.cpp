#include "../../tiny_htm/tiny_htm/tiny_htm.hpp"
#include <iostream>
#include <xtensor/xarray.hpp>
#include <chrono>
#include <thread>
#include <ncurses.h>

#define NCOLS 16
#define ITERMAX 10000

void printSdr(const xt::xarray<bool> sdr);
const std::string ACTIVE{"\u25A0"};
const std::string INACTIVE{"\u25A1"};



int main(){

	long iter; 
	int xi, yi;
	int y[3], x[3];
	int index; 
	int maxlines, maxcols;

	// Initialize curses
	initscr();
	cbreak();
	noecho();
	clear();
	
	// Initialize triangle
	maxlines = LINES - 1;
	maxcols = COLS -1 ;

	y[0] = 0;
	x[0] = 0;

	y[1] = maxlines; 
	x[1] = maxcols / 2; 

	y[2] = 0; 
	x[2] = maxcols;

	mvaddch(y[0], x[0], '0');
	mvaddch(y[1], x[1], '1');
	mvaddch(y[2], x[2], '2');
	
	// Initialize yi,xi with random values 
	yi = std::rand()  % maxlines;
	xi = std::rand()  % maxcols;

	mvaddch(yi,xi, '.'); 

	// Iterate the triangle
	for(iter = 0; iter < ITERMAX; iter++) {

		index = std::rand() % 3;

		yi = (yi + y[index]) / 2;
		xi = (xi + x[index]) / 2;
	
		mvaddch(yi,xi,'*');
		refresh(); 
	}

	mvaddstr(maxlines, 0, "Press any key to quit"); 

	refresh(); 

	getch(); 
	endwin();

	exit(0); 

}



