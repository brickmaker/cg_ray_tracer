//
// Created by Xiaodong Zhao on 2020/2/18.
//

#ifndef CG_RAY_TRACER_NORMALCOORD_H
#define CG_RAY_TRACER_NORMALCOORD_H


#include <iostream>
#include "libs/glm/glm.hpp"
#include "defs.h"

struct NormalCoord {
    glm::vec3 u{};
    glm::vec3 v{};
    glm::vec3 w{};

    glm::vec3 base_u{};
    glm::vec3 base_v{};
    glm::vec3 base_w{};

    explicit NormalCoord(glm::vec3 normal) {
        v = normal;
        if (std::fabs(v.x) > std::fabs(v.y)) {
            w = glm::normalize(glm::vec3(v.z, 0, -v.x));
        } else {
            w = glm::normalize(glm::vec3(0, -v.z, v.y));
        }
        u = glm::cross(v, w);

        base_u = glm::vec3(u.x, v.x, w.x);
        base_v = glm::vec3(u.y, v.y, w.y);
        base_w = glm::vec3(u.z, v.z, w.z);
    }

    glm::vec3 local_to_world(glm::vec3 pos) {
        return glm::vec3(
                glm::dot(pos, base_u),
                glm::dot(pos, base_v),
                glm::dot(pos, base_w)
        );
    }

};


#endif //CG_RAY_TRACER_NORMALCOORD_H
