
#pragma once

#include "../../tiny_htm/tiny_htm/tiny_htm.hpp"    
#include "stringConstants.hpp"

namespace dh {

constexpr char MODE_INSERT[]{"INSERT"};		// Insert data
constexpr char MODE_SELECT[]{"SELECT"};		// Select from menu
constexpr char MODE_EDIT[]{"EDIT"};		// Edit parameter

constexpr unsigned int hash(const char *s, int off = 0) {                                                                                                                                                           
	        return !s[off] ? 5381 : (hash(s, off+1)*33) ^ s[off];                           
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

class HtmController {
public: 
	HtmController();
	virtual ~HtmController();
	
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
	void setScalarEncoder(th::ScalarEncoder* argScalarEncoder);
	void setCategoryEncoder(th::CategoryEncoder* argCategoryEncoder);
	void setEncoderType(EncoderType argEncoderType);
	std::string getStatusTxt();
	Mode getMode();
	std::string getModeTxt();
	EncoderType getEncoderType();

private: 
	// Active Model
	// TODO Dynamically switch between different encoders 
	// Parameter Selection Menu should change

	// Models
	// TODO Make Vector to allow multiple
	th::CategoryEncoder* categoryEncoder; 
	th::ScalarEncoder* scalarEncoder;
	std::string statusTxt;
	std::vector<std::string> modeLabels{MODE_INSERT,MODE_SELECT,MODE_EDIT};
	Mode mode = INSERT;
	EncoderType encoderType = SCALAR;
};

}