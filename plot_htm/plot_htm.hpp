#pragma once

#include <string>
#include <cstring>
#include <chrono>
#include <thread>
#include <memory>
#include <algorithm>
#include <xtensor/xarray.hpp>
#include <ncursesw/ncurses.h>
#include "../../tiny_htm/tiny_htm/tiny_htm.hpp"
#include <locale.h>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#define KEY_LINE_FEED 0x0A
#define KEY_CARRIAGE_RETURN 0x0D
#define KEY_ZERO 0x2F
#define KEY_NINE 0x39
#define KEY_DOT 0x2E

namespace ph {

constexpr unsigned int hash(const char *s, int off = 0) {                        
	return !s[off] ? 5381 : (hash(s, off+1)*33) ^ s[off];                           
}        

constexpr char SYM_ACTIVE[]{"\u25A0"};		// Active Minicolumn 
constexpr char SYM_INACTIVE[]{"\u25A1"};	// Inactive minicolumn

constexpr char MODE_INSERT[]{"INSERT"};		// Insert data
constexpr char MODE_SELECT[]{"SELECT"};		// Select from menu
constexpr char MODE_EDIT[]{"EDIT"};		// Edit parameter

constexpr char SEP_MEN[]{"\u25B6"};		// Menu Separator
constexpr char SEP_SEL[]{"\u25BC"};		// Selection Separator
constexpr char SEP_PRM[]{":"};			// Parameter Separator

constexpr char KEY_MENU[]{"MENU"};		// Menu
constexpr char KEY_VIEW[]{"VIEW"};		// View
constexpr char KEY_LAYER[]{"LAYER"};		// Layer
constexpr char KEY_PARAMS[]{"PARMS"};		// Params
constexpr char KEY_INPUT[]{"INPUT"};		// Input
constexpr char KEY_SPOOL[]{"SPOOL"};		// Spatial Pooler
constexpr char KEY_SDRLEN[]{"SDRLEN"};		// SDR Length
constexpr char KEY_ACTDEN[]{"ACTDEN"};		// Active Density

class HtmController {
public: 
	HtmController(th::CategoryEncoder& argCategoryEncoder) : categoryEncoder(argCategoryEncoder){};
	// Screen params
	int avrows;
	int avcols; 
	int avrowstmo; 
	int avcolstmo; 

	// Active Model
	// TODO Dynamically switch between different encoders while keeping same parameter keys

	// Models
	th::CategoryEncoder& categoryEncoder; 

	size_t sdrSize = 8; 
	size_t ncols = 8;	
	int layer=0; 

	const char* getValue(const char* key) {
		const char * value;
		switch(hash(key)) {
			case hash(KEY_SDRLEN):
				value = std::to_string(categoryEncoder.sdrLength()).c_str();
			break;
			case hash(KEY_ACTDEN):
				value = std::to_string(categoryEncoder.encodeLength()).c_str();
			break;
		}		
		return value;
	}
	void setValue(const char* key, double value) {
		// TODO: Set value
	}
	void setStatusTxt(std::string argStatusTxt){statusTxt = argStatusTxt;}
	void setModeIdx(size_t argModeIdx){modeIdx = argModeIdx;}
	std::string getStatusTxt() {return statusTxt;}
	size_t getModeIdx() {return modeIdx;}
	std::string getModeTxt() {return modes.at(modeIdx);}
private: 
	std::string statusTxt;
	std::vector<std::string> modes{MODE_INSERT,MODE_SELECT,MODE_EDIT};
	size_t modeIdx = 0;
};

class Item {
public:
	const char* name; 
	int type;
protected:
	Item(const char* name, int type) : name(name), type(type) {}
};

class MenuItem : public Item {
public:	
	static std::shared_ptr<MenuItem> create(const char* argName, std::shared_ptr<MenuItem> parent) {
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
	MenuItem(const char* name): Item(name, 0) {}
};

class ParamItem : public Item {
public:
	static std::shared_ptr<ParamItem> create(const char* argName, std::shared_ptr<MenuItem> parent, std::shared_ptr<HtmController> argHtmCtrl) {
		auto pi = std::shared_ptr<ParamItem>(new ParamItem(argName));
		if(parent != nullptr) {
			parent->children.push_back(pi);
			parent->isLeaf = true;
			pi->htmCtrl = argHtmCtrl;
		}
		return pi;
	}
	const char* getValue() {
		return htmCtrl->getValue(name);
	}
private:
	ParamItem(const char* name): Item(name, 1) {}
	std::shared_ptr<HtmController> htmCtrl;
};


class SelectItem : public Item {
public:
	static std::shared_ptr<SelectItem> create(const char* argName, std::shared_ptr<MenuItem> parent, std::shared_ptr<HtmController> argHtmCtrl,std::vector<std::string> argOptions) {
		auto pi = std::shared_ptr<SelectItem>(new SelectItem(argName));
		if(parent != nullptr) {
			parent->children.push_back(pi);
			parent->isLeaf = true;
			pi->htmCtrl = argHtmCtrl;
			pi->options = argOptions;
			// TODO: load current index from controller
		}
		return pi;
	}
	void selectValue(){
		selIdx = curIdx;
		htmCtrl->setValue(name,selIdx);
	}
	const char* getValue() {
		return options.at(curIdx).c_str();
	}
	void nextValue() {
		curIdx = (curIdx+1<options.size())? curIdx+1 : 0;
	}
	void prevValue() {
		curIdx = (curIdx-1>=0) ? curIdx-1 : options.size()-1;
	}
private:
	SelectItem(const char* name): Item(name,2) {}
	std::shared_ptr<HtmController> htmCtrl;
	std::vector<std::string> options;
	size_t selIdx = 0;
	size_t curIdx = 0;
};

class ControlBar {
public:	
	ControlBar(std::shared_ptr<HtmController> argHtmCtrl) : htmCtrl(argHtmCtrl) {
		addMenu();
	}

	void addMenu() {
		auto root = MenuItem::create(KEY_MENU, std::shared_ptr<MenuItem>());
		menuStack.push_back(root);
	       	auto view = MenuItem::create(KEY_VIEW, root);
		auto params = MenuItem::create(KEY_PARAMS, root);
		ParamItem::create(KEY_SDRLEN, params, htmCtrl);
		ParamItem::create(KEY_ACTDEN, params, htmCtrl);
		std::vector<std::string> options = {KEY_INPUT, KEY_SPOOL};
		SelectItem::create(KEY_LAYER, view, htmCtrl, options);
		selItem = 0;
	}

	void selUp(WINDOW *ctrlwin) {
		auto& mi = menuStack.back()->children.at(selItem);
		if(htmCtrl->getModeIdx()==2 && mi->type==2) {
			const auto& si = std::static_pointer_cast<SelectItem>(mi);
			si->prevValue();
		} else {
			keyInput = "";		// Clear keyboard input
			htmCtrl->setModeIdx(1);
			selItem = (selItem == 0) ? menuStack.back()->children.size()-1 : selItem-1;
			menuStack.back()->selChild = selItem;
		}
		print(ctrlwin);
	}

	void selDown(WINDOW *ctrlwin) {
		auto& mi = menuStack.back()->children.at(selItem);
		if(htmCtrl->getModeIdx()==2 && mi->type==2) {
			const auto& si = std::static_pointer_cast<SelectItem>(mi);
			si->nextValue();
		} else {
			keyInput = "";		// Clear keyboard input
			htmCtrl->setModeIdx(1);	
			selItem = (selItem == menuStack.back()->children.size()-1) ? 0 : selItem+1;
			menuStack.back()->selChild = selItem;
		}
		print(ctrlwin);
	}

	void selLeft(WINDOW *ctrlwin) {
		if(menuStack.size()>1) {
			menuStack.pop_back();
			selItem = menuStack.back()->selChild;
		} else {
			collapsed = true;
		}
		keyInput = "";		// Clear keyboard input
		htmCtrl->setModeIdx(1);
		print(ctrlwin);
	}

	void selRight(WINDOW *ctrlwin) {
		if(collapsed) {
			collapsed = false;
		}
		else if(!menuStack.back()->isLeaf) {
			auto mi = std::static_pointer_cast<MenuItem>(menuStack.back()->children.at(selItem));
			menuStack.push_back(mi);
			selItem = 0;
		}	
		print(ctrlwin);
	}

	void numEntry(WINDOW *ctrlwin, int key) {
		if(key == KEY_DOT && keyInput.find('.') != std::string::npos) {
			return; 		// Dont allow multiple dots		
		} 
		if(key == KEY_BACKSPACE) {
			// TODO Fix this branch
			keyInput.pop_back();
		} else {
			keyInput.append(std::string(1, char(key)));
		}	
		print(ctrlwin);
	}

	void enter(WINDOW *ctrlwin) {
		if(collapsed || !menuStack.back()->isLeaf) { 
			selRight(ctrlwin);
		} else {
			const auto& pi = std::static_pointer_cast<ParamItem>(menuStack.back()->children.at(selItem));
			if(htmCtrl->getModeIdx()==2) {
				// TODO Check with regex if entry is correct
				// TODO Confirm new value by calling set 
				keyInput = "";			// Clear keyboard input
				htmCtrl->setModeIdx(1);		// Swtich to select mode
			} else {
				htmCtrl->setModeIdx(2);
			}
		}
		print(ctrlwin);
	}

	void collapse(WINDOW *ctrlwin) {
		menuStack.resize(1);
		collapsed = true;
		print(ctrlwin);
	}

	void print(WINDOW *ctrlwin) 
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
				if(htmCtrl->getModeIdx()==2) {
					wattron(ctrlwin, A_BLINK);
				}
				if(htmCtrl->getModeIdx()==2 && keyInput.size() > 0) {
					mvwprintw(ctrlwin,y,x,"%s",keyInput.c_str());
				}else {	
					mvwprintw(ctrlwin,y,x,"%s",pi->getValue());	
				}
				wattroff(ctrlwin, A_BLINK);
			} else if(mi->type == 2) {	// Selection 
			       const auto& si = std::static_pointer_cast<SelectItem>(mi);
				mvwprintw(ctrlwin,y,x,"%s",SEP_PRM);
				x += 3;
				if(htmCtrl->getModeIdx()==2)
					wattron(ctrlwin,A_BLINK);
				mvwprintw(ctrlwin,y,x,"%s",si->getValue());
				wattroff(ctrlwin, A_BLINK); 
			} else {
				mvwprintw(ctrlwin,y,x,"%s",SEP_MEN);		// Add menu separator
			}
		}
		wrefresh(ctrlwin);
	}

private:	
	std::shared_ptr<HtmController> htmCtrl;
	std::vector<std::shared_ptr<MenuItem>> menuStack; 
	int selItem;
	bool collapsed = true;
	std::string keyInput; 
};

class StatusBar {
public:
	StatusBar(std::shared_ptr<HtmController> argHtmCtrl) : htmCtrl(argHtmCtrl) {}

	void print(WINDOW *statwin)  {
		wclear(statwin);
		int x,y; 
		x = 3;
		y = 1;	
		box(statwin,0,0); 
		mvwprintw(statwin,y,x,htmCtrl->getModeTxt().c_str());
		x += 7;
		mvwprintw(statwin,y,x,htmCtrl->getStatusTxt().c_str());
		wrefresh(statwin);
	}	
private:	
	std::shared_ptr<HtmController> htmCtrl;
};

class ContentPane {
public:
	ContentPane(std::shared_ptr<HtmController> argHtmCtrl) : htmCtrl(argHtmCtrl){}

	void print(WINDOW *contwin, const xt::xarray<bool> & sdr){
		int i; 
		int xi,yi;
		int xoff,yoff;
		int maxrows, maxcols;

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
				mvwprintw(contwin,yi,xi,SYM_ACTIVE);
				mvwaddch(contwin,yi,xi-1,' ');
			}
			else {
				mvwprintw(contwin,yi,xi,SYM_INACTIVE);
				mvwaddch(contwin,yi,xi-1,' ');
			}
		}	
		wrefresh(contwin);
	}

private:	
	std::shared_ptr<HtmController> htmCtrl;
};

}
