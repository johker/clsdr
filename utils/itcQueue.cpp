#include "itcQueue.hpp"

namespace dh {

template <class Q>
ItcQueue<Q>::ItcQueue() {
		done = false;
}
template <class Q>
ItcQueue<Q>::~ItcQueue() {
	done = true;
	condition.notify_all();
}
template <class Q>
bool ItcQueue<Q>::pushMessage(Q argMessage) {
	if(done) {
		return false;
	}
	std::lock_guard<std::mutex> guard(queueMutex);
	queue.push(argMessage);
	condition.notify_one();
	return true;
}
template <class Q>
bool ItcQueue<Q>::wait(Q* argResult) {
	std::unique_lock<decltype(queueMutex)> lock(queueMutex);
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
template <class Q>
bool ItcQueue<Q>::get(Q* argResult) {
	std::unique_lock<decltype(queueMutex)> lock(queueMutex);
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
