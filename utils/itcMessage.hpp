
#pragma once

#include <xtensor/xarray.hpp>

namespace dh {

enum ItcType {
	READ = 0,
	WRITE = 1,
	PRINT = 2
};

class ItcMessage {

public:
	ItcMessage();
	virtual ~ItcMessage();

	ItcType type;
	char* key;
	float value; 
	xt:xarray<bool> sdr;
}

}
