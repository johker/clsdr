#include "terminalController.hpp"

namespace dh {

TerminalController::TerminalController(std::shared_ptr<ItcQueue<ItcMessage>> argInQ, std::shared_ptr<ItcQueue<ItcMessage>> argOutQ): inQ(argInQ), outQ(argOutQ) {}
TerminalController::~TerminalController(){};

void TerminalController::updateContent() {
	ItcMessage* message = nullptr;
	while(inQ->get(message)) {
		if(message->type == ItcType::READ) {
			std::shared_ptr<ParamItem> paramItem = getParameterByKey(message->key);
			paramItem->setValue(message->value);
		} else if(message->type == ItcType::PRINT) {
			sdr = message->sdr;
		}
	}
}
std::shared_ptr<ParamItem> TerminalController::getParameterByKey(const char* argKey)
{
	std::vector<std::shared_ptr<ParamItem>>::iterator it = 
		   std::find_if(parameters.begin(), parameters.end(),
			[argKey](std::shared_ptr<ParamItem> const n) {
			return strcmp(n->getValue(),argKey);});
	if (it != parameters.end()) {
		return *it;
	}
	return nullptr;
}
void TerminalController::setValue(const char* argKey, float argValue) {
	ItcMessage itcMessage(ItcType::WRITE, argKey, argValue);
	outQ->pushMessage(itcMessage);
}
void TerminalController::setStatusTxt(std::string argStatusTxt){
	statusTxt = argStatusTxt;
}
void TerminalController::setMode(Mode argMode){
	mode = argMode;
}
std::string TerminalController::getStatusTxt() {
	return statusTxt;
}
Mode TerminalController::getMode() {
	return mode;
}
std::string TerminalController::getModeTxt() {
	return modeLabels.at((int) mode);
}
EncoderType TerminalController::getEncoderType() {
	return encoderType;
}
}
