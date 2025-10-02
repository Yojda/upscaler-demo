#include "config.h"
#include "Shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image/std_image.h>
#include <iostream>
#include <chrono>

// ImGui
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() {
    // ----------------- GLFW Init -----------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH,SCR_HEIGHT,"FSR Demo",nullptr,nullptr);
    if(!window){ std::cout<<"Failed to create GLFW window\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){ std::cout<<"Failed to initialize GLAD\n"; return -1; }

    // ----------------- ImGui Init -----------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window,true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // ----------------- Quad Setup -----------------
    float quadVertices[] = {
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f,-1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,-1.0f, 0.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         1.0f,-1.0f, 0.0f, 1.0f, 0.0f,
         1.0f, 1.0f, 0.0f, 1.0f, 1.0f
    };
    unsigned int VAO,VBO;
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(quadVertices),quadVertices,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // ----------------- Shaders -----------------
    Shader shader("shaders/vertex.txt","shaders/fragment_upscale.txt");
    Shader sharpenShader("shaders/vertex.txt","shaders/fragment_sharpen.txt");
    Shader easuShader("shaders/vertex.txt","shaders/fragment_easu.txt");
    Shader rcasShader("shaders/vertex.txt","shaders/fragment_rcas.txt");

    // ----------------- Load Texture -----------------
    int width,height,nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("assets/low_res_image.png",&width,&height,&nrChannels,0);
    unsigned int texture;
    glGenTextures(1,&texture);

    if(!data){ std::cout<<"Failed to load texture!\n"; }
    else {
        GLenum format = (nrChannels==4)?GL_RGBA:GL_RGB;
        glBindTexture(GL_TEXTURE_2D,texture);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D,0,format,width,height,0,format,GL_UNSIGNED_BYTE,data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }

    // ----------------- FBO Setup -----------------
    unsigned int fbo,fboTexture;
    glGenFramebuffers(1,&fbo);
    glBindFramebuffer(GL_FRAMEBUFFER,fbo);
    glGenTextures(1,&fboTexture);
    glBindTexture(GL_TEXTURE_2D,fboTexture);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,SCR_WIDTH,SCR_HEIGHT,0,GL_RGB,GL_UNSIGNED_BYTE,nullptr);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,fboTexture,0);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE)
        std::cout<<"ERROR: Framebuffer not complete!\n";
    glBindFramebuffer(GL_FRAMEBUFFER,0);

    // ----------------- Main Loop -----------------
    int mode = 1; // default: bilinear
    float fps = 0.0f;
    while(!glfwWindowShouldClose(window)){
        // Poll events
        glfwPollEvents();

        // ----------------- FPS -----------------
        auto start = std::chrono::high_resolution_clock::now();

        switch(mode) {
            case 0: /* nearest */ break;
            case 1: /* bilinear */ break;
            case 2: /* bilinear + sharpen */ break;
            case 3: /* EASU + RCAS */ break;
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> duration = end - start;
        fps = 1.0f / duration.count();

        // ----------------- ImGui Frame -----------------
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

        // ----------------- Clear -----------------
        glViewport(0,0,SCR_WIDTH,SCR_HEIGHT);
        glClearColor(0.1f,0.1f,0.1f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);

        // ----------------- Render Scene -----------------
        if(mode==0){
            glBindTexture(GL_TEXTURE_2D,texture);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
            shader.use();
            glUniform1i(glGetUniformLocation(shader.ID,"uMode"),mode);
            glUniform2f(glGetUniformLocation(shader.ID,"uTexSize"),(float)width,(float)height);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES,0,6);
        } else if(mode==1){
            glBindTexture(GL_TEXTURE_2D,texture);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            shader.use();
            glUniform1i(glGetUniformLocation(shader.ID,"uMode"),mode);
            glUniform2f(glGetUniformLocation(shader.ID,"uTexSize"),(float)width,(float)height);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES,0,6);
        } else if(mode==2){
            // Multi-pass: bilinear -> sharpen
            glBindFramebuffer(GL_FRAMEBUFFER,fbo);
            glViewport(0,0,SCR_WIDTH,SCR_HEIGHT);
            glClear(GL_COLOR_BUFFER_BIT);
            shader.use();
            glUniform1i(glGetUniformLocation(shader.ID,"uMode"),1);
            glUniform2f(glGetUniformLocation(shader.ID,"uTexSize"),(float)width,(float)height);
            glBindTexture(GL_TEXTURE_2D,texture);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES,0,6);

            glBindFramebuffer(GL_FRAMEBUFFER,0);
            glViewport(0,0,SCR_WIDTH,SCR_HEIGHT);
            glClear(GL_COLOR_BUFFER_BIT);
            sharpenShader.use();
            glBindTexture(GL_TEXTURE_2D,fboTexture);
            glUniform1f(glGetUniformLocation(sharpenShader.ID,"uSharpness"),0.2f);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES,0,6);
        } else if(mode==3){
            // Multi-pass: EASU -> RCAS
            glBindFramebuffer(GL_FRAMEBUFFER,fbo);
            glViewport(0,0,SCR_WIDTH,SCR_HEIGHT);
            glClear(GL_COLOR_BUFFER_BIT);
            easuShader.use();
            glBindTexture(GL_TEXTURE_2D,texture);
            glUniform2f(glGetUniformLocation(easuShader.ID,"uTexSize"),(float)width,(float)height);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES,0,6);

            glBindFramebuffer(GL_FRAMEBUFFER,0);
            glViewport(0,0,SCR_WIDTH,SCR_HEIGHT);
            glClear(GL_COLOR_BUFFER_BIT);
            rcasShader.use();
            glBindTexture(GL_TEXTURE_2D,fboTexture);
            glUniform1f(glGetUniformLocation(rcasShader.ID,"uSharpness"),0.2f);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES,0,6);
        }

        // ----------------- Render ImGui -----------------
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // ----------------- Cleanup -----------------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
