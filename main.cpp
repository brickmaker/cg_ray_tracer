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

//    std::string inputfile = "models/spot.obj";
//    std::string inputfile = "models/box/box.obj";
    std::string inputfile = "models/sphere/sphere.obj";

    Mesh mesh(inputfile);
    Light light(glm::vec3(0., 0., -1.));

//    buffer_t buffer = Utils::show_vertices(mesh, width, height);
    Renderer renderer(mesh, light, width, height);
    buffer_t buffer = renderer.render();

    Utils::save_img(buffer);

    return 0;
}