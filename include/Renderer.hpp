#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <glm/mat4x4.hpp>

struct RendererFrameData
{
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::vec3 cameraPosition;
};

class Renderer
{
public:
    static Renderer& instance();

    void beginFrame();

    const RendererFrameData& getFrameData() const;

private:
    Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    RendererFrameData m_frameData;
};


#endif //RENDERER_HPP
