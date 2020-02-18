//
// Created by Xiaodong Zhao on 2020/2/18.
//

#ifndef CG_RAY_TRACER_RANDOM_H
#define CG_RAY_TRACER_RANDOM_H


#include "libs/glm/vec3.hpp"
#include "defs.h"

struct Random {
    static float_t num() {
        return (float_t) std::rand() / RAND_MAX;
    }

    static glm::vec3 hemi_sphere() {
        float_t r1 = Random::num();
        float_t r2 = Random::num();
        float_t sin_theta = glm::sqrt(1. - r1 * r1);
        float_t phi = 2 * PI * r2;
        return glm::vec3(
                sin_theta * glm::cos(phi),
                r1,
                sin_theta * glm::sin(phi)
        );
    }
};


#endif //CG_RAY_TRACER_RANDOM_H
