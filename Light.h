//
// Created by Xiaodong Zhao on 2020/2/7.
//

#ifndef CG_RAY_TRACER_LIGHT_H
#define CG_RAY_TRACER_LIGHT_H

#include <string>
#include "libs/glm/vec3.hpp"
#include "libs/glm/ext.hpp"
#include "libs/glm/gtx/norm.hpp"
#include "NormalCoord.h"
#include "Random.h"
#include "Mesh.h"
#include "KDTree.h"
#include "Utils.h"

struct Light {
    const glm::vec3 intensity;
    const std::string material_name;

    Light(const glm::vec3 &intensity, const std::string &material_name) :
            intensity(intensity), material_name(material_name) {}

    virtual glm::vec3 sample(
            const Mesh &mesh,
            const KDTree &tree,
            const glm::vec3 &hit_pos,
            const glm::vec3 &hit_normal,
            const glm::vec3 &ray_in_direction,
            const glm::vec3 &Kd,
            const glm::vec3 &Ks,
            const float_t &Ns,
            const float_t &P // TODO: P usage
    ) = 0;
};


struct DistantLight {
    glm::vec3 direction;

    explicit DistantLight(glm::vec3 direction) : direction(glm::normalize(direction)) {}
};

struct SquareLight : Light {
    const glm::vec3 pos1;
    const glm::vec3 pos2;
    const glm::vec3 pos3;
    const glm::vec3 pos4;
    glm::vec3 normal{};
    glm::vec3 u_axis{};
    glm::vec3 v_axis{};
    float_t width;
    float_t height;
    float_t area;

    SquareLight(
            const glm::vec3 &pos1,
            const glm::vec3 &pos2,
            const glm::vec3 &pos3,
            const glm::vec3 &pos4,
            const glm::vec3 &intensity,
            const std::string &material_name
    ) : Light(intensity, material_name),
        pos1(pos1),
        pos2(pos2),
        pos3(pos3),
        pos4(pos4) {
        width = glm::distance(pos2, pos1);
        height = glm::distance(pos4, pos1);
        area = width * height;
        u_axis = glm::normalize(pos2 - pos1);
        v_axis = glm::normalize(pos4 - pos1);
        normal = glm::normalize(glm::cross(u_axis, v_axis));
    }

    glm::vec3
    sample(const Mesh &mesh,
           const KDTree &tree,
           const glm::vec3 &hit_pos,
           const glm::vec3 &hit_normal,
           const glm::vec3 &ray_in_direction,
           const glm::vec3 &Kd,
           const glm::vec3 &Ks,
           const float_t &Ns,
           const float_t &P) override {
        float_t eps1 = Random::num();
        float_t eps2 = Random::num();
        glm::vec3 target = pos1 + eps1 * u_axis + eps2 * v_axis;

        glm::vec3 light_dir = -glm::normalize(target - hit_pos);
        Ray shadow_ray(hit_pos + EPSILON * -light_dir, -light_dir);
        IntersectInfo shadow_intersect_info{};

        bool shadow_intersect = tree.intersect(shadow_ray, shadow_intersect_info);

        if (mesh.materials[shadow_intersect_info.material_id].name == material_name) {
//            glm::vec3 diffuse = Kd * final_intensity * light_dist_2 * glm::max(glm::dot(-light_dir_normalized, intersect_info.normal), 0.f);
            glm::vec3 diffuse = Kd * intensity * glm::max(glm::dot(-light_dir, hit_normal), 0.f);
            // Phong
//            glm::vec3 specular = Ks * final_intensity * glm::pow(glm::max(glm::dot(-light_dir, Utils::reflect_direction(intersect_info.normal, ray.direction)), 0.f), Ns);
            // Phong-specular
            glm::vec3 specular = Ks * intensity * (Ns + 1.f) * 0.5f * (float_t) M_PI *
                                 glm::pow(glm::max(glm::dot(-light_dir, Utils::reflect_direction(hit_normal, ray_in_direction)), 0.f), Ns);
//            glm::vec3 specular = glm::vec3(0);

            // TODO: both side square light
            float_t pdf_inv = area * glm::abs(glm::dot(normal, light_dir));

            return (diffuse + specular) * pdf_inv * (float_t) M_1_PI * (1 - P);
        }

        return glm::vec3(0);
    }
};

struct PointLight : Light {
    const glm::vec3 pos;
    const float_t r;

    PointLight(
            const glm::vec3 &pos,
            const float_t &r,
            const glm::vec3 &intensity,
            const std::string &material_name
    ) : Light(intensity, material_name),
        pos(pos),
        r(r) {}

    glm::vec3 sample(
            const Mesh &mesh,
            const KDTree &tree,
            const glm::vec3 &hit_pos,
            const glm::vec3 &hit_normal,
            const glm::vec3 &ray_in_direction,
            const glm::vec3 &Kd,
            const glm::vec3 &Ks,
            const float_t &Ns,
            const float_t &P) override {

        float_t dist = glm::distance(pos, hit_pos);
        float_t cos_theta_max = glm::sqrt(1. - (r * r) / (dist * dist));
        float_t pdf_inv = 2. * M_PI * (1. - cos_theta_max);

        NormalCoord light_normal_coord(glm::normalize(pos - hit_pos));
        glm::vec3 light_dir = -glm::normalize(light_normal_coord.local_to_world(Random::sphere(r, dist)));

        Ray shadow_ray(hit_pos + EPSILON * -light_dir, -light_dir);
        IntersectInfo shadow_intersect_info{};

        bool shadow_intersect = tree.intersect(shadow_ray, shadow_intersect_info);

        if (mesh.materials[shadow_intersect_info.material_id].name == material_name) {
//            glm::vec3 diffuse = Kd * final_intensity * light_dist_2 * glm::max(glm::dot(-light_dir_normalized, intersect_info.normal), 0.f);
            glm::vec3 diffuse = Kd * intensity * glm::max(glm::dot(-light_dir, hit_normal), 0.f);
            // Phong
//            glm::vec3 specular = Ks * final_intensity * glm::pow(glm::max(glm::dot(-light_dir, Utils::reflect_direction(intersect_info.normal, ray.direction)), 0.f), Ns);
            // Phong-specular
            glm::vec3 specular = Ks * intensity * (Ns + 1.f) * 0.5f * (float_t) M_PI *
                                 glm::pow(glm::max(glm::dot(-light_dir, Utils::reflect_direction(hit_normal, ray_in_direction)), 0.f), Ns);
//            glm::vec3 specular = glm::vec3(0);

            return (diffuse + specular) * pdf_inv * (float_t) M_1_PI * (1 - P);
        }

        return glm::vec3(0);
    };

};

#endif //CG_RAY_TRACER_LIGHT_H
