//
// Created by Xiaodong Zhao on 2020/2/7.
//

#include <iostream>
#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 look_at, glm::vec3 up_direction, float_t fov, glm::vec2 resolution) :
        position(position),
        look_at(look_at),
        up_direction(up_direction),
        fov(fov),
        resolution(resolution) {

    /*
    // camera v2
    glm::vec3 dir = glm::normalize(look_at - position);

    x_axis = glm::cross(dir, up_direction);
    y_axis = glm::cross(x_axis, dir);
    origin = position + dir * (resolution.y / 2.f) / glm::tan(glm::radians(fov / 2.f));
     */

    // camera v1
    // TODO: not consider up direction

    glm::vec3 pos_z(0, 0, 1);
    glm::vec3 neg_z(0, 0, -1);
    glm::vec3 direction = glm::normalize(look_at - position);

    glm::mat4 translate1 = glm::translate(glm::vec3(-resolution.x / 2., -resolution.y / 2., 0.));

    // move to pos z axis when direction is +z axis
//    glm::mat4 translate2 = glm::translate(glm::vec3(0, 0, (direction == pos_z ? 1. : -1.) * (resolution.y / 2.) / glm::tan(glm::radians(fov / 2.))));
    glm::mat4 translate2 = glm::translate(glm::vec3(0, 0, -(resolution.y / 2.) / glm::tan(glm::radians(fov / 2.))));

    glm::vec3 axis = glm::cross(neg_z, direction);
    float_t angle = glm::acos(glm::dot(neg_z, direction));
    glm::mat4 rotate = axis == glm::vec3(0) ? (direction == pos_z ? glm::rotate((float_t) (M_PI), glm::vec3(0, 1, 0)) : glm::mat4(1)) : glm::rotate(angle, axis);

    glm::mat4 translate3 = glm::translate(position);

    transform_matrix = translate3 * rotate * translate2 * translate1;
}

glm::mat4 Camera::get_transform_matrix() {
    return transform_matrix;
}

Ray Camera::generate_ray(float_t x, float_t y) {
    // camera v1
    glm::vec3 target = transform_matrix * glm::vec4(x, y, 0, 1);
    return {position, glm::normalize(target - position)};
    /*
    // camera v2
    glm::vec3 target = origin + (x - resolution.x / 2.f) * x_axis + (y - resolution.y / 2) * y_axis;
    return {position, glm::normalize(target - position)};
     */
}
