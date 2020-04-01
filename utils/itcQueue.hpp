#pragma once
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>

namespace dh {

template <class Q>
class ItcQueue {
public:
	ItcQueue() {
		done = false;
	}
	virtual ~ItcQueue() {
		done = true;
		condition.notify_all();
	}
	bool pushMessage(Q argMessage) {
		if(done) {
			return false;
		}
		std::lock_guard<std::mutex> guard(queueMutex);
		queue.push(argMessage);
		condition.notify_one();
		return true;
	}
	bool wait(Q* argResult) {
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
	bool get(Q* argResult) {
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
private:
	mutable std::mutex queueMutex;
	std::condition_variable condition;
	std::queue<Q> queue;
	std::atomic<bool> done;
};
}
