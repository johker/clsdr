#pragma once

#include "../../tiny_htm/tiny_htm/tiny_htm.hpp"    
#include "stringConstants.hpp"

namespace dh {

constexpr unsigned int hash(const char *s, int off = 0) {                                                                                                                                                           
	        return !s[off] ? 5381 : (hash(s, off+1)*33) ^ s[off];                           
}

class HtmProcessor : public Task<ItcMessage> {

public:
	HtmProcessor();
	virtual ~HtmProcessor();
}

}
