#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"

class Renderer {
public:
    unsigned int VAO, VBO;
    unsigned int fbo, fboTextureLinear, fboTextureNearest, fboDepthTexture;

    void initQuad();
    void initFBO(int width, int height);
    void renderQuad();
};
