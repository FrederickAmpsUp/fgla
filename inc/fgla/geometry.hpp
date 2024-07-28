#pragma once

#include <stdint.h>
#include <vector>
#include <tuple>
#include <memory>
#include <stdexcept>

namespace fgla {

enum class VertexDataType {
    FLOAT,
    INT,
    UINT
};

size_t VertexDataType_size(VertexDataType type) {
    switch (type) {
        case VertexDataType::FLOAT:
        case VertexDataType::INT:
        case VertexDataType::UINT:
            return 4;
    }
    return 0;
}

class VertexDataLayout {
public:

    static VertexDataLayout empty() { return VertexDataLayout(); }
    VertexDataLayout add(VertexDataType type, uint32_t count);

    uint32_t stride() const { return m_stride; }
private:
    uint32_t m_stride;
                               // n, type
    std::vector<std::tuple<uint32_t, VertexDataType>> m_attributes;
};

// Not to be confused with the inaptly named g-buffers of today's rendering technologies
class GeometryBuffer {
public:

    /**
     * @brief Add vertex data to the buffer.
     * @param v The vertex to append to the vertex buffer.
     * This function sends NO data to the GPU.
     */
    template<typename T>
    void add_vertex(const T& v) {
        size_t size = sizeof(T);

        if (size != this->m_vertexDataLayout->stride()) {
            throw std::runtime_error("Vertex data of improper size (expected " + this->m_vertexDataLayout->stride() + ", got " + size + ")");
        }

        m_vertexBuffer.reserve(size + m_vertexBuffer.size());

        const char *p = (const char *)&v;
        m_vertexBuffer.insert(m_vertexBuffer.end(), p, p + size);
    }
private:
    std::vector<char> m_vertexBuffer;
    std::vector<uint32_t> m_indexBuffer;

    std::shared_ptr<VertexDataLayout> m_vertexDataLayout;
};
}