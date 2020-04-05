#include <iostream>
#include <string>
#include <xtensor/xarray.hpp>
#include <chrono>
#include <thread>
#include <panel.h>
#include <ncursesw/ncurses.h>
#include <locale.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "../tiny_htm/tiny_htm/tiny_htm.hpp"

#include "dhtm/stringConstants.hpp"
#include "dhtm/terminal.hpp"
#include "dhtm/terminalController.hpp"
#include "utils/itcMessage.hpp"
#include "utils/itcQueue.hpp"
#include "utils/logger.hpp"


// VIM like HTM interface:
// INSERT mode: Keyboard entries are directly encoded 
// NORMAL mode: Commands to select view, exit program etc.
// Input should be decoupled from HTM output so a different sensor can be used easily


int main(){
	
	// HTM 
	size_t numcat = 2;
	size_t encLen = 256;
	size_t actBits = 2;
	size_t phorizon = 10; 
	size_t min = 0; 
	size_t max = 100; 

	th::ScalarEncoder scalarEncoder(min, max, encLen, actBits);
	th::CategoryEncoder categoryEncoder(numcat, encLen);
	th::TemporalMemory tm({numcat*4}, 6);
	
	std::shared_ptr<dh::ItcQueue<dh::ItcMessage>> term2htmq = std::make_shared<dh::ItcQueue<dh::ItcMessage>>(); 
	std::shared_ptr<dh::ItcQueue<dh::ItcMessage>> htm2termq = std::make_shared<dh::ItcQueue<dh::ItcMessage>>(); 
	
	// Initialize controller
	std::shared_ptr<dh::TerminalController> termCtrl = std::make_shared<dh::TerminalController>(htm2termq, term2htmq);
	
	// Start Terminal Thread
	dh::Terminal terminal(termCtrl);
	terminal.startTerminal();

	DEBUG("Terminal started");

	return 0;
}

