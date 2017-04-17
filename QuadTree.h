#pragma once

#include "stdafx.h"
#include "GameObject.h"
#include "TransformComp.h"
#include "MeshComp.h"
#include "RenderComp.h"

class QuadTreeComp;

class QuadTreeNode {
public:
    QuadTreeNode(QuadTreeComp * _owner) : owner(_owner) {
        MeshComp * x = (MeshComp *) owner;
        x->newGridMesh(5, glm::vec3(0.0f));
    }

    void getActions() {}
    void execActions() {}
private:
    QuadTreeComp * owner;
    // TODO: mesh pointer

};

class QuadTreeComp : public MeshComp {
public:
    QuadTreeComp(GameObject * _game_object) : MeshComp(_game_object), root(this), verticesPerSide(5) {
        //newGridMesh(5, glm::vec3(0.0f, 0.0f, 0.0f));
        //newGridMesh(5, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    void update() {
        root.getActions();
        root.execActions();
    }

private:
    QuadTreeNode root;
    size_t verticesPerSide;
};

class QuadTree : public GameObject {
public:
    QuadTree(GLuint _program) : GameObject(_program) {
        addComponent(new TransformComp(this));
        addComponent(new QuadTreeComp(this));
        addComponent(new RenderComp(this)); // the render component gets the QuadTreeComp, which is a type of MeshComp
    }
private:
};

//class QuadTree;
//
//enum corner_t {FIRST_CORNER=0, SW_CORNER=0, SE_CORNER, NE_CORNER, NW_CORNER, NUM_CORNERS, NO_CORNER};
//corner_t& operator++( corner_t &c );
//
////corner_t& operator++( corner_t &c ) {
////  c = static_cast<corner_t>( static_cast<int>(c) + 1 );
////  if ( c == corner_t::NO_CORNER )
////    c = corner_t::NO_CORNER;
////  return c;
////}
//
//class QuadTreeNode : public GameObject {
//public:
//
//    QuadTreeNode(GLuint _program, QuadTree * _owner) : GameObject(_program), owner(_owner), is_leaf(true) {
//
//        addComponent(new TransformComp(this)); // TODO: inherit position from parent
//
//        MeshComp * mc = new MeshComp(this);
//        addComponent(mc);
//        mc->make_grid(5);
//
//        RenderComp * rc = new RenderComp(this, true);
//        addComponent(rc);
//    }
//
//private:
//    QuadTree * owner;
//    bool is_leaf;
//    QuadTreeNode * children[4];
//    size_t depth;
//};
//
//class QuadTree : public GameObject {
//public:
//
//    QuadTree(GLuint _program) : GameObject(_program), root(_program, this), verticesPerSide(5) {
//        addComponent(new TransformComp(this));
//
//        addChild(&root);
//
//        // TODO: initialize the root node
//    }
//
//    int vps() { return verticesPerSide; }
//
//private:
//
//    void getActions();
//
//    void execActions();
//
//
//    QuadTreeNode root;
//
//    int verticesPerSide;
//};