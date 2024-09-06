#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "model/model.h"
#include "shader/shader.h"
#include "scene/camera.h"
#include "scene/scene.h"
#include "inner/buffer.h"
#include "inner/context.h"
#include "inner/vao.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

int wWidth = 800;
int wHeight = 600;

float deltaTime;
float prevTime;

bool noMouseInputYet = true;
float prevMouseX = 0.0;
float prevMouseY = 0.0;
bool blockMouseInput = false;
bool keyPressed = false;

static Context *ctx;
Scene *scene;
Camera *camera;

struct Framebuffer {
    unsigned int buf;
    unsigned int color;
    unsigned int rbo;
};

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);

    wWidth = width;
    wHeight = height;
}

void processInput(GLFWwindow *window) {
    const float cameraSpeed = 4.0f * deltaTime;

    if(glfwGetKey(ctx->window, GLFW_KEY_W) == GLFW_PRESS) {
        camera->moveForward(cameraSpeed);
    }

    if(glfwGetKey(ctx->window, GLFW_KEY_S) == GLFW_PRESS) {
        camera->moveForward(-cameraSpeed);
    }

    if(glfwGetKey(ctx->window, GLFW_KEY_D) == GLFW_PRESS) {
        camera->moveRight(cameraSpeed);
    }

    if(glfwGetKey(ctx->window, GLFW_KEY_A) == GLFW_PRESS) {
        camera->moveRight(-cameraSpeed);
    }

    if(glfwGetKey(ctx->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        camera->moveUp(-cameraSpeed);
    }
    
    if(glfwGetKey(ctx->window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera->moveUp(cameraSpeed);
    }

    if(glfwGetKey(ctx->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(ctx->window, true);
    }

    if(glfwGetKey(ctx->window, GLFW_KEY_P) == GLFW_PRESS) {
        if(!keyPressed) {
            keyPressed = true;
        }
    }

    if(glfwGetKey(ctx->window, GLFW_KEY_P) == GLFW_RELEASE) {
        if(keyPressed) {
            blockMouseInput = !blockMouseInput;

            if(!blockMouseInput) {
                glfwSetInputMode(ctx->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            } else {
                glfwSetInputMode(ctx->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }

            keyPressed = false;        
        }
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if(noMouseInputYet) {
        prevMouseX = xpos;
        prevMouseY = ypos;

        noMouseInputYet = false;
    }

    float xOffset = xpos - prevMouseX;
    float yOffset = prevMouseY - ypos;

    prevMouseX = xpos;
    prevMouseY = ypos;

    const float sensitivity = 0.006;
    if(!blockMouseInput) {
        camera->changeDirection(xOffset * sensitivity, yOffset * sensitivity);
    }
}

void dbg(const std::string &name) {
    std::cout << name << std::endl;
}

inline void calcDeltaTime() {
    float time = glfwGetTime();
    deltaTime = time - prevTime;
    prevTime = time;
}

void drawLinesArray(VertexArray &vao, Shader &gridShader, unsigned int sideLen) {
    gridShader.use();
    gridShader.setVec3("color", 1.0, 1.0, 1.0);

    vao.bind();
    glDrawArrays(GL_LINES, 0, sideLen * 4 + 1);
}

VertexArray createGrid(int sideLen) {
    std::vector<glm::vec3> vertices;

    vertices.reserve(sideLen * 4);
    //Fill with zeroes
    for(int i = 0; i < sideLen * 4; i++) {
        vertices.push_back(glm::vec3(0)); 
    }

    //Half side lengths
    const int lHsl = sideLen / 2;
    const int rHsl = (sideLen % 2 == 0) ? -sideLen / 2 + 1 : -sideLen / 2;
    
    for(int i = 0; i < sideLen; i++) {
        vertices[i*2] = glm::vec3(rHsl, 0, rHsl + i);
        vertices[i*2 + 1] = glm::vec3(lHsl, 0, rHsl + i);
    }
    
    for(int i = 0; i < sideLen; i++) {
        vertices[sideLen*2 + i*2] = glm::vec3(rHsl + i, 0, lHsl);
        vertices[sideLen*2 + i*2 + 1] = glm::vec3(rHsl + i, 0, rHsl);
    }

    VertexArray vao;
    Buffer vbo(GL_ARRAY_BUFFER, vertices);
    vao.setBuffer(vbo);
    vao.addAttrib(VertexAttrib{3, sizeof(float) * 3, GL_FLOAT, 0});

    return vao;
}

Framebuffer createFramebuffer() {
    //Create the framebuffer
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    //Create a texture and attach as a color attachment to the framebuffer
    unsigned int colorBuffer;
    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wWidth, wHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    //Create a renderbuffer object to hold depth and stencil buffers
    unsigned int rboDepthStencil;
    glGenRenderbuffers(1, &rboDepthStencil);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencil);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, wWidth, wHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthStencil);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "ERROR! MAIN_FRAMEBUFFER::INCOMPLETE\n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return Framebuffer{framebuffer, colorBuffer, rboDepthStencil};
}

Framebuffer createFramebufferMultisampled() {
    //Create tyhe framebuffer
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    //Create a texture and attach as a color attachment to the framebuffer
    unsigned int colorBuffer;
    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorBuffer);

    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, wWidth, wHeight, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, colorBuffer, 0);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

    //Create a renderbuffer object to hold depth and stencil buffers
    unsigned int rboDepthStencil;
    glGenRenderbuffers(1, &rboDepthStencil);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencil);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, wWidth, wHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthStencil);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "ERROR! MAIN_FRAMEBUFFER::INCOMPLETE\n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return Framebuffer{framebuffer, colorBuffer, rboDepthStencil};
}

float floatRandom() {
    return (float)(std::rand() - RAND_MAX / 2) / RAND_MAX * 4;
}

int main(int argc, char *argv[]) {
    ctx = new Context("Renderer", glm::vec2(800, 600));

    glViewport(0, 0, wWidth, wHeight);

    glfwSetFramebufferSizeCallback(ctx->window, framebuffer_size_callback);
    glfwSetCursorPosCallback(ctx->window, mouse_callback);

    glfwSetInputMode(ctx->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    //ImGui setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplGlfw_InitForOpenGL(ctx->window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    //Scene setup----------------------------------

    //Load shaders, models
    Shader flatLight("glsl/flatLight.vert", "glsl/flatLight.frag");
    Shader gridShader("glsl/grid.vert", "glsl/color.frag");
    Shader post("glsl/post.vert", "glsl/post.frag");
    //Model backpack("backpack/backpack.obj");
    Model asteroid("asteroid/asteroid_centered.obj");

    //Bind the shader's UB to 0
    flatLight.use();
    flatLight.bindUniformBlock("Matrices", 0);
    gridShader.bindUniformBlock("Matrices", 0);
    
    scene = new Scene();
    camera = new Camera();

    camera->setPosition(glm::vec3(0.0f, 0.0f, 6.0f));
    camera->setDirection(glm::vec3(0.0f, 0.0f, -1.0f));

    scene->useCamera(camera);

    const int n = 3*4;
    float pos[n];

    for(int i = 0; i < n; i+=3) {
        //pos[i] = (float)(std::rand() - RAND_MAX / 2) / RAND_MAX * 10.0;
        pos[i] = 5.0 * sin( 6.28 * (float)i / n);
        pos[i + 1] = 5.0 * cos( 6.28 * (float)i / n);
        pos[i + 2] = 0.0;
    }

    std::vector<Object> objects;
    objects.reserve(512);
    for(int i = 0; i < n; i += 3) {
        objects.push_back(Object(&asteroid, glm::vec3(pos[i], pos[i + 1], pos[i + 2])));
    }
    for(int i = 0; i < objects.size(); i++) {
        scene->addInstancedObject(objects[i], flatLight);
    }


    Framebuffer multiFb = createFramebufferMultisampled(); //MSAA framebuffer
    Framebuffer pixelFb = createFramebuffer();

    //Screen quad-------------------------------------
    float quadVertices[] = {
        -1.0, -1.0, 0.0, 0.0,
        1.0, -1.0, 1.0, 0.0,
        1.0, 1.0, 1.0, 1.0,
        -1.0, 1.0, 0.0, 1.0,
        -1.0, -1.0, 0.0, 0.0,
        1.0, 1.0, 1.0, 1.0,
    };

    VertexArray screenQuad;
    {
        Buffer vbo(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices);
        screenQuad.setBuffer(vbo);
        screenQuad.addAttrib(VertexAttrib{2, sizeof(float) * 4, GL_FLOAT, (void*)0});
        screenQuad.addAttrib(VertexAttrib{2, sizeof(float) * 4, GL_FLOAT, (void*)(2 * sizeof(float))});
    }
    //------------------------------------------------

    //Main loop
    int l = 8;
    char filename[64];
    for(int i = 0; i < 64; i++) filename[i] = ' ';

    VertexArray grid = createGrid(l);
    while(!glfwWindowShouldClose(ctx->window)) {
        processInput(ctx->window);
        calcDeltaTime();

        //ImGui render----------------------------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("Model loader");
            ImGui::InputText("Model name", filename, sizeof(filename) / sizeof(*filename));
            if(ImGui::Button("Load")) {
                Object *obj = new Object(&asteroid, glm::vec3(floatRandom(), floatRandom(), floatRandom()));
                objects.push_back(*obj);

                scene->addInstancedObject(*obj, flatLight);
            }
            ImGui::SameLine();
            ImGui::Text("%s", filename);
            ImGui::End();
        }
        //----------------------------------------

        //Render to framebuffer-------------------
        glBindFramebuffer(GL_FRAMEBUFFER, multiFb.buf);

        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        scene->render();
        drawLinesArray(grid, gridShader, l);
        //----------------------------------------

        //Render to screen------------------------
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pixelFb.buf);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, multiFb.buf);
        glBlitFramebuffer(0, 0, wWidth, wHeight, 0, 0, wWidth, wHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        screenQuad.bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pixelFb.color);

        post.use();
        glDisable(GL_DEPTH_TEST);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glEnable(GL_DEPTH_TEST);
        //----------------------------------------

        glBindVertexArray(0);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(ctx->window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
