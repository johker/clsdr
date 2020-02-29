#pragma once

#include <string>
#include <cstring>
#include <chrono>
#include <memory>
#include <atomic>
#include <algorithm>
#include <mutex>
#include <thread>
#include <xtensor/xarray.hpp>
#include <ncursesw/ncurses.h>
#include <locale.h>

#include "htmController.hpp"
#include "stringConstants.hpp"

namespace dh {

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

class TerminalOutput {
public: 
	explicit TerminalOutput(std::shared_ptr<HtmController> argHtmController); 
	virtual ~TerminalOutput();

	void selUp(); 
	void selDown(); 
	void selLeft(); 
	void selRight(); 
	void numEntry(int key); 
	void enter();
	void collapse();
	void printControlBar();
	void printStatusBar();
	void printContentPane(const xt::xarray<bool>& sdr);
	int updateScreen();
	bool startTerminal();
	bool stopTerminal();
	bool done = false; 

private:
	static int worker(TerminalOutput* argTerminal);
	void addMenu();
	void addParamsMenu(std::shared_ptr<MenuItem> params); 
	
	int avrows,avcols;					// Available rows / cols
	int avrowstmo = 0, avcolstmo = 0;			// Available rows / cols in t minus one
	WINDOW *stdwin;
	WINDOW *ctrlwin;
	WINDOW *statuswin;
	WINDOW *contentwin;
	int cmdidx = 0;						// command index
	std::thread* workerThread; 
	std::shared_ptr<HtmController> htmCtrl;
	std::vector<std::shared_ptr<MenuItem>> menuStack; 
	int selItem;
	bool collapsed = true;
	std::string keyInput; 
};

}
