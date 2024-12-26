#pragma once
#include <vector>
#include <string>
#include <fgla/instance.hpp>
#include <fgla/util.hpp>

namespace fgla {

class Device {
public:
	static const std::vector<Device> enumerate(Instance& instance);
	fgla::result initialize();

	const std::string& name() const;
	int score() const;
	unsigned long long available_memory() const;

	~Device();
	struct NativeData;
	NativeData *data;

	Device(const Device&); // copy constructors
	Device operator=(const Device&);
	Device();
private:
	Device(NativeData);
};
}
