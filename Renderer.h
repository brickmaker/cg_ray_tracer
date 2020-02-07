//
// Created by Xiaodong Zhao on 2020/2/6.
//


#ifndef CG_RAY_TRACER_RENDERER_H
#define CG_RAY_TRACER_RENDERER_H

#define GLM_ENABLE_EXPERIMENTAL

#include "libs/glm/glm.hpp"
#include "libs/glm/ext.hpp"

#include "Mesh.h"
#include "Light.h"
#include "Camera.h"

struct Renderer {
    Mesh mesh;
    Light light;
    Camera camera;
    const int width;
    const int height;

    Renderer(Mesh &mesh, Light &light, Camera &camera, int width, int height);

    buffer_t render();

    glm::vec3 cast(Ray ray);

    bool intersect(Ray ray, IntersectInfo &intersectInfo);

    // TODO：test sphere cast
    glm::vec3 castSphere(Ray ray);

    // TODO: test transform
    static Ray ortho_ray_transform(Ray ray, int width, int height);
};


#endif //CG_RAY_TRACER_RENDERER_H
