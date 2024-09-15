#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "event_handler.h"
#include "model/model.h"
#include "shader/shader.h"
#include "scene/camera.h"
#include "scene/scene.h"
#include "inner/buffer.h"
#include "inner/state.h"
#include "inner/vao.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

int wWidth = 800;
int wHeight = 600;

static EventHandler *ctx;
Scene *scene;
Camera *camera;

struct Framebuffer {
    unsigned int buf;
    Texture color;
    unsigned int rbo;
};

class ControlKeyListener : public KeyPressListener {
    public:
        ControlKeyListener() {
            ctx->addListener((KeyPressListener*)this);
        }
        virtual void onKeyboardAction(KeyPress press) override {
            if(press.action != GLFW_PRESS) {
                return;
            }
            switch(press.key) {
                case GLFW_KEY_P:
                    if(keyPressed) {
                        ctx->enableMouse();
                        ctx->setCursorMode(GLFW_CURSOR_DISABLED);
                    } else {
                        ctx->disableMouse();
                        ctx->setCursorMode(GLFW_CURSOR_NORMAL);
                    }
                    keyPressed  = !keyPressed;
                    break;
                case GLFW_KEY_ESCAPE:
                    glfwSetWindowShouldClose(ctx->window, true);
                    break;
            }
        }
    private:
        bool keyPressed = false;
};
ControlKeyListener *cl;


void dbg(const std::string &name) {
    std::cout << name << std::endl;
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
    Texture colorMap(GL_TEXTURE_2D, GL_RGB, {wWidth, wHeight}, GL_RGB, nullptr);
    colorMap.bind(0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorMap.getId(), 0);

    //Create a renderbuffer object to hold depth and stencil buffers
    unsigned int rboDepthStencil;
    glGenRenderbuffers(1, &rboDepthStencil);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencil);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, wWidth, wHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthStencil);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "ERROR! PIXEL_FRAMEBUFFER::INCOMPLETE\n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return Framebuffer{framebuffer, colorMap, rboDepthStencil};
}

Framebuffer createFramebufferMultisampled() {
    //Create tyhe framebuffer
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    //Create a texture and attach as a color attachment to the framebuffer
    Texture colorMap(GL_TEXTURE_2D_MULTISAMPLE, GL_RGB, {wWidth, wHeight}, 4, true);
    colorMap.bind(0);
    //glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorMap.getId());
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, colorMap.getId(), 0);
    //glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

    //Create a renderbuffer object to hold depth and stencil buffers
    unsigned int rboDepthStencil;
    glGenRenderbuffers(1, &rboDepthStencil);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencil);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, wWidth, wHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthStencil);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "ERROR! MULTISAMPLED_FRAMEBUFFER::INCOMPLETE\n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return Framebuffer{framebuffer, colorMap, rboDepthStencil};
}

float floatRandom() {
    return (float)(std::rand() - RAND_MAX / 2) / RAND_MAX * 4;
}

int main(int argc, char *argv[]) {
    ctx = new EventHandler("Renderer", glm::vec2(800, 600));
    ctx->setCursorMode(GLFW_CURSOR_DISABLED);
    ctx->enable(GL_DEPTH_TEST);

    cl = new ControlKeyListener();

    ctx->setDepthFunc(GL_LEQUAL);

    //glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

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
    Shader flatLightTextured("glsl/flatLight.vert", "glsl/simpleTexture.frag");
    Shader gridShader("glsl/grid.vert", "glsl/color.frag");
    Shader post("glsl/post.vert", "glsl/post.frag");
    Model backpack("backpack/backpack.obj");
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
    objects.push_back(Object(&backpack, glm::vec3(0.0)));
    for(int i = 0; i < n; i += 3) {
        objects.push_back(Object(&asteroid, glm::vec3(pos[i], pos[i + 1], pos[i + 2])));
    }
    scene->addInstancedObject(objects[0], flatLightTextured);
    for(int i = 1; i < objects.size(); i++) {
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

    int c = 0;
    glfwSwapInterval(1);

    VertexArray grid = createGrid(l);
    while(!glfwWindowShouldClose(ctx->window)) {
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
            ImGui::Text("%.6f", ctx->getDeltaTime() * 1000);
            ImGui::End();
        }
        //----------------------------------------
        //
        //There's likely an issue with the drivers (disappears in RenderDoc), so the event handling has to come with some delay after glfwSwapBuffers
        ctx->pollEvents();

        //Render to framebuffer-------------------
        glBindFramebuffer(GL_FRAMEBUFFER, multiFb.buf);
        glViewport(0, 0, 800, 600);

        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene->render();
        drawLinesArray(grid, gridShader, l);
        //----------------------------------------

        ctx->setViewport(ctx->getViewport());
        //Render to screen------------------------
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pixelFb.buf);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, multiFb.buf);
        glBlitFramebuffer(0, 0, wWidth, wHeight, 0, 0, wWidth, wHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        screenQuad.bind();
        pixelFb.color.bind(0);
        post.setInt("screenTexture", 0);

        post.use();
        glDisable(GL_DEPTH_TEST);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glEnable(GL_DEPTH_TEST);
        //----------------------------------------

        glBindVertexArray(0);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(ctx->window);
    }

    glfwTerminate();
    return 0;
}
