#include "config.h"
#include "Renderer.h"
#include "Shader.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "FSR Demo", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n"; return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Renderer & FBO
    Renderer renderer;
    renderer.initQuad();
    renderer.initFBO(SCR_WIDTH, SCR_HEIGHT);

    // Shaders
    Shader sceneShader("shaders/3d_vertex.txt", "shaders/3d_fragment.txt");
    Shader nearestShader("shaders/vertex.txt", "shaders/fragment_upscale.txt");
    Shader bilinearShader("shaders/vertex.txt", "shaders/fragment_upscale.txt");
    Shader sharpenShader("shaders/vertex.txt", "shaders/fragment_sharpen.txt");
    Shader easuShader("shaders/vertex.txt", "shaders/fragment_easu.txt");

    nearestShader.use();
    nearestShader.setInt("uTexture", 0);
    bilinearShader.use();
    bilinearShader.setInt("uTexture", 0);
    sharpenShader.use();
    sharpenShader.setInt("uTexture", 0);
    easuShader.use();
    easuShader.setInt("uTexture", 0);

    // Cube setup
    float cubeVertices[] = {
        // positions          // texcoords
        // Front face
        -0.5f,-0.5f, 0.5f,   0.0f, 0.0f,
         0.5f,-0.5f, 0.5f,   1.0f, 0.0f,
         0.5f, 0.5f, 0.5f,   1.0f, 1.0f,
         0.5f, 0.5f, 0.5f,   1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f,   0.0f, 1.0f,
        -0.5f,-0.5f, 0.5f,   0.0f, 0.0f,

        // Back face
        -0.5f,-0.5f,-0.5f,   1.0f, 0.0f,
         0.5f,-0.5f,-0.5f,   0.0f, 0.0f,
         0.5f, 0.5f,-0.5f,   0.0f, 1.0f,
         0.5f, 0.5f,-0.5f,   0.0f, 1.0f,
        -0.5f, 0.5f,-0.5f,   1.0f, 1.0f,
        -0.5f,-0.5f,-0.5f,   1.0f, 0.0f,

        // Left face
        -0.5f,-0.5f,-0.5f,   0.0f, 0.0f,
        -0.5f,-0.5f, 0.5f,   1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f,   1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f,   1.0f, 1.0f,
        -0.5f, 0.5f,-0.5f,   0.0f, 1.0f,
        -0.5f,-0.5f,-0.5f,   0.0f, 0.0f,

        // Right face
         0.5f,-0.5f,-0.5f,   1.0f, 0.0f,
         0.5f,-0.5f, 0.5f,   0.0f, 0.0f,
         0.5f, 0.5f, 0.5f,   0.0f, 1.0f,
         0.5f, 0.5f, 0.5f,   0.0f, 1.0f,
         0.5f, 0.5f,-0.5f,   1.0f, 1.0f,
         0.5f,-0.5f,-0.5f,   1.0f, 0.0f,

        // Top face
        -0.5f, 0.5f,-0.5f,   0.0f, 1.0f,
         0.5f, 0.5f,-0.5f,   1.0f, 1.0f,
         0.5f, 0.5f, 0.5f,   1.0f, 0.0f,
         0.5f, 0.5f, 0.5f,   1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f,   0.0f, 0.0f,
        -0.5f, 0.5f,-0.5f,   0.0f, 1.0f,

        // Bottom face
        -0.5f,-0.5f,-0.5f,   0.0f, 0.0f,
         0.5f,-0.5f,-0.5f,   1.0f, 0.0f,
         0.5f,-0.5f, 0.5f,   1.0f, 1.0f,
         0.5f,-0.5f, 0.5f,   1.0f, 1.0f,
        -0.5f,-0.5f, 0.5f,   0.0f, 1.0f,
        -0.5f,-0.5f,-0.5f,   0.0f, 0.0f
    };



    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    unsigned int cubeTexture;
    glGenTextures(1, &cubeTexture);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);

    // Set parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image (e.g., using stb_image)
    int width, height, nrChannels;
    unsigned char* data = stbi_load("assets/low_res_image.png", &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    int mode = 0;
    float fps = 0.0f;
    float lastTime = glfwGetTime();
    int nbFrames = 0;

    while (!glfwWindowShouldClose(window)) {
        // Input
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) mode = 0;
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) mode = 1;
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) mode = 2;

        // FPS calculation
        float currentTime = glfwGetTime();
        nbFrames++;
        if (currentTime - lastTime >= 1.0f) {
            fps = nbFrames / (currentTime - lastTime);
            nbFrames = 0;
            lastTime += 1.0f;
        }

        // 1️⃣ Render 3D cube to FBO
        glBindFramebuffer(GL_FRAMEBUFFER, renderer.fbo);
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        sceneShader.use();
        sceneShader.setInt("uTexture", 0);
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime()/10, glm::vec3(0,1,0));
        glm::mat4 view = glm::lookAt(glm::vec3(2,2,2), glm::vec3(0,0,0), glm::vec3(0,1,0));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);
        sceneShader.setMat4("model", model);
        sceneShader.setMat4("view", view);
        sceneShader.setMat4("projection", projection);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2️⃣ Apply upscaling shader
        glViewport(0,0,SCR_WIDTH,SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        switch(mode){
            case 0:
                nearestShader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, renderer.fboTexture);
            nearestShader.setInt("uTexture", 0);
            break;
            case 1:
                bilinearShader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, renderer.fboTexture);
            bilinearShader.setInt("uTexture", 0);
            break;
            case 2:
                sharpenShader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, renderer.fboTexture);
            sharpenShader.setInt("uTexture", 0);
            break;
            case 3:
                easuShader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, renderer.fboTexture);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, renderer.fboDepthTexture);
            easuShader.setInt("uTexture", 0);
            easuShader.setInt("uDepth", 1);
            easuShader.setVec2("uTexSize", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
            easuShader.setVec2("uScreenSize", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
            break;
        }


        // Draw fullscreen quad
        renderer.renderQuad();



        // 3️⃣ ImGui overlay
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Info");
        ImGui::Text("FPS: %.1f",fps);
        ImGui::Text("Mode: %d",mode);
        ImGui::Text("Toggle mode:");
        if(ImGui::Button("Nearest")) mode=0;
        if(ImGui::Button("Bilinear")) mode=1;
        if(ImGui::Button("Bilinear+Sharpen")) mode=2;
        if(ImGui::Button("EASU+RCAS")) mode=3;
        ImGui::End();


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}
