#include "itcQueue.hpp"

namespace dh {

ItcQueue::ItcQueue() {
		done = false;
}
ItcQueue::~ItcQueue() {
	done = true;
	condition.notify_all();
}
bool ItcQueue::pushMessage(Q argMessage) {
	if(done) {
		return false;
	}
	std::lock_guard<std::mutex> guard(queueMutex);
	queue.push(argMessage);
	condition.notify_one();
	return true;
}
bool wait(Q* argResult) {
	std::unique_lock<decltype(mutex)> lock(queueMutex);
	Q message;
	while(!done) {
		if(queue.size()) {
			message = queue.front(); 
			queue.pop();
			*argResult = message; 
			return true;
		}
		condition.wait(lock);
	}
	return false;
}
bool get(Q* argResult) {
	std::unique_lock<decltype(mutex)> lock(queueMutex);
	Q message;
	if(queue.size()) {
		message = queue.front();
		queue.pop();
		*argResult = message;
		return true;
	}
	return false;
}
}
