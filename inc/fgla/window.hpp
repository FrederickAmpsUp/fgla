#pragma once
#include <string>

namespace fgla {

class Window {
public:
	Window(unsigned int width, unsigned int height, const std::string& title);

	unsigned int get_width();
	unsigned int get_height();

	void set_width(unsigned int);
	void set_height(unsigned int);

	bool is_ok();

	void run();

	~Window();
	
	struct NativeData;
	NativeData *data;
};
}
