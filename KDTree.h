//
// Created by Xiaodong Zhao on 2020/2/12.
//

#ifndef CG_RAY_TRACER_KDTREE_H
#define CG_RAY_TRACER_KDTREE_H

#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include "defs.h"
#include "Mesh.h"
#include "libs/glm/ext.hpp"

const int LEAF_TRIANGLE_CNT = 5;

using std::vector;
using std::cout;
using std::endl;
using std::string;

struct BBox {
    glm::vec3 min_p;
    glm::vec3 max_p;

    BBox &operator+=(const BBox &a);
};

struct TriangleItem {
    size_t face_i;
    BBox b_box;
    vector<glm::vec3> vertices;
    vector<glm::vec3> normals;
};

struct comparator {
    int axis;

    explicit comparator(int axis) : axis(axis) {}

    bool operator()(TriangleItem *a, TriangleItem *b) {
        // use center point compare
        return (a->b_box.min_p[axis] + a->b_box.max_p[axis]) <
               (b->b_box.min_p[axis] + b->b_box.max_p[axis]);
    }
};

struct TreeNode {
    BBox b_box;
    TreeNode *left;
    TreeNode *right;
    vector<TriangleItem *> triangles;
};

struct KDTree {
    Mesh &mesh;
    int shape_num;
    vector<vector<TriangleItem *>> items;
    vector<TreeNode *> trees;

    KDTree(Mesh &mesh);

    void store_items();

    TreeNode *build_tree(int shape_i, int l, int r, int axis);

    TriangleItem *search(Ray &ray, TreeNode *root, float_t &dist, glm::vec2 &bary_pos);

    bool intersect(Ray &ray, IntersectInfo &intersect_info);

    bool intersect_naive(Ray &ray, IntersectInfo &intersect_info);
};


#endif //CG_RAY_TRACER_KDTREE_H
