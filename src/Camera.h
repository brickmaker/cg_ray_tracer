//
// Created by Xiaodong Zhao on 2020/2/7.
//

#ifndef CG_RAY_TRACER_CAMERA_H
#define CG_RAY_TRACER_CAMERA_H

#include "../libs/glm/glm.hpp"
#include "../libs/glm/gtx/transform.hpp"
#include "defs.h"


class Camera {
private:
    glm::mat4 transform_matrix;
    glm::vec3 position;
    glm::vec3 look_at;
    glm::vec3 up_direction;
    float_t fov;
    glm::vec2 resolution;
    glm::vec3 origin;
    glm::vec3 x_axis;
    glm::vec3 y_axis;

public:
    Camera(glm::vec3 position, glm::vec3 look_at, glm::vec3 up_direction, float_t fov, glm::vec2 resolution);

    glm::mat4 get_transform_matrix();

    Ray generate_ray(float_t x, float_t y);

};


#endif //CG_RAY_TRACER_CAMERA_H
