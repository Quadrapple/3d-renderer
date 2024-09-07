#ifndef CONTEXT_H
#define CONTEXT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <unordered_map>

struct BindingPoint {
    GLenum target;
    unsigned int point;
    bool operator==(const BindingPoint &rhs) const {
        return this->target == rhs.target && this->point == rhs.point;
    }
};

struct BindingPointHasher {
    std::size_t operator()(const BindingPoint& lhs) const {
        std::size_t h1 = std::hash<unsigned int>{}(lhs.point);
        std::size_t h2 = std::hash<GLenum>{}(lhs.target);
        return h1 ^ (h2 << 1);
    }
};

class State {
    public:
        GLFWwindow *window;
        State(std::string title, glm::uvec2 size);

        //Map of target-buffer binds
        std::unordered_map<GLenum, unsigned int> targets;

        //Map of bindingPoint-buffer binds
        std::unordered_map<BindingPoint, unsigned int, BindingPointHasher> bindingPoints;

        //Map of enabled things
        std::unordered_map<GLenum, bool> enabled;

        //Map of textures
        std::unordered_map<GLenum, unsigned int> textures;

        unsigned int boundVaoID = 0;
        unsigned int usedShaderID = 0;

        static const unsigned int MAJOR_VER = 3;
        static const unsigned int MINOR_VER = 3;

        static inline std::unordered_map<unsigned int, State*> hack;
        static State& getContext();

        float getDeltaTime();

        void bindBuffer(GLenum target, unsigned int id);
        void bindBufferBase(GLenum target, unsigned int index, unsigned int id);
        void bindVertexArray(unsigned int id);
        void useProgram(unsigned int id);

        void setViewport(glm::uvec2 size);

        void enable(GLenum option);
        void disable(GLenum option);
    protected:
        float deltaTime;
        float prevTime;
    private:
        glm::uvec2 viewportSize;

        static void initGLFW();
        static void initGLAD();

        void createWindow(std::string title, glm::uvec2 size);
};

#endif
