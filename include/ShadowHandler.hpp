#ifndef SHADOW_HANDLER_HPP
#define SHADOW_HANDLER_HPP

class ShadowHandler
{
public:
    void initShadows();
    void bindShadows();
    void unbindShadows();

    void activateTexture();
    void deactivateTexture();

    unsigned int depthMapFBO;
    unsigned int depthMap;
    static constexpr unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
};

#endif //SHADOW_HANDLER_HPP
