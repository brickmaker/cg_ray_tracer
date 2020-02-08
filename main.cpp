#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#define GLM_ENABLE_EXPERIMENTAL

#include "Renderer.h"

#include "defs.h"
#include "Utils.h"
#include "Mesh.h"
#include "Light.h"


int main() {
    const int height = 100;
    const int width = 150;

//    std::string infile = "models/spot.obj";
//    std::string infile = "models/box/box.obj";
//    std::string infile = "models/sphere/sphere.obj";
//    std::string infile = "models/plane-cylinder/plane-cylinder.obj";
//    std::string infile = "models/cbox/cbox.obj";
    std::string infile = "models/veach_mis/mis.obj";

    Mesh mesh(infile);
//    Light light(glm::vec3(-1. / glm::sqrt(3.), -1. / glm::sqrt(3.), -1. / glm::sqrt(3.)));
//    Camera camera(glm::vec3(-0.5, 0.2, 2.), glm::vec3(0., 0., 0.), glm::vec3(0., 1., 0.), 60, glm::vec2(width, height));

    // cbox config
//    Light light(glm::vec3(0, 0, 1));
//    Camera camera(glm::vec3(278, 273, -800), glm::vec3(278, 273, -799), glm::vec3(0., 1., 0.), 39.3077, glm::vec2(width, height));

    // veach config
//    Light light(glm::vec3(-1, -1, -1));
    Light light(glm::vec3(-1. / glm::sqrt(3.), -1. / glm::sqrt(3.), -1. / glm::sqrt(3.)));
    Camera camera(glm::vec3(0, 2, 15), glm::vec3(0, -2, 2.5), glm::vec3(0., 1., 0.), 28, glm::vec2(width, height));

//    buffer_t buffer = Utils::show_vertices(mesh, width, height);
    Renderer renderer(mesh, light, camera, width, height);
    buffer_t buffer = renderer.render();

    Utils::save_img(buffer);

    return 0;
}