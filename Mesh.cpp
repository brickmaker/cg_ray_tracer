//
// Created by Xiaodong Zhao on 2020/2/6.
//

#include "Mesh.h"

void Mesh::load_obj(std::string &file) {

    std::string warn;
    std::string err;

    std::cout << file << std::endl;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, file.c_str());
    if (!warn.empty()) {
        std::cout << warn << std::endl;
    }
    if (!err.empty()) {
        std::cerr << err << std::endl;
    }
    if (!ret) {
        exit(1);
    }
}
