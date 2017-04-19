#pragma once

#include "stdafx.h"
#include "GameObject.h"
#include "TransformComp.h"
#include "MeshComp.h"
#include "RenderComp.h"

// Design choices:
// * How nodes access tree data:
//     * The QuadTreeComp passes a pointer to a data structure to the node functions
//     * The QuadTreeNode contains a pointer to the owner's data structure
//     * Something else?
// * How nodes access/modify mesh data
//     * The QuadTreeNode casts the owner pointer to a MeshComp type

// TODO? struct qt_data

// TODO?
//enum corner_t {FIRST_CORNER=0, SW_CORNER=0, SE_CORNER, NE_CORNER, NW_CORNER, NUM_CORNERS, NO_CORNER};
//corner_t& operator++( corner_t &c );
//
////corner_t& operator++( corner_t &c ) {
////  c = static_cast<corner_t>( static_cast<int>(c) + 1 );
////  if ( c == corner_t::NO_CORNER )
////    c = corner_t::NO_CORNER;
////  return c;
////}

//enum action_type {SUBDIVIDE, MERGE};
//
//class QuadTreeNode;
//
//struct qt_action {
//    // constructor?
//    action_type type;
//    QuadTreeNode * const node;
//};
//
//// TODO? struct boundary_vert
//
//class QuadTreeComp;
//
//class QuadTreeNode {
//public:
//    QuadTreeNode(QuadTreeComp * _owner) : owner(_owner) {
//        MeshComp * x = (MeshComp *) owner;
//        x->newGridMesh(5, glm::vec3(0.0f));
//    }
//
//    void init_root() {
//        // make_vertices()
//        // subdivide()
//    }
//
//    void getActions() {}
//    void execActions() {}
//
//private:
//
//    float perturb() { return 0.0f; } // tree_data, s, idx
//
//    void subdivide() {} // tree_data
//    void merge() {} // tree_data
//
//    void free_children() {} // tree_data
//    void make_vertices() {} // tree_data, parent_vertices, corner
//    void calc_vertices() {} // tree_data, parent_vertices, corner
//    void diamond_square() {} // tree_data, r, idx
//    void copy_parent_vertices() {} // tree_data, parent_vertices, corner
//    void define_boundary_vertices() {} // tree_data?
//
//    // TODO: contribute and deduct norm functions
//
//    size_t vps_to_r() { return 0; } // vps
//    size_t parent_corner() { return 0; } // vps, row, col, corner
//
//    // TODO: get_center, get_north, etc.
//
//    // TODO: add boundary vertices
//    // TODO: should "corner" be a data member?
//    // TODO: add normal map?
//    QuadTreeComp * owner;
//    MeshCompMesh * mesh; // TODO: needed?
//    QuadTreeNode * children[4];
//    glm::vec3 position;
//    bool is_leaf;
//    size_t depth;
//};
//
//class QuadTreeComp : public MeshComp {
//public:
//    QuadTreeComp(GameObject * _game_object) : MeshComp(_game_object), root(this), verticesPerSide(5) {
//    }
//
//    void update() {
//        root.getActions();
//        root.execActions();
//    }
//
//    void add_p_values() {}
//    void add_r_values() {}
//    void add_inducer(glm::vec3 const * inducer) {} // TODO: how will this get called?
//
//private:
//    QuadTreeNode root;
//    // TODO: do these need to be put into a struct?
//    size_t verticesPerSide;
//    float side_length;
//    //NOTE: position and orientation are in the Transform component. Should I have a pointer?
//    std::vector<glm::vec3 const * const> inducers;
//    std::vector<float> r_values;
//    std::vector<float> p_values;
//};
//
//class QuadTree : public GameObject {
//public:
//    QuadTree(GLuint _program) : GameObject(_program) {
//        addComponent(new TransformComp(this));
//        addComponent(new QuadTreeComp(this));
//        addComponent(new RenderComp(this)); // the render component gets the QuadTreeComp, which is a type of MeshComp
//    }
//private:
//};