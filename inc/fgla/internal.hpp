#pragma once

namespace fgla {

/**
* A class which may be inherited to grant access to the internal NativeData of fgla classes
*/
class NativeDataAccessor {
protected:
	/**
	* Get the internal NativeData of an fgla class
	* # Template Parameters
	* * T - The fgla class to access
	* # Parameters
	* * t - the class whose data to access
	* # Returns
	* A pointer to the class's internal NativeData
	*/
	template<typename T>
	static auto data(const T& t) -> typename T::NativeData * {
		return t.data.get();
	}
};

} // namespace fgla
