#include "terminalOutput.hpp"

namespace dh {

TerminalOutput::TerminalOutput(std::shared_ptr<HtmController> argHtmCtrl) : htmCtrl(argHtmCtrl) {
	startTerminal();
	addMenu();
}
TerminalOutput::~TerminalOutput() {
	stopTerminal();
}
bool TerminalOutput::startTerminal() {
	// Initialize View	
	setlocale(LC_ALL, "");					// Unicode support
	initscr();						// Init screen		
	clear();						// Clear terminal	 
	timeout(0);						// Non-blocking input
	noecho();						// No automatic printing
	cbreak();						// Immediate key input
	curs_set(0);						// Hide real cursor
	use_default_colors();					// Transparent background
	start_color();						// Use colors
	keypad(stdscr, 1);					// Fix keypad
	nonl();							// Get return key
	leaveok(stdscr,1);					// Dont care where cursor is left
	intrflush(stdscr,0);					// Avoid potential graphical issues
	
	// Initialize windows
	ctrlwin = newwin(3,avcols-2,0,1);			// Control window
	statuswin = newwin(3,avcols-2,avrows-3,1);		// Status window
	contentwin = newwin(avrows-6,avcols-2,3,1);		// Content window

	// Draw borders
	box(ctrlwin,0,0);
	box(statuswin,0,0);
	box(contentwin,0,0);

	// Get initial screen dimensions
	getmaxyx(stdscr,avrows,avcols);	
	htmCtrl->avrows = avrows;
	htmCtrl->avcols = avcols;
	
	// Initialize colors
	init_pair(1, COLOR_BLACK, COLOR_WHITE);

	// Start the thread
	done = false; 
	workerThread = new std::thread(TerminalOutput::worker, this);
	std::this_thread::sleep_for(std::chrono::milliseconds(5));
	return true;
}
bool TerminalOutput::stopTerminal() {
	if(!workerThread) {
		return true;
	}
	done = true;
	workerThread->join();
	workerThread = nullptr;
	return true;
}
int TerminalOutput::worker(TerminalOutput* argTerminal) {
	int i = 0;
	size_t max = 100; 
	do {
		// User Input
		while(1) { 
			int key = getch();
			if(key == ERR) break;
			if (key == KEY_F(2)) {
				argTerminal->htmCtrl->setMode(dh::SELECT);		// F2: Switch to select mode
				argTerminal->collapse();
			} else if(key == KEY_F(3)) {
				argTerminal->htmCtrl->setMode(dh::INSERT);		// F3: Switch to insert mode
				argTerminal->collapse();
			}
			if(argTerminal->htmCtrl->getMode()==dh::EDIT) {		
				if(key== KEY_DOT || key == KEY_BCKSPACE || key >= KEY_ZERO && key <= KEY_NINE) {
					argTerminal->numEntry(key);
				}				
			}
		 	if(argTerminal->htmCtrl->getMode() == dh::SELECT || argTerminal->htmCtrl->getMode() == dh::EDIT) {
				switch(key) {
					case KEY_LEFT:
						argTerminal->selLeft();	
						break;
					case KEY_RIGHT:
						argTerminal->selRight();
						break;
					case KEY_UP:
						argTerminal->selUp();
						break;
					case KEY_DOWN:
						argTerminal->selDown();
						break;
					case KEY_ENTER: 
						argTerminal->enter();
						break;
					case KEY_LINE_FEED: 
						argTerminal->enter();
						break;
					case KEY_CARRIAGE_RETURN: 
						argTerminal->enter();
						break;
					default:
						argTerminal->htmCtrl->setStatusTxt(std::to_string(key));
						break;
					} 
				}
			if(argTerminal->htmCtrl->getMode() == dh::INSERT) {
				
			}
		}
		// HTM Update 
		auto sdr = argTerminal->htmCtrl->getScalarEncoder()->encode(i%max);

		// Update screen dimension
		argTerminal->updateScreen();

		// Update status and control bar
		argTerminal->printControlBar();	
		argTerminal->printStatusBar();		
		argTerminal->printContentPane(sdr);

		i += 1;
		
		std::this_thread::sleep_for(std::chrono::milliseconds(100));	

	} while(!argTerminal->done);
	clrtoeol();
	refresh();
	endwin();
	return true;
}
void TerminalOutput::addMenu() {
	auto root = MenuItem::create(KEY_MENU, std::shared_ptr<MenuItem>());
	menuStack.push_back(root);
	// PARAMS
	auto params = MenuItem::create(KEY_PARAMS, root);
	addParamsMenu(params);
	// VIEW
	auto view = MenuItem::create(KEY_VIEW, root);
	std::vector<std::string> options = {KEY_INPUT, KEY_SPOOL};
	SelectItem::create(KEY_LAYER, view, htmCtrl, options);
	ParamItem::create(KEY_NUMCOL, view, htmCtrl);
	selItem = 0;
}
void TerminalOutput::addParamsMenu(std::shared_ptr<MenuItem> params) {
	if(htmCtrl->getEncoderType() == SCALAR) {
		ParamItem::create(KEY_MINVAL, params, htmCtrl);
		ParamItem::create(KEY_MAXVAL, params, htmCtrl);
		ParamItem::create(KEY_SDRLEN, params, htmCtrl);
		ParamItem::create(KEY_ACTBTS, params, htmCtrl);				
	} 
	// TODO: Add more encoder menues
	
}
void TerminalOutput::selUp() {
	auto& mi = menuStack.back()->children.at(selItem);
	if(htmCtrl->getMode()== EDIT && mi->type==2) {
		const auto& si = std::static_pointer_cast<SelectItem>(mi);
		si->prevValue();
	} else {
		keyInput = "";		// Clear keyboard input
		htmCtrl->setMode(SELECT);
		selItem = (selItem == 0) ? menuStack.back()->children.size()-1 : selItem-1;
		menuStack.back()->selChild = selItem;
	}
	printControlBar();
}
void TerminalOutput::selDown() {
	auto& mi = menuStack.back()->children.at(selItem);
	if(htmCtrl->getMode()== EDIT && mi->type==2) {
		const auto& si = std::static_pointer_cast<SelectItem>(mi);
	si->nextValue();
	} else {
		keyInput = "";		// Clear keyboard input
		htmCtrl->setMode(SELECT);	
		selItem = (selItem == menuStack.back()->children.size()-1) ? 0 : selItem+1;
		menuStack.back()->selChild = selItem;
	}
	printControlBar();
}
void TerminalOutput::selLeft() {
	if(menuStack.size()>1) {
		menuStack.pop_back();
		selItem = menuStack.back()->selChild;
	} else {
		collapsed = true;
	}
	keyInput = "";		// Clear keyboard input
	htmCtrl->setMode(SELECT);
	printControlBar();
}
void TerminalOutput::selRight() {
	if(collapsed) {
		collapsed = false;
	}
	else if(!menuStack.back()->isLeaf) {
		auto mi = std::static_pointer_cast<MenuItem>(menuStack.back()->children.at(selItem));
		menuStack.push_back(mi);
		selItem = 0;
	}	
	printControlBar();
}
void TerminalOutput::numEntry(int key) {
	if(key == KEY_DOT && keyInput.find('.') != std::string::npos) {
		return; 		// Dont allow multiple dots		
	} 
	if(key == KEY_BCKSPACE) {
		if(keyInput.size()>0) {
			keyInput.pop_back();
		}
	} else {
		keyInput.append(std::string(1, char(key)));
	}	
	printControlBar();
}
void TerminalOutput::enter() {
	if(collapsed || !menuStack.back()->isLeaf) { 
		selRight();
	} else {
		const auto& pi = std::static_pointer_cast<ParamItem>(menuStack.back()->children.at(selItem));
		if(htmCtrl->getMode()== EDIT) {
			if(keyInput.size()>0) {
				htmCtrl->setValue(pi->name, std::stof(keyInput));
			}
			keyInput = "";			
			htmCtrl->setMode(SELECT);
		} else {
			keyInput = pi->getValue();	
			htmCtrl->setMode(EDIT);
		}
	}
	printControlBar();
}
void TerminalOutput::collapse() {
	menuStack.resize(1);
	collapsed = true;
	printControlBar();
}
void TerminalOutput::printControlBar() 
{
	wclear(ctrlwin);
	int i = 0, x = 3, y = 1;
	box(ctrlwin,0,0); 
	for(auto& menuItem : menuStack) { 	// Menu > Submenu > ... > Leaf : (Parameter)
		mvwprintw(ctrlwin,y,x,"%s",menuItem->name);	
		x += strlen(menuItem->name)+1;
		mvwprintw(ctrlwin,y,x,"%s",SEP_MEN);	
		x += 3;
		i++;
	}
	if(!collapsed) {			
		const auto& mi = menuStack.back()->children.at(selItem); 
		wattron(ctrlwin,COLOR_PAIR(1));			// Highlight last element
		mvwprintw(ctrlwin,y,x,"%s",mi->name);
		x += strlen(mi->name)+1;
		wattroff(ctrlwin,COLOR_PAIR(1));
		if(mi->type == 1) {	// Numerical entry 
			const auto& pi = std::static_pointer_cast<ParamItem>(mi);
			mvwprintw(ctrlwin,y,x,"%s",SEP_PRM);
			x += 3;
			if(htmCtrl->getMode()== EDIT) {
				wattron(ctrlwin, A_BLINK);
				if(keyInput.size() > 0) {
					mvwprintw(ctrlwin,y,x,"%s",keyInput.c_str());
			       }
			} else {	
				mvwprintw(ctrlwin,y,x,"%s",pi->getValue());	
			}
			wattroff(ctrlwin, A_BLINK);
		} else if(mi->type == 2) {	// Selection 
		       const auto& si = std::static_pointer_cast<SelectItem>(mi);
			mvwprintw(ctrlwin,y,x,"%s",SEP_PRM);
			x += 3;
			if(htmCtrl->getMode()== EDIT) {
				wattron(ctrlwin,A_BLINK);
			}
			mvwprintw(ctrlwin,y,x,"%s",si->getValue());
			wattroff(ctrlwin, A_BLINK); 
		} else {
			mvwprintw(ctrlwin,y,x,"%s",SEP_MEN);		// Add menu separator
		}
	}
	wrefresh(ctrlwin);
}
void TerminalOutput::printStatusBar()  {
	wclear(statuswin);
	int x,y; 
	x = 3;
	y = 1;	
	box(statuswin,0,0); 
	mvwprintw(statuswin,y,x,htmCtrl->getModeTxt().c_str());
	x += 7;
	mvwprintw(statuswin,y,x,htmCtrl->getStatusTxt().c_str());
	wrefresh(statuswin);
}	
void TerminalOutput::printContentPane(const xt::xarray<bool> & sdr){
	int i; 
	int xi,yi;
	int xoff,yoff;
	int maxrows, maxcols;

	wclear(contentwin);
	// Offset depending on HTM topology
	// We need NCOLS << 1 for whitespaces
	maxcols = htmCtrl->avcols-2 < htmCtrl->ncols << 1 ? htmCtrl->avcols-2 : htmCtrl->ncols << 1;   	
	maxrows = (htmCtrl->sdrSize << 1) / maxcols;

	yoff = (htmCtrl->avrows-6 - maxrows) >> 1;
	xoff = (htmCtrl->avcols-2 - maxcols) >> 1; 

	for(i = 0; i < sdr.size(); i++) {
		xi = (i << 1) % maxcols + xoff;
		yi = (i << 1) / maxcols + yoff;

		if(sdr[i]) {
			mvwprintw(contentwin,yi,xi,SYM_ACTIVE);
			mvwaddch(contentwin,yi,xi-1,' ');
		}
		else {
			mvwprintw(contentwin,yi,xi,SYM_INACTIVE);
			mvwaddch(contentwin,yi,xi-1,' ');
		}
	}	
	wrefresh(contentwin);
}
int TerminalOutput::updateScreen() {
	getmaxyx(stdwin,htmCtrl->avrows,htmCtrl->avcols);	// Get screen dimensions
	if(htmCtrl->avrows != htmCtrl->avrowstmo || htmCtrl->avcols != htmCtrl->avcolstmo) {
		wresize(ctrlwin,3,htmCtrl->avcols-2);
		wresize(statuswin,3,htmCtrl->avcols-2);
		wresize(contentwin,htmCtrl->avrows-6,htmCtrl->avcols-2);
		mvwin(statuswin,htmCtrl->avrows-3,1); 
		wclear(stdwin);
		wclear(ctrlwin);
		box(ctrlwin,0,0);
		wrefresh(ctrlwin);
		wclear(statuswin);
		box(statuswin,0,0);
		wrefresh(statuswin);
		wclear(contentwin);
		box(contentwin,0,0);
		wrefresh(contentwin); 
		htmCtrl->avrowstmo = htmCtrl->avrows;
		htmCtrl->avcolstmo = htmCtrl->avcols;
		return 1;
	}
	return 0;
}

}
