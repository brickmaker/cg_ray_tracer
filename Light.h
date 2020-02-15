//
// Created by Xiaodong Zhao on 2020/2/7.
//

#ifndef CG_RAY_TRACER_LIGHT_H
#define CG_RAY_TRACER_LIGHT_H

#include <string>
#include "libs/glm/vec3.hpp"
#include "libs/glm/ext.hpp"

struct Light {
    glm::vec3 direction;

    explicit Light(glm::vec3 direction) : direction(glm::normalize(direction)) {}
};

struct PointLight {
    const glm::vec3 pos;
    const float_t r;
    const float_t intensity;
    const std::string material_name;
};


#endif //CG_RAY_TRACER_LIGHT_H
