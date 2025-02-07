#pragma once

#include <memory>
#include <fgla/internal.hpp>
#include <fgla/util.hpp>
#include <fgla/window.hpp>
#include <optional>
#include <functional>
#include <fgla/device.hpp>

namespace fgla {

/**
* An fgla instance
*/
class Instance {
public:
	/**
	* A structure describing how an Instance is to be created
	*/
	struct Desc {
		/**
		* The name of the application using fgla
		*/
		std::string app_name = "";
		/**
		* The version of the application using fgla
		*/
		fgla::Version app_version = {};

		/**
		* The window to use for instance creation. If null, the insance is only guaranteed to support offscreen rendering.
		*/
		fgla::Window *window = nullptr;
	}; // struct Desc

	/**
	* Constructor
	* # Parameters
	* * descriptor - The descriptor storing creation data for this Instance
	* # Returns
	* The created Instance on success or an error message on failure
	*/
	static cpp::result<Instance, Error> create(const Desc& desriptor);

	/**
	* Retrieve a Device
	* # Parameters
	* * descriptor - The descriptor storing requirements and preferences for device selection
	* # Returns
	* The selected Device on success or an error message on failure
	*/
	cpp::result<Device, Error> get_device(const Device::Desc& descriptor);

	/**
	* Move constructor
	*/
	Instance(Instance&& other) noexcept;

	/**
	* Default constructor
	*/
	Instance();

	/**
	* Destructor
	*/
	~Instance();
private:
	struct NativeData;
	std::unique_ptr<NativeData> data;
	friend class fgla::NativeDataAccessor;

}; // class Instance

} // namespace fgla
