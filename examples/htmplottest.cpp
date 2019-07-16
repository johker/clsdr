#include "../../tiny_htm/tiny_htm/tiny_htm.hpp"
#include <iostream>
#include <xtensor/xarray.hpp>
#include <chrono>
#include <thread>

void countdown(tm *ltm); 

int main(){
	int i; 
	th::ScalarEncoder scalarEncoder(0,100,128,12);

	for(i=0; i < 10; i++) {
		time_t now = time(0);
   		tm *ltm = localtime(&now);
		countdown(ltm);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		// std::cout << xt::cast<int>(scalarEncoder.encode(i)) + "\r";
	}
}

void countdown(tm *ltm)
{
	int h = 13 - ltm->tm_hour;
	int m = 59 - ltm->tm_min;
	int s = 59 - ltm->tm_sec;

	std::stringstream ss;
	ss << h << ":" << m << ":" << s;
	std::string string = ss.str();
	std::cout << '\r'
		  << std::setw(2) << std::setfill('0') << h << ':'
		  << std::setw(2) << m << ':'
		  << std::setw(2) << s << std::flush;
}
