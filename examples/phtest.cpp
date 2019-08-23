#include "../../tiny_htm/tiny_htm/tiny_htm.hpp"
#include "../plot_htm/plot_htm.hpp"
#include <iostream>
#include <string>
#include <xtensor/xarray.hpp>
#include <chrono>
#include <thread>
#include <ncursesw/ncurses.h>
#include <locale.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define NCOLS 16
#define SDR_SIZE 256


struct input_line {
    char *ln;
    int length;
    int capacity;
    int cursor;
    int last_rendered;
};

int simpleDemo(int maxrows, int maxcols, int xoff, int yoff);
void make_buffer(struct input_line *buf); 
void destroy_buffer(struct input_line *buf);
void render_line(struct input_line *buf);
int retrieve_content(struct input_line *buf, char *target, int max_len);
void add_char(struct input_line *buf, char ch);
int handle_input(struct  input_line *buf, char *target, int max_len, int key);
int get_line_non_blocking(struct input_line *buf, char *target, int max_len);


int main(){

	int i = 0;		// main loop index	
        int sdrsize;
        int avrows,avcols;
        int xoff,yoff; 
        int maxrows,maxcols;
	WINDOW *controlwin; 
	WINDOW *statuswin;
	int cmdidx = 0;		// command index
	int c;			// keyboard input 

	std::map<ph::HTMParamKey, ph::HTMParam> params;
	std::vector<std::string> commands;

	// Initialize View	
        initscr();		
	clear();			// Clear terminal	 
        setlocale(LC_ALL, "");		// Unicode support
	noecho(); 			// No automatic printing
	cbreak(); 			// Immediate key input
	curs_set(0);			// Hide real cursor
	use_default_colors();		// Transparent background
	start_color();			// Use colors
	keypad(stdscr, 1);		// Fix keypad
	nonl(); 			// Get return key
	leaveok(stdscr,1);		// Dont care where cursor is left
	intrflush(stdscr,0);		// Avoid potential graphical issues



	getmaxyx(stdscr,avrows,avcols);	// Get screen dimensions

	// Initialize windows
	controlwin = newwin(1,avcols,0,0);
	statuswin = newwin(1,avcols,avrows,0);

	// Offset depending on HTM topology
        maxcols = avcols < NCOLS << 1 ? avcols : NCOLS << 1;
        maxrows = (SDR_SIZE << 1) / maxcols; 

        yoff = (avrows - maxrows) >> 1;
        xoff = (avcols - maxcols) >> 1; 

	init_pair(1, COLOR_BLACK, COLOR_WHITE);


	// Initialize Parameters
	ph::HTMParam idxParam = {"IDX","%.0f",0};
	params.insert({ph::HTMParamKey::IDX, idxParam}); 

	// Initialize Commands
	commands.push_back("IPL"); 	// Input Layer
	commands.push_back("SPL"); 	// Spacial Pooling Layer

	// HTM Test 	
	size_t numcat = 2;
	size_t enclen = 4;
	size_t phorizon = 10; 

	th::CategoryEncoder encoder(numcat, enclen);
	th::TemporalMemory tm({numcat*4}, 6);

	struct input_line lnbuffer;
	make_buffer(&lnbuffer);	
	
	// User Input
	refresh();
	while(1)
	{
		// Get input non-blocking
		while(1) {

		int key = getch();
		if(key == ERR) {
			break;
		} else {

			switch(c)
				{
				case KEY_LEFT:
					if (cmdidx == 0) {
						cmdidx = commands.size()-1;
					} else {
						cmdidx -= 1;
					}
					break;
				case KEY_RIGHT:
					if (cmdidx == commands.size()-1) {
						cmdidx = 0;
					} else {
						cmdidx += 1;
					}
					break;
				default:
					mvaddch(2,10,key);
					refresh();
					break;
				}
			}
		}
		mvaddch(2, 2, '0' + (rand() % 10));  
		//auto sdr = encoder.encode(i%numcat);
		//auto pos = params.find(ph::HTMParamKey::IDX);
		//if(pos != params.end()) {
		//	pos->second.value +=1;
		//}
		//ph::printControlBar(controlwin,commands,cmdidx,avrows,avcols); 
		////ph::printStatusBar(statuswin,params,avrows,avcols);
		//ph::printSDR(sdr,maxrows,maxcols,xoff,yoff);

		//i += 1;
		////std::this_thread::sleep_for(std::chrono::milliseconds(1000));	

	} // End of while

        endwin();
	clrtoeol();
	refresh();
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









void make_buffer(struct input_line *buf) {
    buf->ln = NULL;
    buf->length = 0;
    buf->capacity = 0;
    buf->cursor = 0;
    buf->last_rendered = 0;
}

void destroy_buffer(struct input_line *buf) {
    free(buf->ln);
    make_buffer(buf);
}

void render_line(struct input_line *buf) {
    int i = 0;
    for(; i < buf->length; i ++) {
        chtype c = buf->ln[i];
        if(i == buf->cursor) {
            c |= A_REVERSE;
        }
        addch(c);
    }
    if(buf->cursor == buf->length) {
        addch(' ' | A_REVERSE);
        i ++;
    }
    int rendered = i;
    // Erase previously rendered characters
    for(; i < buf->last_rendered; i ++) {
        addch(' '); 
    }
    buf->last_rendered = rendered;
}

int retrieve_content(struct input_line *buf, char *target, int max_len) {
    int len = buf->length < (max_len - 1) ? buf->length : (max_len - 1);
    memcpy(target, buf->ln, len);
    target[len] = '\0';
    buf->cursor = 0;
    buf->length = 0;
    return len + 1;
}

void add_char(struct input_line *buf, char ch) {
    // Ensure enough space for new character
    if(buf->length == buf->capacity) {
        int ncap = buf->capacity + 128;
        char *nln = (char*) realloc(buf->ln, ncap);
        if(!nln) {
            // Out of memory!
            return;
        }
        buf->ln = nln;
        buf->capacity = ncap;
    }

    // Add new character
    memmove(
        &buf->ln[buf->cursor+1],
        &buf->ln[buf->cursor],
        buf->length - buf->cursor
    );
    buf->ln[buf->cursor] = ch;
    ++ buf->cursor;
    ++ buf->length;
}

int handle_input(struct input_line *buf, char *target, int max_len, int key) {
    if(!(key & KEY_CODE_YES) && isprint(key)) {
        add_char(buf, key);
        return 0;
    }

    switch(key) {
    case ERR: /* no key pressed */ break;
    case KEY_LEFT:  if(buf->cursor > 0)           { buf->cursor --; } break;
    case KEY_RIGHT: if(buf->cursor < buf->length) { buf->cursor ++; } break;
    case KEY_HOME:  buf->cursor = 0;           break;
    case KEY_END:   buf->cursor = buf->length; break;
    case '\t':
        add_char(buf, '\t');
        break;
    case KEY_BACKSPACE:
    case 127:
    case 8:
        if(buf->cursor <= 0) {
            break;
        }
        buf->cursor --;
        // Fall-through
    case KEY_DC:
        if(buf->cursor < buf->length) {
            memmove(
                &buf->ln[buf->cursor],
                &buf->ln[buf->cursor+1],
                buf->length - buf->cursor - 1
            );
            buf->length --;
        }
        break;
    case KEY_ENTER:
    case '\r':
    case '\n':
        return retrieve_content(buf, target, max_len);
    }
    return 0;
}

int get_line_non_blocking(struct input_line *buf, char *target, int max_len) {
    while(1) {
        int key = getch();
        if(key == ERR) {
            // No more input
            return 0;
        }
        int n = handle_input(buf, target, max_len, key);
        if(n) {
            return n;
        }
    }
}
