#pragma once

#include "../../tiny_htm/tiny_htm/tiny_htm.hpp"    
#include "stringConstants.hpp"
#include "../utils/itcMessage.hpp"
#include "../utils/itcQueue.hpp"
#include <xtensor/xarray.hpp>

namespace dh {

constexpr char MODE_INSERT[]{"INSERT"};		// Insert data
constexpr char MODE_SELECT[]{"SELECT"};		// Select from menu
constexpr char MODE_EDIT[]{"EDIT"};		// Edit parameter

enum class Mode {
	INSERT = 0,
	SELECT = 1,
        EDIT = 2
};
                                                                                                                                                                                                                    
enum class EncoderType {
	SCALAR,
	CATEGORY
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
		float value = 0;
};
class SelectItem : public Item {
public:
	static std::shared_ptr<SelectItem> create(const char* argName, std::shared_ptr<MenuItem> parent, std::vector<std::string> argOptions) {
		auto pi = std::shared_ptr<SelectItem>(new SelectItem(argName));
		if(parent != nullptr) {
			parent->children.push_back(pi);
			parent->isLeaf = true;
			pi->options = argOptions;
			// TODO: load current index from controller
		}
		return pi;
	}
	void selectValue(){
		selIdx = curIdx;
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
	std::vector<std::string> options;
	size_t selIdx = 0;
	size_t curIdx = 0;
};
class TerminalController {
public: 
	TerminalController(std::shared_ptr<ItcQueue<ItcMessage>> argInQ, std::shared_ptr<ItcQueue<ItcMessage>> argOutQ);
	virtual ~TerminalController();
	
	// Screen params
	int avrows;
	int avcols; 
	int avrowstmo; 
	int avcolstmo; 

	size_t sdrSize = 8; 
	size_t ncols = 32;	
	int layer=0; 

	void updateContent();
	std::shared_ptr<ParamItem> getParameterByKey(const char* argKey);
	bool pushMessage(std::shared_ptr<ItcMessage> argMessage);
	void setValue(const char* argKey, float argValue);
	void setStatusTxt(std::string argStatusTxt);
	void setMode(Mode argMode);
	std::string getStatusTxt();
	Mode getMode();
	std::string getModeTxt();
	EncoderType getEncoderType();

	std::vector<std::shared_ptr<ParamItem>> parameters;
	xt::xarray<bool> sdr;

private: 
	std::shared_ptr<ItcQueue<ItcMessage>> inQ;
	std::shared_ptr<ItcQueue<ItcMessage>> outQ;

	// Models
	std::string statusTxt;
	std::vector<std::string> modeLabels{MODE_INSERT,MODE_SELECT,MODE_EDIT};
	Mode mode = Mode::INSERT;
	EncoderType encoderType = EncoderType::SCALAR;
};

}
