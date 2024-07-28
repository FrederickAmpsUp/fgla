#pragma once

#include <fgla/instance.hpp>
#include <fgla/geometry.hpp>
#include <vector>
#include <memory>

namespace fgla {

/**
 * Simple wrapper for vertex and fragment shader SPIR-V soruce data
 */
class Shader {
public:
    Shader(const std::vector<char>& vertSpirv, const std::vector<char>& fragSpirv) : m_vertSpirv(vertSpirv), m_fragSpirv(fragSpirv) { }

    const std::vector<char>& vertSpirv() const { return m_vertSpirv; }
    const std::vector<char>& fragSpirv() const { return m_fragSpirv; }
private:
    std::vector<char> m_vertSpirv;
    std::vector<char> m_fragSpirv;
};

class Renderer {
public:
    
private:
    std::shared_ptr<fgla::Instance> m_instance;
    std::shared_ptr<fgla::Shader> m_shader;
    std::shared_ptr<fgla::VertexDataLayout> m_vertexDataLayout;
};
}