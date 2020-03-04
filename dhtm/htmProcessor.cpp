#include "htmProcessor.hpp"

namespace dh {

HtmProcessor::HtmProcessor(){};
HtmProcessor::~HtmProcessor(){};

const char* HtmProcessor::getValue(const char* key) {
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
void HtmProcessor::setValue(const char* key, float value) {
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
}
