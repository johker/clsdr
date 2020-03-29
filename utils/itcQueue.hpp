#pragma once
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>

namespace dh {

template <class Q>
class ItcQueue {
public:
	ItcQueue();
	virtual	~ItcQueue();
	bool pushMessage(Q argMessage);
	bool wait(Q* argResult);
	bool get(Q* argResult);
	int size();	
private:
	mutable std::mutex queueMutex;
	std::condition_variable condition;
	std::queue<Q> queue;
	std::atomic<bool> done;
};
}
