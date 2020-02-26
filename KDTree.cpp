//
// Created by Xiaodong Zhao on 2020/2/12.
//

#include "KDTree.h"


static BBox operator+(BBox &a, BBox &b) {
    return {
            glm::min(a.min_p, b.min_p),
            glm::max(a.max_p, b.max_p),
    };
}

static bool intersect_ray_box(Ray &ray, BBox &box) { // TODO: consider origin in box??
    float_t tmin, tmax, tymin, tymax, tzmin, tzmax;

    float_t invdirx = 1. / ray.direction.x,
            invdiry = 1. / ray.direction.y,
            invdirz = 1. / ray.direction.z;

    glm::vec3 origin = ray.origin;

    if (invdirx >= 0) {

        tmin = (box.min_p.x - origin.x) * invdirx;
        tmax = (box.max_p.x - origin.x) * invdirx;

    } else {

        tmin = (box.max_p.x - origin.x) * invdirx;
        tmax = (box.min_p.x - origin.x) * invdirx;

    }

    if (invdiry >= 0) {

        tymin = (box.min_p.y - origin.y) * invdiry;
        tymax = (box.max_p.y - origin.y) * invdiry;

    } else {

        tymin = (box.max_p.y - origin.y) * invdiry;
        tymax = (box.min_p.y - origin.y) * invdiry;

    }

    if ((tmin > tymax) || (tymin > tmax)) return false;

// These lines also handle the case where tmin or tmax is NaN
// (result of 0 * Infinity). x !== x returns true if x is NaN

    if (tymin > tmin || tmin != tmin) tmin = tymin;

    if (tymax < tmax || tmax != tmax) tmax = tymax;

    if (invdirz >= 0) {

        tzmin = (box.min_p.z - origin.z) * invdirz;
        tzmax = (box.max_p.z - origin.z) * invdirz;

    } else {

        tzmin = (box.max_p.z - origin.z) * invdirz;
        tzmax = (box.min_p.z - origin.z) * invdirz;

    }

    if ((tmin > tzmax) || (tzmin > tmax)) return false;

    if (tzmin > tmin || tmin != tmin) tmin = tzmin;

    if (tzmax < tmax || tmax != tmax) tmax = tzmax;

    //return point closest to the ray (positive side)

    if (tmax < 0) return false;

//    return ray.origin + ray.direction * (tmin >= 0 ? tmin : tmax);

    return true;
}

BBox &BBox::operator+=(const BBox &a) {
    this->min_p = glm::min(this->min_p, a.min_p);
    this->max_p = glm::max(this->max_p, a.max_p);
    return *this;
}

KDTree::KDTree(Mesh &mesh) : mesh(mesh) {
    shape_num = mesh.shapes.size();
    store_items();

    for (int i = 0; i < shape_num; i++) {
        int l = 0;
        int r = items[i].size();
        trees.push_back(build_tree(i, l, r, 0));
    }
}

void KDTree::store_items() {
    for (size_t shape_i = 0; shape_i < mesh.shapes.size(); shape_i++) {
        vector<TriangleItem *> shape_items;
        size_t offset = 0;
        for (size_t face_i = 0; face_i < mesh.shapes[shape_i].mesh.num_face_vertices.size(); face_i++) {
            // TODO: only support triangle
            size_t face_vertex_num = 3;
//                size_t face_vertex_num = mesh.shapes[shape_i].mesh.num_face_vertices[face_i];
            std::vector<glm::vec3> vertices(face_vertex_num);
            std::vector<glm::vec3> normals(face_vertex_num);
            for (size_t v_i = 0; v_i < face_vertex_num; v_i++) {
                tinyobj::index_t indices = mesh.shapes[shape_i].mesh.indices[v_i + offset];
                float_t vx = mesh.attrib.vertices[3 * indices.vertex_index + 0];
                float_t vy = mesh.attrib.vertices[3 * indices.vertex_index + 1];
                float_t vz = mesh.attrib.vertices[3 * indices.vertex_index + 2];
                vertices[v_i] = glm::vec3(vx, vy, vz);

                float_t nx = mesh.attrib.normals[3 * indices.normal_index + 0];
                float_t ny = mesh.attrib.normals[3 * indices.normal_index + 1];
                float_t nz = mesh.attrib.normals[3 * indices.normal_index + 2];
                normals[v_i] = glm::vec3(nx, ny, nz);
            }
            offset += face_vertex_num;

            glm::vec3 min_p = glm::min(vertices[0], vertices[1], vertices[2]);
            glm::vec3 max_p = glm::max(vertices[0], vertices[1], vertices[2]);
            shape_items.push_back(new TriangleItem{face_i, BBox{min_p, max_p}, vertices, normals});
        }
        items.push_back(shape_items);
    }
}

TreeNode *KDTree::build_tree(int shape_i, int l, int r, int axis) {
    if (r - l <= LEAF_TRIANGLE_CNT) {
        // leaf node
        auto *node = new TreeNode();
        node->left = nullptr;
        node->right = nullptr;
        node->triangles = vector<TriangleItem *>(items[shape_i].begin() + l, items[shape_i].begin() + r);
        node->b_box = (*(items[shape_i].begin() + l))->b_box;
        for (auto it = items[shape_i].begin() + l + 1; it < items[shape_i].begin() + r; it++) {
            node->b_box += (*it)->b_box;
        }
        return node;
    }

    int mid = (l + r) / 2;
    std::nth_element(items[shape_i].begin() + l, items[shape_i].begin() + mid, items[shape_i].begin() + r, comparator(axis));

    TreeNode *root = new TreeNode();
    root->left = build_tree(shape_i, l, mid, (axis + 1) % 3);
    root->right = build_tree(shape_i, mid, r, (axis + 1) % 3);
    root->b_box = root->left->b_box + root->right->b_box;

    return root;
}

TriangleItem *KDTree::search(Ray &ray, TreeNode *root, float_t &dist, glm::vec2 &bary_pos) const {
    if (!root) {
        return nullptr;
    }
    TriangleItem *res = nullptr;
    glm::vec2 curr_bary_pos;
    float_t curr_dist;

    if (!intersect_ray_box(ray, root->b_box)) {
        return nullptr;
    }

    if (!root->left && !root->right) {
        // leaf node
        for (TriangleItem *item : root->triangles) {
            bool is_intersect = glm::intersectRayTriangle(
                    ray.origin,
                    ray.direction,
                    item->vertices[0],
                    item->vertices[1],
                    item->vertices[2],
                    curr_bary_pos,
                    curr_dist
            );
            if (is_intersect && curr_dist > 0 && curr_dist < dist) {
                dist = curr_dist;
                bary_pos = curr_bary_pos;
                res = item;
            }
        }
    }

    curr_dist = dist;
    TriangleItem *l_item = search(ray, root->left, curr_dist, curr_bary_pos);
    if (l_item && curr_dist > 0 && curr_dist < dist) {
        dist = curr_dist;
        bary_pos = curr_bary_pos;
        res = l_item;
    }

    curr_dist = dist;
    TriangleItem *r_item = search(ray, root->right, curr_dist, curr_bary_pos);
    if (r_item && curr_dist > 0 && curr_dist < dist) {
        dist = curr_dist;
        bary_pos = curr_bary_pos;
        res = r_item;
    }

    return res;
}

bool KDTree::intersect(Ray &ray, IntersectInfo &intersect_info) const {
//        return intersect_naive(ray, intersect_info);
    float_t dist = FLOAT_INF;
    glm::vec2 bary_pos;
    TriangleItem *intersect_item = nullptr;
    for (int i = 0; i < shape_num; i++) {
        TriangleItem *item = search(ray, trees[i], dist, bary_pos);
        if (item) {
            // TODO: not elegant, maybe need top level hierarchy
            intersect_item = item;
            intersect_info.shape_i = i;
        }
    }
    if (intersect_item) {
        intersect_info.face_i = intersect_item->face_i;
        intersect_info.material_id = mesh.shapes[intersect_info.shape_i].mesh.material_ids[intersect_info.face_i];
        intersect_info.pos = ray.origin + dist * ray.direction;
        intersect_info.normal = (1 - bary_pos.x - bary_pos.y) * intersect_item->normals[0] + bary_pos.x * intersect_item->normals[1] + bary_pos.y * intersect_item->normals[2];
        return true;
    }
    return false;
}

bool KDTree::intersect_naive(Ray &ray, IntersectInfo &intersect_info) {

    bool flag_intersect = false;
    float_t near_dist = FLOAT_INF; // record nearest distance


    for (size_t shape_i = 0; shape_i < mesh.shapes.size(); shape_i++) {
        size_t offset = 0;
        for (size_t face_i = 0; face_i < mesh.shapes[shape_i].mesh.num_face_vertices.size(); face_i++) {
            // TODO: only support triangle
            size_t face_vertex_num = 3;
//                size_t face_vertex_num = mesh.shapes[shape_i].mesh.num_face_vertices[face_i];
            std::vector<glm::vec3> vertices(face_vertex_num);
            std::vector<glm::vec3> normals(face_vertex_num);
            for (size_t v_i = 0; v_i < face_vertex_num; v_i++) {
                tinyobj::index_t indices = mesh.shapes[shape_i].mesh.indices[v_i + offset];
                float_t vx = mesh.attrib.vertices[3 * indices.vertex_index + 0];
                float_t vy = mesh.attrib.vertices[3 * indices.vertex_index + 1];
                float_t vz = mesh.attrib.vertices[3 * indices.vertex_index + 2];
                vertices[v_i] = glm::vec3(vx, vy, vz);

                float_t nx = mesh.attrib.normals[3 * indices.normal_index + 0];
                float_t ny = mesh.attrib.normals[3 * indices.normal_index + 1];
                float_t nz = mesh.attrib.normals[3 * indices.normal_index + 2];
                normals[v_i] = glm::vec3(nx, ny, nz);
            }
            offset += face_vertex_num;

            glm::vec2 bary_pos;
            float_t distance;
            bool is_intersect = glm::intersectRayTriangle(
                    ray.origin,
                    ray.direction,
                    vertices[0],
                    vertices[1],
                    vertices[2],
                    bary_pos,
                    distance
            );

//            glm::vec3 normal = glm::cross(vertices[1] - vertices[0], vertices[2] - vertices[0]);
//            normal = glm::normalize(normal);

            if (is_intersect && distance > 0 && distance < near_dist) {
                near_dist = distance;
                flag_intersect = true;
                intersect_info.shape_i = shape_i;
                intersect_info.face_i = face_i;
                intersect_info.material_id = mesh.shapes[shape_i].mesh.material_ids[face_i];
                intersect_info.pos = ray.origin + distance * ray.direction;
                intersect_info.normal = (1 - bary_pos.x - bary_pos.y) * normals[0] + bary_pos.x * normals[1] + bary_pos.y * normals[2];
//                intersect_info.normal = normal;
            }
        }
    }

    return flag_intersect;
}
