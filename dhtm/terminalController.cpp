#include "terminalController.hpp"

namespace dh {

TerminalController::TerminalController(std::shared_ptr<ItcQueue> argInQ, std::shared_ptr<ItcQueue> argOutQ): inQ(argInQ), outQ(argOutQ) {}
TerminalController::TerminalController(){};

void TerminalController::setValue(const char* key, float value) {
	std::shared_ptr<ItcMessage> msg std::make_shared<ItcMessage>(WRITE,key,value);
	outQ->pushMessage(msg);
I
void Terminal::updateContent() {
	std::shared_ptr<ItcMessage> message = nullptr;
	while(inQ->get(message)) {
		if(message->type == READ) {
			auto paramItem = getParameterByKey(message->key);
			paramItem->setValue(message->value);
		} else if(message->type = PRINT) {
			sdr = message->sdr;
		}
	}
}
std::shared_ptr<ItcMessage> TerminalController::getParameterByKey(char* argKey)
{
	std::vector<std::shared_ptr<ItcMessage>>::iterator it = 
		   std::find_if(parameters.begin(), parameters.end(),
			[argKey](std::shared_ptr<ItcMessage> const n) {
			return strcmp(n->key,argKey);});
	if (it != parameters.end()) {
		return *it;
	}
	return nullptr;
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
	return modeLabels.at(mode);
}
EncoderType TerminalController::getEncoderType() {
	return encoderType;
}
