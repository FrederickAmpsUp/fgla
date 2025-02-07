#pragma once

#include <fgla/util.hpp>
#include <fgla/internal.hpp>
#include <string>
#include <cstdint>

namespace fgla {

/**
* A window. This may not be supported on all platforms
*/
class Window {
public:
	/**
	* A structure describing how a Window is to be created
	*/
	struct Desc {
		/**
		* The title of the window to create
		*/
		std::string title = "";
		
		/**
		* The width (in pixels) of the window to create
		*/
		uint32_t width = 800;
		/**
		* The height (in pixels of the window to create
		*/
		uint32_t height = 600;
	}; // struct Desc
	/**
	* Constructor
	* # Parameters
	* * descriptor - The descriptor storing creation data for this Window
	* # Returns
	* The created Window on success or an error message on failure
	*/
	static cpp::result<Window, Error> create(const Desc& descriptor);

	/**
	* Move constructor
	*/
	Window(Window&& other) noexcept;

	/**
	* Default constructor
	*/
	Window();

	/**
	* Destructor
	*/
	~Window();
private:
	struct NativeData;
	std::unique_ptr<NativeData> data;
	friend class NativeDataAccessor;
}; // class Window

} // namespace fgla
