
#pragma once

#include "../../tiny_htm/tiny_htm/tiny_htm.hpp"    
#include "stringConstants.hpp"
#include "../utils/itcMessage.hpp"

namespace dh {

constexpr char MODE_INSERT[]{"INSERT"};		// Insert data
constexpr char MODE_SELECT[]{"SELECT"};		// Select from menu
constexpr char MODE_EDIT[]{"EDIT"};		// Edit parameter

}

enum Mode {
	INSERT = 0,
	SELECT = 1,
        EDIT = 2
};
                                                                                                                                                                                                                    
enum EncoderType {
	SCALAR,
	CATEGORY
};

class TerminalController {
public: 
	TerminalController(std::shared_ptr<ItcQueue> argInQ, std::shared_ptr<ItcQueue> argOutQ);
	virtual ~TerminalController();
	
	// Screen params
	int avrows;
	int avcols; 
	int avrowstmo; 
	int avcolstmo; 

	size_t sdrSize = 8; 
	size_t ncols = 32;	
	int layer=0; 

	const char* getValue(const char* key);
	void setValue(const char* key, float value);
	void setStatusTxt(std::string argStatusTxt);
	void setMode(Mode argMode);
	std::string getStatusTxt();
	Mode getMode();
	std::string getModeTxt();
	EncoderType getEncoderType();
	bool pushMessage(std::shared_ptr<ItcMessage> argMessage);

private: 
	std::shared_ptr<ItcQueue<ItcMessage>> inQ;
	std::shared_ptr<ItcQueue<ItcMessage>> outQ;
	
	std::vector<std::shared_ptr<ParamItem>> parameters;
	xt:xarray<bool> sdr;

	// Models
	std::string statusTxt;
	std::vector<std::string> modeLabels{MODE_INSERT,MODE_SELECT,MODE_EDIT};
	Mode mode = INSERT;
	EncoderType encoderType = SCALAR;
};

}
