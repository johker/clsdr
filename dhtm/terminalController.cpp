#include "terminalController.hpp"

namespace dh {

TerminalController::HtmController(){};
TerminalController::~HtmController(){};

const char* TerminalController::getValue(const char* key) {
	const char * value;
	// TODO: Send ItcMessage with callback
	return value;
}
void TerminalController::setValue(const char* key, float value) {
	// TODO: Set value via ItcMessage
	}
}
void Terminal::updateContent() {
	std::shared_ptr<ItcMessage> message = nullptr;
	{
		std::unique_lock<std::mutex> lock(messageQueueMutex); 
		size_t queueSize = messageQueue.size();
	}
	// There may be more messages (processed later) but never fewer than queueSize
	while(queueSize > 0) {
		{
			std::unique_lock<std::mutex> lock(messageQueueMutex);
			message = messageQueue.front(); 	
			messageQueue.pop();
		}
		if(message != nullptr) {
			if(message->type == READ) {
				auto paramItem = getParameterByKey(message->key);
				paramItem->setValue(message->value);
			} else if(message->type = PRINT) {
				sdr = message->sdr;
			}
		}
		queueSize--;
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
void TerminalController::setScalarEncoder(th::ScalarEncoder* argScalarEncoder) { 
	scalarEncoder = argScalarEncoder;
}
th::ScalarEncoder* TerminalController::getScalarEncoder() {
	return scalarEncoder;
}
void TerminalController::setCategoryEncoder(th::CategoryEncoder* argCategoryEncoder) {
	categoryEncoder = argCategoryEncoder;
}
void TerminalController::setEncoderType(EncoderType argEncoderType){
	encoderType = argEncoderType;
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
bool TerminalController::pushMessage(std::shared_ptr<ItcMessage> argMessage){
	//TODO: Prevent message overflow / diagnostics
	messageQueue.push(argMessage); 
	return true;
}

}
