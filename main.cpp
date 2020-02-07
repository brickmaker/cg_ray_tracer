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
    std::string infile = "models/plane-cylinder/plane-cylinder.obj";

    Mesh mesh(infile);
    Light light(glm::vec3(-1. / glm::sqrt(3.), -1. / glm::sqrt(3.), -1. / glm::sqrt(3.)));

//    buffer_t buffer = Utils::show_vertices(mesh, width, height);
    Renderer renderer(mesh, light, width, height);
    buffer_t buffer = renderer.render();

    Utils::save_img(buffer);

    return 0;
}