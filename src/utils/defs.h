//
// Created by Xiaodong Zhao on 2020/2/6.
// Define some constants or types
//

#ifndef CG_RAY_TRACER_DEFS_H
#define CG_RAY_TRACER_DEFS_H

#include <vector>

#include "../../libs/glm/glm.hpp"
#include "../../libs/glm/ext.hpp"

#include "../configs.h"

typedef float float_t;

typedef std::vector<std::vector<glm::vec3>> buffer_t;
typedef std::vector<glm::vec3> buffer_row_t;

const float_t FLOAT_INF = std::numeric_limits<float_t>::max();
const float_t EPSILON = 1e-3;
const float_t PI = glm::pi<float_t>();

struct Ray {
    glm::vec3 origin{};
    glm::vec3 direction{};

    Ray() = default;

    Ray(glm::vec3 origin, glm::vec3 direction) : origin(origin), direction(direction) {}
};

struct IntersectInfo {
    size_t shape_i;
    size_t face_i;
    size_t material_id;
    glm::vec3 pos;
    glm::vec3 normal;
    // TODO: not consider material
};

#endif //CG_RAY_TRACER_DEFS_H
