#include "htmController.hpp"

namespace dh {

HtmController::HtmController(){};
HtmController::~HtmController(){};

const char* HtmController::getValue(const char* key) {
	const char * value;
	switch(hash(key)) {
		case hash(KEY_MINVAL):
			value = std::to_string(scalarEncoder->minimumValue()).c_str();
		break;
		case hash(KEY_MAXVAL):
			value = std::to_string(scalarEncoder->maximumValue()).c_str();
		break;
		case hash(KEY_SDRLEN):
			value = std::to_string(scalarEncoder->sdrLength()).c_str();
		break;
		case hash(KEY_ACTBTS):
			value = std::to_string(scalarEncoder->encodeLength()).c_str();
		break;
		case hash(KEY_NUMCOL):
			value = std::to_string(ncols).c_str();
		break;
	}		
	return value;
}
void HtmController::setValue(const char* key, float value) {
	// TODO: Set value
	switch(hash(key)) {
		case hash(KEY_MINVAL):
			scalarEncoder->setMinimumValue((size_t) value);
		break;
		case hash(KEY_MAXVAL):
			scalarEncoder->setMaximumValue((size_t) value);
		break;
		case hash(KEY_SDRLEN):
			scalarEncoder->setSDRLength((size_t) value);
		break;
		case hash(KEY_ACTBTS):
			scalarEncoder->setEncodeLength((size_t) value);
		break;
		case hash(KEY_NUMCOL):
			ncols = (size_t) value;
		break;
	}
}
void HtmController::setStatusTxt(std::string argStatusTxt){
	statusTxt = argStatusTxt;
}
void HtmController::setMode(Mode argMode){
	mode = argMode;
}
void HtmController::setScalarEncoder(th::ScalarEncoder* argScalarEncoder) { 
	scalarEncoder = argScalarEncoder;
}
void HtmController::setCategoryEncoder(th::CategoryEncoder* argCategoryEncoder) {
	categoryEncoder = argCategoryEncoder;
}
void HtmController::setEncoderType(EncoderType argEncoderType){
	encoderType = argEncoderType;
}
std::string HtmController::getStatusTxt() {
	return statusTxt;
}
Mode HtmController::getMode() {
	return mode;
}
std::string HtmController::getModeTxt() {
	return modeLabels.at(mode);
}
EncoderType HtmController::getEncoderType() {
	return encoderType;
}

}
