#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#define GLM_ENABLE_EXPERIMENTAL

#include "Renderer.h"

#include "defs.h"
#include "Utils.h"
#include "Mesh.h"
#include "Light.h"
#include "KDTree.h"


int main() {
//    const int height = 720;
//    const int width = 1280;

//    std::string infile = "models/spot.obj";
//    std::string infile = "models/box/box.obj";
//    std::string infile = "models/sphere/sphere.obj";
//    std::string infile = "models/plane-cylinder/plane-cylinder.obj";
//    std::string infile = "models/cbox/cbox.obj";
//    std::string infile = "models/diningroom/diningroom.obj";
    std::string infile = "models/veach_mis/mis.obj";
//    std::string infile = "models/multi-balls/multi-balls.obj";
//    std::string infile = "models/refraction/refraction.obj";

    // refraction config
//    Light light(glm::vec3(-1, -1, -1));
//    Camera camera(glm::vec3(0., 0., 3.), glm::vec3(0., 0., 0.), glm::vec3(0., 1., 0.), 60, glm::vec2(width, height));

    // multi-ball config
//    Light light(glm::vec3(-1, -1, -1));
//    Camera camera(glm::vec3(0.5, 0., 2.5), glm::vec3(0., 0., 0.), glm::vec3(0., 1., 0.), 60, glm::vec2(width, height));

    // cbox config
//    Light light(glm::vec3(0, 0, 1));
//    Camera camera(glm::vec3(278, 273, -800), glm::vec3(278, 273, -799), glm::vec3(0., 1., 0.), 39.3077, glm::vec2(width, height));

    // dinning room config
//    const int width = 1280;
//    const int height = 720;
//    Light light(glm::vec3(0, 0, -1));
//    Camera camera(glm::vec3(-0.5, 3, 5.5), glm::vec3(-0.5, 2, 0), glm::vec3(0., 1., 0.), 60, glm::vec2(width, height));

    // veach config
    const int width = 768;
    const int height = 512;
    Light light(glm::vec3(-1, -1, -3));
    vector<PointLight> point_lights{
            PointLight{glm::vec3(-3.75, 0, 0), 0.03, 901.8, "mi:light1"},
            PointLight{glm::vec3(-1.25, 0, 0), 0.1, 100, "mi:light2"},
            PointLight{glm::vec3(1.25, 0, 0), 0.3, 11.11, "mi:light3"},
            PointLight{glm::vec3(3.75, 0, 0), 0.9, 1.24, "mi:light4"},
            PointLight{glm::vec3(10, 10, 4), 0.5, 800, "mi:light5"},
    };
    Camera camera(glm::vec3(0, 2, 15), glm::vec3(0, -2, 2.5), glm::vec3(0., 1., 0.), 28, glm::vec2(width, height));

    Mesh mesh(infile);
    KDTree tree(mesh);
//    buffer_t buffer = Utils::show_vertices(mesh, width, height);
    Renderer renderer(mesh, tree, point_lights, camera, width, height);
    buffer_t buffer = renderer.render();

    Utils::save_img(buffer);

    return 0;
}