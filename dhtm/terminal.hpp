#pragma once

#include <stdio.h>
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
	void printContentPane();
	int updateScreenDimension();
	bool startTerminal();
	bool stopTerminal();
	void updateContent(std::shared_ptr<ItcMessage> argMessage);	

	bool done = false; 

private:
	static int worker(Terminal* argTerminal);
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
