#pragma once
#include <string>
#include <fgla/instance.hpp>

namespace fgla {

class Window {
public:
	Window(const fgla::Instance& instance, unsigned int width, unsigned int height, const std::string& title);

	unsigned int get_width() const;
	unsigned int get_height() const;

	void set_width(unsigned int);
	void set_height(unsigned int);

	bool is_ok() const;

	void run();

	~Window();
	
	struct NativeData;
	NativeData *data;
};
}
