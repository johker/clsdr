#pragma once

#include <string>
#include <cstring>
#include <chrono>
#include <memory>
#include <atomic>
#include <algorithm>
#include <mutex>
#include <thread>
#include <queue>
#include <xtensor/xarray.hpp>
#include <ncursesw/ncurses.h>
#include <locale.h>

#include "terminalController.hpp"
#include "stringConstants.hpp"
#include "../utils/itcMessage.hpp"

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
	static std::shared_ptr<ParamItem> create(const char* argName, std::shared_ptr<MenuItem> parent) {
		auto pi = std::shared_ptr<ParamItem>(new ParamItem(argName));
		if(parent != nullptr) {
			parent->children.push_back(pi);
			parent->isLeaf = true;
		}
		return pi;
	}
	void setValue(float argValue) {
		value = argValue;
	}
	const char* getValue() {
		return std::to_string(value).c_str();
	}
private:
	ParamItem(const char* name): Item(name, 1) {}
	float value = 0f;
};


class SelectItem : public Item {
public:
	static std::shared_ptr<SelectItem> create(const char* argName, std::shared_ptr<MenuItem> parent, std::shared_ptr<TerminalController> argHtmCtrl,std::vector<std::string> argOptions) {
		auto pi = std::shared_ptr<SelectItem>(new SelectItem(argName));
		if(parent != nullptr) {
			parent->children.push_back(pi);
			parent->isLeaf = true;
			pi->termCtrl = argHtmCtrl;
			pi->options = argOptions;
			// TODO: load current index from controller
		}
		return pi;
	}
	void selectValue(){
		selIdx = curIdx;
		termCtrl->setValue(name,selIdx);
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
	std::shared_ptr<TerminalController> termCtrl;
	std::vector<std::string> options;
	size_t selIdx = 0;
	size_t curIdx = 0;
};

class Terminal {
public: 
	explicit Terminal(std::shared_ptr<TerminalController> argHtmController); 
	virtual ~Terminal();

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
	int updateScreenDimension();
	bool startTerminal();
	bool stopTerminal();
	void updateContent(std::shared_ptr<ItcMessage> argMessage);	

	bool done = false; 

private:
	static int worker(Terminal* argTerminal);
	std::shared_ptr<ItcMessage> getParameterByKey(char* argKey);
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
	std::shared_ptr<TerminalController> termCtrl;
	std::vector<std::shared_ptr<MenuItem>> menuStack; 
	int selItem;
	bool collapsed = true;
	std::string keyInput; 
};

}
