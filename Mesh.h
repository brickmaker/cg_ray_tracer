//
// Created by Xiaodong Zhao on 2020/2/6.
//

#ifndef CG_RAY_TRACER_MESH_H
#define CG_RAY_TRACER_MESH_H

#include <utility>
#include <iostream>

#include "libs/tiny_obj_loader.h"
#include "libs/glm/glm.hpp"
#include "libs/glm/ext.hpp"
#include "defs.h"

struct Mesh {
    std::string path;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    void load_obj(std::string &file);

    explicit Mesh(std::string path) : path(path) {
        load_obj(path);
    }
};


#endif //CG_RAY_TRACER_MESH_H
