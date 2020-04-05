#pragma once

#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>

#define DEBUG(text) dh::Logger::debug(text, __FILE__, __LINE__)
#define ERROR(text) dh::Logger::error(text, __FILE__, __LINE__)

namespace dh {


class Logger {

public:
	static void debug(const std::string& argText, const std::string& argFile, int argLine);
	static void error(const std::string& argText, const std::string& argFile, int argLine);

private:
	static std::string getTime();

	static std::mutex logMutex;

};

}


