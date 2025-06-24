#pragma once

#include <fgla/adapter.hpp>

namespace fgla::backends::vulkan {

struct AdapterImpl : public fgla::Adapter::Impl {
	virtual ~AdapterImpl() override = default;
};

}
