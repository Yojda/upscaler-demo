#include "config.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() {
    int mode = 1; // default to bilinear (0 = nearest, 1 = bilinear)
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "FSR Demo", nullptr, nullptr);
    if (!window) { std::cout << "Failed to create GLFW window\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { std::cout << "Failed to initialize GLAD\n"; return -1; }

    // Quad setup
    float quadVertices[] = {
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,

        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f
    };
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO); glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    Shader shader("shaders/vertex.txt", "shaders/fragment_upscale.txt");
    Shader sharpenShader("shaders/vertex.txt", "shaders/fragment_sharpen.txt");
    Shader easuShader("shaders/vertex.txt", "shaders/fragment_easu.txt");
    Shader rcasShader("shaders/vertex.txt", "shaders/fragment_rcas.txt");

    // Load texture
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("assets/low_res_image.png", &width, &height, &nrChannels, 0);
    unsigned int texture;
    glGenTextures(1, &texture);

    if (!data) {
        std::cout << "Failed to load texture!" << std::endl;
    } else {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
    }

    shader.use();
    shader.setInt("uTexture", 0);
    glUniform1i(glGetUniformLocation(shader.ID, "uMode"), mode == 2 ? 1 : mode);

    sharpenShader.use();
    sharpenShader.setInt("uTexture", 0);
    glUniform1f(glGetUniformLocation(sharpenShader.ID, "uSharpness"), 0.2f);

    // set the texture size uniform (make sure shader is in use)
    GLint locTexSize = glGetUniformLocation(shader.ID, "uTexSize");
    if (locTexSize >= 0) {
        glUniform2f(locTexSize, (float)width, (float)height);
    }

    // set initial mode
    GLint locMode = glGetUniformLocation(shader.ID, "uMode");
    if (locMode >= 0) {
        glUniform1i(locMode, mode);
    }

    unsigned int fbo, fboTexture;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create texture to render into
    glGenTextures(1, &fboTexture);
    glBindTexture(GL_TEXTURE_2D, fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // attach texture to FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);

    // check completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR: Framebuffer not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    while (!glfwWindowShouldClose(window)) {
    // Input
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) { mode = 0; std::cout << "Mode 0\n"; }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) { mode = 1; std::cout << "Mode 1\n"; }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) { mode = 2; std::cout << "Mode 2\n"; }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) { mode = 3; std::cout << "Mode 3 (EASU + RCAS)\n"; }

    // Clear screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);

    if (mode == 0) {
        // Nearest
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        shader.use();
        glUniform1i(glGetUniformLocation(shader.ID, "uMode"), mode);
        glUniform2f(glGetUniformLocation(shader.ID, "uTexSize"), (float)width, (float)height);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

    } else if (mode == 1) {
        // Bilinear
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        shader.use();
        glUniform1i(glGetUniformLocation(shader.ID, "uMode"), mode);
        glUniform2f(glGetUniformLocation(shader.ID, "uTexSize"), (float)width, (float)height);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

    } else if (mode == 2) {
        // Bilinear + sharpen (multi-pass)
        // Pass 1: Render bilinear -> FBO
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        glUniform1i(glGetUniformLocation(shader.ID, "uMode"), 1); // bilinear
        glUniform2f(glGetUniformLocation(shader.ID, "uTexSize"), (float)width, (float)height);

        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Pass 2: Render sharpen -> screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT);

        sharpenShader.use();
        glBindTexture(GL_TEXTURE_2D, fboTexture);
        glUniform1f(glGetUniformLocation(sharpenShader.ID, "uSharpness"), 0.2f);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

    } else if (mode == 3) {
        // EASU + RCAS (placeholders for AMD FSR)
        // Pass 1: EASU upscaling
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT);

        easuShader.use(); // <-- You need to implement/load EASU shader
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform2f(glGetUniformLocation(easuShader.ID, "uInputSize"), (float)width, (float)height);
        glUniform2f(glGetUniformLocation(easuShader.ID, "uOutputSize"), (float)SCR_WIDTH, (float)SCR_HEIGHT);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Pass 2: RCAS sharpening
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT);

        rcasShader.use(); // <-- You need to implement/load RCAS shader
        glBindTexture(GL_TEXTURE_2D, fboTexture);
        glUniform1f(glGetUniformLocation(rcasShader.ID, "uSharpness"), 0.2f);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}


    glfwTerminate();
    return 0;
}
