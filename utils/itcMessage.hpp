
#pragma once

#include <xtensor/xarray.hpp>

namespace dh {

enum class ItcType {
	READ = 0,
	WRITE = 1,
	PRINT = 2
};

class ItcMessage {

public:
	ItcMessage() {};
	ItcMessage(ItcType argType, const char* argKey, float argValue) : type(argType), key(argKey), value(argValue) {}
	ItcMessage(ItcType argType, const char* argKey, xt::xarray<bool> argSdr) : type(argType), key(argKey), sdr(argSdr) {}
	virtual ~ItcMessage() {}

	ItcType type;
	const char* key;
	float value; 
	xt::xarray<bool> sdr;
};

}
