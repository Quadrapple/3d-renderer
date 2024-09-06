#include "scene.h"

Scene::Scene() : viewProjection(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL) {
    viewProjection.bindBufferBase(0);
}

void Scene::useCamera(Camera *camera) {
    this->camera = camera;
    glm::mat4 projection = camera->projectionMatrix();

    viewProjection.bufferSubData(0, sizeof(glm::mat4), glm::value_ptr(projection));
}

void Scene::addInstancedObject(Object &object, Shader &shader) {
    Drawable key = {object.model, &shader};
    drawables.push_back(key);
    const auto vector = instances.find(key);

    if(vector == instances.end()) {
        std::vector<Object*>* newVector = new std::vector<Object*>();
        newVector->reserve(128);
        newVector->push_back(&object);
        instances.emplace(key, newVector);
        return;
    }
    if(vector->second->size() >= 128) {
        std::cout << "ERROR! Cannot add more objects to model/shader combination!" << std::endl;
        return;
    }
    vector->second->push_back(&object);
    std::cout << "Object added" << std::endl;
}

void Scene::render() {

    glm::mat4 view = camera->viewMatrix();

    //Set view matrix in uniform block
    viewProjection.bufferSubData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));

    //Instanced rendering
    for(Drawable d: drawables) {
        //Extract the model matrices
        std::vector<Object*> toDrawInstanced = *instances.find(d)->second;
        std::vector<glm::mat4> modelMatrices;

        modelMatrices.reserve(toDrawInstanced.size());
        for(Object* obj: toDrawInstanced) {
            modelMatrices.push_back(obj->modelMat());
        }

        d.model->drawInstanced(*d.shader, modelMatrices);
    }
}
