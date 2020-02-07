#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#define GLM_ENABLE_EXPERIMENTAL

#include "Renderer.h"

#include "defs.h"
#include "Utils.h"
#include "Mesh.h"


int main() {
    const int height = 200;
    const int width = 300;

    std::string inputfile = "models/spot.obj";
//    std::string inputfile = "models/box/box.obj";
//    std::string inputfile = "models/sphere/sphere.obj";

    Mesh mesh(inputfile);

//    buffer_t buffer = Utils::show_vertices(mesh, width, height);
    Renderer renderer(mesh, width, height);
    buffer_t buffer = renderer.render();

    Utils::save_img(buffer);

    return 0;
}