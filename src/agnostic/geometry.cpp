#include <fgla/geometry.hpp>

namespace fgla {

VertexDataLayout VertexDataLayout::add(VertexDataType type, uint32_t n) {
    this->m_stride += n * VertexDataType_size(type); // we have an additional n*size bytes in our "cycle"
    this->m_attributes.push_back({ n, type }); // add attribute to our list of attributes

    return *this;
}
}