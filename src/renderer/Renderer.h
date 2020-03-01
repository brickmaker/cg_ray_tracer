//
// Created by Xiaodong Zhao on 2020/2/6.
//


#ifndef CG_RAY_TRACER_RENDERER_H
#define CG_RAY_TRACER_RENDERER_H

#define GLM_ENABLE_EXPERIMENTAL

#include "../../libs/glm/glm.hpp"
#include "../../libs/glm/ext.hpp"

#include "../mesh/Mesh.h"
#include "../light/Light.h"
#include "../camera/Camera.h"
#include "../kdtree/KDTree.h"

struct Renderer {
    Mesh &mesh;
    KDTree &tree;
    vector<Light *> lights;
    Camera camera;
    const int width;
    const int height;

    Renderer(Mesh &mesh, KDTree &tree, vector<Light *> &lights, Camera &camera, int width, int height);

    ~Renderer();

    buffer_t render();

    glm::vec3 cast(Ray ray, int depth, bool is_sample_light);

//    bool intersect(Ray ray, IntersectInfo &intersectInfo);

    // TODO：test sphere cast
    glm::vec3 castSphere(Ray ray);

    // TODO: test transform
    static Ray ortho_ray_transform(Ray ray, int width, int height);

    glm::vec3 sample_lights(IntersectInfo &intersect_info, Ray &ray, glm::vec3 &Kd, glm::vec3 &Ks, float_t Ns, float_t P);
};


#endif //CG_RAY_TRACER_RENDERER_H
