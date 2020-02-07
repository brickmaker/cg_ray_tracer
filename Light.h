//
// Created by Xiaodong Zhao on 2020/2/7.
//

#ifndef CG_RAY_TRACER_LIGHT_H
#define CG_RAY_TRACER_LIGHT_H

#include "libs/glm/vec3.hpp"

struct Light {
    glm::vec3 direction;

    explicit Light(glm::vec3 direction) : direction(direction) {}
};


#endif //CG_RAY_TRACER_LIGHT_H
