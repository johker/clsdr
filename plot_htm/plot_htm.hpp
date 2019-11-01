#pragma once

#include <string>
#include <chrono>
#include <thread>
#include <memory>
#include <algorithm>
#include <xtensor/xarray.hpp>
#include <ncursesw/ncurses.h>
#include <locale.h>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

namespace ph {

const std::string ACTIVE{"\u25A0"};		// Active Minicolumn 
const std::string INACTIVE{"\u25A1"};		// Inactive minicolumn
const std::string SEPMEN{"\u25B8"};		// Menu Separator
const std::string SEPSEL{"\u25BE"};		// Selection Separator
const std::string SEPPRM{":"};			// Parameter Separator

class Item {
public:
	const std::string name; 
protected:
	Item(const std::string &name) : name(name) {}
};

class MenuItem : public Item {
public:	
	static std::shared_ptr<MenuItem> create(const std::string &argName, std::shared_ptr<MenuItem> parent) {
		auto mi = std::shared_ptr<MenuItem>(new MenuItem(argName));
		if(parent != nullptr) {
			parent->children.push_back(mi);
		}	
		return mi;
	}
	std::vector<std::shared_ptr<Item>> children;
	int selChild = 0;
	bool isLeaf = false;
private:
	MenuItem(const std::string &name): Item(name) {}
};

class ParamItem : public Item {
public:
	static std::shared_ptr<ParamItem> create(const std::string &argName, std::shared_ptr<MenuItem> parent, bool argSelection) {
		auto pi = std::shared_ptr<ParamItem>(new ParamItem(argName));
		if(parent != nullptr) {
			parent->children.push_back(pi);
			parent->isLeaf = true;
			pi->isSelection = argSelection;
		}
		return pi;
	}
	bool isSelection = false;
	double value = 0.0;
	std::string key;
private:
	ParamItem(const std::string &name): Item(name) {}
};

struct ControlBar {
	
	ControlBar()  {
		addMenu();
	}

	void addMenu() {
		// TODO: Link to HTM object
		auto root = MenuItem::create("", std::shared_ptr<MenuItem>());
		menuStack.push_back(root);
	       	auto layers = MenuItem::create("LAYERS", root);
		auto params = MenuItem::create("PARAMS", root);
		ParamItem::create("INPUT", layers, true);
		ParamItem::create("SPOOL", layers, true);
		ParamItem::create("SDRLEN", params, false);
		ParamItem::create("ACTDEN", params, false);
		selItem = 0;
	}

	void selUp(WINDOW *ctrlwin) {
		selItem = (selItem == 0) ? menuStack.back()->children.size()-1 : selItem-1;
		menuStack.back()->selChild = selItem;
	}

	void selDown(WINDOW *ctrlwin) {
		selItem = (selItem == menuStack.back()->children.size()-1) ? selItem = 0 : selItem+1;
		menuStack.back()->selChild = selItem;
	}

	void selLeft(WINDOW *ctrlwin) {
		if(menuStack.size()>1) {
			menuStack.pop_back();
			selItem = menuStack.back()->selChild;
		}
	}

	void selRight(WINDOW *ctrlwin) {
		if(!menuStack.back()->isLeaf) {
			auto mi = std::static_pointer_cast<MenuItem>(menuStack.back()->children.at(selItem));
			menuStack.push_back(mi);
			selItem = 0;
		}	
	}

	void print(WINDOW *ctrlwin) 
	{
		// TODO
		int x,y,i;
		i = 0;
		x = 3; 
		y = 1;
		box(ctrlwin,0,0); 
		// Menu > Submenu > ... > Submenu : (Parameter)
		for(auto& menuItem : menuStack) {
			if(i==0) { 
				continue;	// Ignore Root
			}
			mvwprintw(ctrlwin,y,x,"%s",menuItem->name.c_str());	
			x += 7;
			mvwprintw(ctrlwin,y,x,"%s",SEPMEN.c_str());	
			x += 3;
			i++;
		}
		if(menuStack.back()->isLeaf) {
			const auto& pi = std::static_pointer_cast<ParamItem>(menuStack.back()->children.at(selItem));
			wattron(ctrlwin,COLOR_PAIR(1));		 		// Highlight Param
			mvwprintw(ctrlwin,y,x,"%s",pi->name.c_str());
			x += 7;
			wattroff(ctrlwin,COLOR_PAIR(1));
			if(pi->isSelection) {
				mvwprintw(ctrlwin,y,x,"%s",SEPSEL.c_str());	// Add selection separator
			} else {
				mvwprintw(ctrlwin,y,x,"%s",SEPPRM.c_str());	// Add parameter separator
				x += 3;
				mvwprintw(ctrlwin,y,x,"%s",std::to_string(pi->value).c_str());	
			}
		}
		wrefresh(ctrlwin);
	}

protected:

	std::vector<std::shared_ptr<MenuItem>> menuStack; 
	int selItem;
};

struct StatusBar {

	StatusBar() {
		modes.push_back("INSERT");
		modes.push_back("SELECT");
		modeidx = 0;
	}

	void print(WINDOW *statwin, const std::string& msg)  {
		int x,y; 
		x = 3;
		y = 1;	
		box(statwin,0,0); 
		mvwprintw(statwin,y,x,modes.at(modeidx).c_str());
		x += 7;
		mvwprintw(statwin,y,x,msg.c_str());
		wrefresh(statwin);
	}	
	std::vector<std::string> modes;
	int modeidx;
				
};

struct ContentPane {

	ContentPane(size_t argncols, size_t argSdrSize) 
	: ncols(argncols), sdrSize(argSdrSize){}

	void print(WINDOW *contwin, const xt::xarray<bool> & sdr, const int avrows, const int avcols){
		int i; 
		int xi,yi;
		int xoff,yoff;
		int maxrows, maxcols;

		// Offset depending on HTM topology
		// We need NCOLS << 1 for whitespaces
		maxcols = avcols < ncols << 1 ? avcols : ncols << 1;   	
		maxrows = (sdrSize << 1) / maxcols; 

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

protected:	
	size_t ncols;
	size_t sdrSize;

	

};

}
