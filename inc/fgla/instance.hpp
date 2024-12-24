#pragma once

namespace fgla {

class Instance {
public:
	Instance();

	bool is_ok();

	~Instance();
	
	struct NativeData;
	NativeData *data;
};
}
