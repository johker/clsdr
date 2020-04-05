#include "logger.hpp"

namespace dh {

std::mutex Logger::logMutex;

void Logger::debug(const std::string& argText, const std::string& argFile, int argLine)
{
	std::lock_guard<std::mutex> lock(logMutex);
	std::ofstream debugFile("dhtm.dbg", std::ostream::app);
	debugFile << getTime() << argText << " | " << argFile << " (" << argLine << ")" << std::endl;
	debugFile.close();
}

void Logger::error(const std::string& argText, const std::string& argFile, int argLine) {
	std::lock_guard<std::mutex> lock(logMutex);
	std::ofstream errorFile("dhtm.err", std::ostream::app);
	errorFile << getTime() << argText << " | " << argFile << " (" << argLine << ")" << std::endl;
	errorFile.close();
}

std::string Logger::getTime() {
	std::stringstream ss;
	time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	struct tm* pointInTimeBrokenDown = std::localtime(&now);
	ss << pointInTimeBrokenDown->tm_year + 1900 << "-"
	   << (pointInTimeBrokenDown->tm_mon + 1) << "-"
	   << pointInTimeBrokenDown->tm_mday << " "
	   << pointInTimeBrokenDown->tm_hour << ":"
	   << pointInTimeBrokenDown->tm_min << ":"
	   << pointInTimeBrokenDown->tm_sec << " ";
	return ss.str();
}

}
