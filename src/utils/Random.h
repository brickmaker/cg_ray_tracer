//
// Created by Xiaodong Zhao on 2020/2/18.
//

#ifndef CG_RAY_TRACER_RANDOM_H
#define CG_RAY_TRACER_RANDOM_H


#include "../../libs/glm/vec3.hpp"
#include "defs.h"

struct Random {
    static float_t num() {
        return (float_t) std::rand() / RAND_MAX;
    }

    static glm::vec3 hemi_sphere() {
        float_t r1 = Random::num();
        float_t r2 = Random::num();
//        float_t sin_theta = glm::sqrt(1. - r1 * r1);
        float_t sin_theta = glm::sqrt(1. - r1);
        float_t phi = 2 * PI * r2;
        return glm::vec3(
                sin_theta * glm::cos(phi),
//                r1,
                glm::sqrt(r1),
                sin_theta * glm::sin(phi)
        );
    }

    static glm::vec3 sphere(float_t r, float_t d) {
        float_t r1 = Random::num();
        float_t r2 = Random::num();
        float_t y = 1 + r1 * (glm::sqrt(1. - (r * r) / (d * d)) - 1.);
//        float_t sin_theta = glm::sqrt(1. - y * y);
        float_t sin_theta = glm::sqrt(1. - y);
        float_t phi = 2 * PI * r2;
        return glm::vec3(
                sin_theta * glm::cos(phi),
                glm::sqrt(y),
                sin_theta * glm::sin(phi)
        );
    }

    static glm::vec3 specular(float_t n) {
        float_t r1 = Random::num();
        float_t r2 = Random::num();
//        float_t y = glm::pow(r1, 1. / n);
        float_t y = glm::pow(r1, 1. / (n + 1));
        float_t sin_theta = glm::sqrt(1. - y * y);
        float_t phi = 2 * PI * r2;
        return glm::vec3(
                sin_theta * glm::cos(phi),
                y,
                sin_theta * glm::sin(phi)
        );
    }
};


#endif //CG_RAY_TRACER_RANDOM_H
