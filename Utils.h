//
// Created by Xiaodong Zhao on 2020/2/6.
//

#ifndef CG_RAY_TRACER_UTILS_H
#define CG_RAY_TRACER_UTILS_H

#include <fstream>
#include "defs.h"
#include "Mesh.h"

struct Utils {
    static void save_img(buffer_t buffer);

    static glm::vec3 reflect_direction(glm::vec3 normal, glm::vec3 in_direction);
    static glm::vec3 refract_direction(glm::vec3 normal, glm::vec3 in_direction, float_t n);
    static glm::vec3 random_in_sphere();
    static float_t rand();

    static buffer_t show_vertices(Mesh &mesh, int width, int height);
};


#endif //CG_RAY_TRACER_UTILS_H
