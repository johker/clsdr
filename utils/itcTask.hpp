#pragma once

#include <string>
#include <thread>

namespace dh {

template <class T>	
class Task {

public: 
	virtual ~Task();
	bool pushMessage(std::shared_ptr<T> argMessage);

protected: 
	explicit Task(std::string argName);
	bool startTask(); 
	bool stopTask(); 
	virtual bool handleMessage(std::shared_ptr<T> argMessage) = 0;

private: 
	std::atomic<bool> done; 
	std::unique_ptr<std::thread> workerThread; 
	mutable std::mutex messageQueueMutex;
	std::queue<std::shared_ptr<T>> messageQueue;
	std::condition_variable messageQueuedCondition;
	static int worker(Task* argTask);
}


