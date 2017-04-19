#pragma once

#include "stdafx.h"
#include "Component.h"

// std::numeric_limits<std::size_t>::max()
// #define MAZ_SZ (~(size_t)0)
// SIZE_MAX

// TODO: consider using bit fields to shrink the node array size
// TODO: the node can efficiently store position by using discretized number
//       where maximum x value represents eastern most node at bottom of tree
// TODO: decide if including path would be useful
// TODO: hsl should be set according to the side length of the Terrain object
struct QuadTreeNode {
	QuadTreeNode() : leaf(true), hsl(0.5f), x(0.0f), y(0.0f), mesh_index(SIZE_MAX) {}
	bool leaf;
    //size_t depth; // TODO: this may not be necessary
    float hsl; // TODO: this may not be necessary
    float x, y; // TODO: these should be combined into a uint16_t or something
    size_t mesh_index; // TODO: maybe remove
};

enum qt_action_type {SUBDIVIDE, MERGE};
enum corner_t {NW=0, NE=1, SW=2, SE=3};

struct qt_action {
    qt_action(size_t _idx, qt_action_type _action) : idx(_idx), action(_action) {}
    size_t idx;
    qt_action_type action;
};

// TODO: write code to generate default p and r values
// TODO: allow user to specify p and r values
class TerrainComp : public MeshComp {
public:
	TerrainComp(GameObject * _game_object, size_t depth, size_t vertsPerSide = 3, float sideLength = 1.0f) :
        MeshComp(_game_object),
	    treeDepth(depth),
	    vps(vertsPerSide),
	    rsl(sideLength)
	{
        numNodes = calcNumNodes(depth);
		nodes = new QuadTreeNode[numNodes];

        float default_rs[] = {3.0f, 1.0f, 0.5f, 0.25f};
        // TODO: user must define these
        for(size_t i=0; i<depth; i++)
		    rs.push_back(default_rs[i]);
        nodes[0].mesh_index = newGridMesh(vertsPerSide, position, sideLength);
        setNodePositions();
	}

    // TODO: Do I need to make the MeshComp's update() virtual? Do I need to make this one virtual?
    // TODO: currently tree actions and mesh enable/disable actions are executed in the same methods
    //       consider making a separate tree traversal for mesh enable/disable (cost: an extra traversal; benefit: more control)
    void update() {
		std::vector<qt_action> actions;
		getActions(actions);
		execActions(actions);
	}

	void addInducer(glm::vec3 const * inducer) {
	    inducers.push_back(inducer);
    }

private:

    // TODO: make sure that if the Transform component changes that this doesn't screw this up
    void setNodePositions(size_t nodeIdx = 0, size_t depth=0, float hsl = 0.5f, float x = 0.0f, float y = 0.0f) {
        float qsl = hsl/2.0f;
        nodes[nodeIdx].x = x;
        nodes[nodeIdx].y = y;
        //nodes[nodeIdx].depth = depth;
        nodes[nodeIdx].hsl = hsl;
        size_t child0 = firstChildIdx(nodeIdx);
        if(depth < treeDepth) {
            setNodePositions(child0+NW, depth+1, qsl, x-qsl, y+qsl);
            setNodePositions(child0+NE, depth+1, qsl, x+qsl, y+qsl);
            setNodePositions(child0+SW, depth+1, qsl, x-qsl, y-qsl);
            setNodePositions(child0+SE, depth+1, qsl, x+qsl, y-qsl);
        }
    }

    inline size_t calcNumNodes(size_t depth) { 
		size_t n = 1;
		size_t r = 1;
		for(size_t i=0; i < depth; i++) {
			r *= 4;
			n += r;
		}
        return n;
    }

    // TODO: getParent()

	inline size_t firstChildIdx(size_t nodeIdx) {
        return nodeIdx+nodeIdx*3+1;
    }

    // TODO: schedule freeing of mesh data
    void merge(size_t nodeIdx) {
        QuadTreeNode & node = nodes[nodeIdx];
        node.leaf = true;
        enableMesh(node.mesh_index);
        size_t child0 = firstChildIdx(nodeIdx);
        for(size_t i=0; i<4; i++) {
            QuadTreeNode & child = nodes[child0+i];
            disableMesh(child.mesh_index);
        }
    }

    void subdivide(size_t nodeIdx) {
        QuadTreeNode & node = nodes[nodeIdx];
        node.leaf = false;
        disableMesh(node.mesh_index);

        size_t child0 = firstChildIdx(nodeIdx);
        for(size_t i=0; i < 4; i++) {
            QuadTreeNode & child = nodes[child0+i];
            float childSide = node.hsl;
            float childHalfSide = node.hsl/2.0f;
            glm::vec3 childPos = glm::vec3(child.x, child.y, 0.0f);
            glm::vec2 uv_nw = glm::vec2(child.x - childHalfSide + 0.5f, child.y - childHalfSide + 0.5f);
            glm::vec2 uv_se = glm::vec2(childSide, childSide);
            if(child.mesh_index == SIZE_MAX)
                child.mesh_index = newGridMesh(vps, childPos, childSide, uv_nw, uv_se);
            else
                enableMesh(child.mesh_index);
            child.leaf = true;
        }
    }

    void execActions(std::vector<qt_action> & actions) {
        for(auto i = actions.begin(); i < actions.end(); i++) {
            if(i->action == MERGE)
                merge(i->idx);
            else if(i->action == SUBDIVIDE)
                subdivide(i->idx);
        }
    }

	void getActions(std::vector<qt_action> & actions, size_t nodeIdx=0, size_t depth=0) {
        QuadTreeNode & node = nodes[nodeIdx];
        glm::vec3 nodePos = glm::vec3(node.x, node.y, 0.0f);

        // TODO: consider using a faster distance function (e.g. bounding box i.e. 0 < x < 1 for x, y, and z)
        // TODO: position comparisons need to take the Transform component into account
	    if(node.leaf) { // is leaf; decide whether to subdivide
            for(auto i = inducers.begin(); i != inducers.end(); i++) {
                glm::vec3 const * inducer = *i;
                float d = distance(*inducer, nodePos);
	            if((depth < treeDepth) && (d < rs[depth])) {
                    actions.push_back(qt_action(nodeIdx, SUBDIVIDE));
                    break;
                }
            }
        }
        else { // has children; decide whether to merge. otherwise, descend
            bool do_merge = true; // assume we do the merge until we confirm that we shouldn't
            for(auto i = inducers.begin(); i != inducers.end(); i++) {
                glm::vec3 const * inducer = *i;
                float d = distance(*inducer, nodePos);
	            if(d < rs[depth]) {
                    do_merge = false;
                    break;
                }
            }
            if(do_merge)
                actions.push_back(qt_action(nodeIdx, MERGE));
            else {
                size_t child0 = firstChildIdx(nodeIdx);
                for(size_t i=0; i<4; i++)
                    getActions(actions, child0+i, depth+1);
            }
        }
    }

	size_t treeDepth;
	size_t numNodes;
	size_t vps; // vertices per side
	float rsl; // root node side length
    glm::vec3 position; // this should be coming from the transform...
    std::vector<glm::vec3 const *> inducers;
    std::vector<float> rs;
    std::vector<float> ps;
	QuadTreeNode * nodes;
};
