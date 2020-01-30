#include "task.hpp"

Task::~Task() {
	stopTask();
}
explicit Task::Task(std::string argTaskName){
		
}
bool startTask() {
	done = false; 
	std::lock_guard<std::mutex> guard(messageQueueMutex);
	workerThread = new std::thread(Task::worker, this);
	std::this_thread::sleep_for(std::chrono::milliseconds(5));
	return true;
}
bool stopTask() {
	if(!workerThread) {
		return true;
	}
	done = true;
	messageQueuedCondition.notify_one();
	workerThread->join();
	workerThread = nullptr;
	std::unique_lock<std::mutex> lock(messageQueueMutex);
	while(messageQueue.size()) 
	{
		messageQueue.pop(); 
	}
	return true;
}
bool Task::pushMessage(std::shared_ptr<T> argMessage) {
	//TODO Prevent message overflow / diagnostics
	messageQueue.push(argMessage);
	messageQueuedCondition.notify_one();
	return true;
}
static int worker(Task* argTask) {
	do {
		{
			std::unique_lock<std::mutex> lock(argTask->messageQueueMutex);
			if(argTask->messageQueue.size() == 0) {
				messageQueuedCondition.wait(lock);
			}
		}
		std::shared_ptr<T> message = nullptr;
		{
			std::unique_lock<std::mutex> lock(argTask->messageQueueMutex);
			if(argTask->messageQueue.size() > 0) {
				message = argTask->messageQueue.front();
				argTask->messageQueue.pop();

			}
		}

		if(message != nullptr) {
			argTask->handleMessage(message);
		}

	} while (!argTask->done);	

	{
		std::unique_lock<std::mutex> lock(argTask->messageQueueMutex); 
		argTask->messageQueue = std::queue<std::shared_ptr<T>>();
	}
	return 0;
}
