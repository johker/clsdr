#include "task.hpp"

Task::~Task() {
	stopTask();
}
Task::Task(std::string argTaskName){
		
}
bool startTask() {
	shouldQuit = false; 
	std::lock_guard<std::mutex> guard(messageQueueMutex);
	workerThread = new std::thread(Task::worker, this);
	std::this_thread::sleep_for(std::chrono::milliseconds(5));
	return true;
}
bool stopTask() {
	shouldQuit = true;
	messageQueuedCondition.notify_one();
	workerThread->join();
	workerThread = nullptr;
	std::unique_lock<std::mutex> lock(messageQueueMutex);
	while(messageQueue.size()) 
	{
		messageQueue.pop(); 
	}
}
bool Task::pushMessage(std::shared_ptr<T> argMessage) {
	//TODO Prevent message overflow / diagnostics
	messageQueue.push(argMessage);
	messageQueuedCondition.notify_one();
	return true;
}


