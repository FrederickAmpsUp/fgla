#pragma once

#include <fgla/internal.hpp>
#include <cstdint>
#include <string>

namespace fgla {

/**
* A device, which creates objects for rendering
*/
class Device {
public:
	/**
	* A structure describing requirements and preferences for device selection
	*/
	struct Desc {
		// there's actually nothing here ... yet
	};
private:
	struct NativeData;
	NativeData *data;
	friend class NativeDataAccessor;
}; // class Device

} // namespace fgla
