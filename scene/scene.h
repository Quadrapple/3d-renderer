#ifndef SCENE_H
#define SCENE_H

#include "object.h"
#include "camera.h"

#include <vector>
#include <unordered_map>

//For instanced rendering
struct Drawable {
    Model *model;
    Shader *shader;

    bool operator==(const Drawable& lhs) const {
        return lhs.shader == this->shader && lhs.model == this->model;
    }
};

struct DrawableHasher {
    std::size_t operator()(const Drawable& d) const {
        std::size_t h1 = std::hash<Model*>{}(d.model);
        std::size_t h2 = std::hash<Shader*>{}(d.shader);
        return h1 ^ (h2 << 1);
    }
};

class Scene {
    public:
        std::vector<Drawable> drawables;
        Camera *camera;
        Scene();

        void useCamera(Camera *camera);
        void addInstancedObject(Object &object, Shader &shader);
        void render();
    private:
        void setViewMatrix();
        unsigned int uboViewProjection; //Projection + view matrices uniform block
        std::unordered_map<Drawable, std::vector<Object*>*, DrawableHasher> instances;
};

#endif
