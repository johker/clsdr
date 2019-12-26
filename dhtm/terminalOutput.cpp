#include "terminalOutput.hpp"

namespace dh {

TerminalOutput::TerminalOutput(std::shared_ptr<HtmController> argHtmCtrl) : htmCtrl(argHtmCtrl) {
	addMenu();
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
void TerminalOutput::selUp(WINDOW *ctrlwin) {
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
	printControlBar(ctrlwin);
}
void TerminalOutput::selDown(WINDOW *ctrlwin) {
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
	printControlBar(ctrlwin);
}
void TerminalOutput::selLeft(WINDOW *ctrlwin) {
	if(menuStack.size()>1) {
		menuStack.pop_back();
		selItem = menuStack.back()->selChild;
	} else {
		collapsed = true;
	}
	keyInput = "";		// Clear keyboard input
	htmCtrl->setMode(SELECT);
	printControlBar(ctrlwin);
}
void TerminalOutput::selRight(WINDOW *ctrlwin) {
	if(collapsed) {
		collapsed = false;
	}
	else if(!menuStack.back()->isLeaf) {
		auto mi = std::static_pointer_cast<MenuItem>(menuStack.back()->children.at(selItem));
		menuStack.push_back(mi);
		selItem = 0;
	}	
	printControlBar(ctrlwin);
}
void TerminalOutput::numEntry(WINDOW *ctrlwin, int key) {
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
	printControlBar(ctrlwin);
}
void TerminalOutput::enter(WINDOW *ctrlwin) {
	if(collapsed || !menuStack.back()->isLeaf) { 
		selRight(ctrlwin);
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
	printControlBar(ctrlwin);
}
void TerminalOutput::collapse(WINDOW *ctrlwin) {
	menuStack.resize(1);
	collapsed = true;
	printControlBar(ctrlwin);
}
void TerminalOutput::printControlBar(WINDOW *ctrlwin) 
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
void TerminalOutput::printStatusBar(WINDOW *statwin)  {
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
void TerminalOutput::printContentPane(WINDOW *contwin, const xt::xarray<bool> & sdr){
	int i; 
	int xi,yi;
	int xoff,yoff;
	int maxrows, maxcols;

	wclear(contwin);
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
int TerminalOutput::updateScreen(WINDOW **win) {
	
	getmaxyx(win[0],htmCtrl->avrows,htmCtrl->avcols);	// Get screen dimensions
	if(htmCtrl->avrows != htmCtrl->avrowstmo || htmCtrl->avcols != htmCtrl->avcolstmo) {
		wresize(win[1],3,htmCtrl->avcols-2);
		wresize(win[2],3,htmCtrl->avcols-2);
		wresize(win[3],htmCtrl->avrows-6,htmCtrl->avcols-2);
		mvwin(win[2],htmCtrl->avrows-3,1); 
		wclear(win[0]);
		for(int wi = 1; wi < 4; wi++) {
			wclear(win[wi]);
			box(win[wi],0,0);
			wrefresh(win[wi]);
		}
		htmCtrl->avrowstmo = htmCtrl->avrows;
		htmCtrl->avcolstmo = htmCtrl->avcols;
			
		return 1;
	}
	return 0;
}

}
