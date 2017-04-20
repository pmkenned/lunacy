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
//enum corner_t {NW=0, NE=1, SW=2, SE=3}; // TODO: this is redundantly defined in MeshComp.h

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
	    rsl(sideLength),
        texZoom(16.0f)
	{
        numNodes = calcNumNodes(depth);
		nodes = new QuadTreeNode[numNodes];

        ps.push_back(1.0f);

        float default_rs[] = {3.0f, 1.0f, 0.5f, 0.25f};
        // TODO: user must define these
        for(size_t i=0; i<depth; i++)
		    rs.push_back(default_rs[i]);
        nodes[0].mesh_index = newGridMesh(0, 0, position, sideLength, glm::vec2(0.0f, 0.0f), texZoom*glm::vec2(1.0f, 1.0f));
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

    inline size_t row_col_to_idx(size_t row, size_t col, size_t n) {
        return row*n+col;
    }

    inline size_t parent_corner(size_t vps, size_t idx, corner_t corner) {
        if(corner == SW || corner == SE)  idx += vps*(vps-1)/2;  // if a southern corner
        if(corner == NE || corner == SE)  idx +=     (vps-1)/2;  // if an eastern corner
        return idx;
    }

    inline size_t parent_corner(size_t vps, size_t row, size_t col, corner_t corner) {
        size_t idx = row_col_to_idx(row, col, vps);
        return parent_corner(vps, idx, corner);
    }

    void copy_parent_vertices(Vertex * child_vertices, Vertex * parent_vertices, corner_t corner, size_t vps) {
        for(size_t i = 0; i < vps/2+1; i++) {
            for(size_t j = 0; j < vps/2+1; j++) {
                size_t pc = parent_corner(vps, i, j, corner);
                size_t idx = i*2*vps+j*2; // TODO: use row_col_to_idx
                if(parent_vertices == 0)
                    child_vertices[idx].pos.z = 0.0f;
                else
                    child_vertices[idx].pos.z = parent_vertices[pc].pos.z;
            }
        }
    }

    // TODO: re-examine this
    inline float perturb(size_t depth, float x, float y, float s, size_t idx) {
        int n_idx_bits = 0;
        int vps2 = vps*vps;
        while(vps2 > 0) {
            vps2 = vps2 >> 1;
            n_idx_bits++;
        }
        unsigned int x8 = ((x+rsl/2)*64)/rsl;
        unsigned int y8 = ((y+rsl/2)*64)/rsl;
        unsigned int seed = (x8 << (n_idx_bits+8)) | (y8 << n_idx_bits) | (idx);
        std::mt19937 generator(seed); // TODO: consider making static or moving to tree_data
        std::uniform_int_distribution<> dist(0,0xfffffff);
        float r = ((float) dist(generator) / (0xfffffff)) - 0.5;
        //assert(depth < tree_data.p_values.size());
        float p = ps[depth];
        return r*s*p;
    }

    inline size_t get_center()           { return (vps+1)*(vps-1)/2;  }

    inline size_t get_north(size_t i)    { return 0 - (vps << i); }
    inline size_t get_south(size_t i)    { return 0 + (vps << i); }
    inline size_t get_west(size_t i)     { return 0 - (1 << i); }
    inline size_t get_east(size_t i)     { return 0 + (1 << i); }
    inline size_t get_nw_far(size_t i)   { return 0 - (vps << i) - (1 << i); }
    inline size_t get_ne_far(size_t i)   { return 0 - (vps << i) + (1 << i); }
    inline size_t get_sw_far(size_t i)   { return 0 + (vps << i) - (1 << i); }
    inline size_t get_se_far(size_t i)   { return 0 + (vps << i) + (1 << i); }
    inline size_t get_nw_close(size_t i) { return 0 - (vps << (i-1)) - (1 << (i-1)); }
    inline size_t get_ne_close(size_t i) { return 0 - (vps << (i-1)) + (1 << (i-1)); }
    inline size_t get_sw_close(size_t i) { return 0 + (vps << (i-1)) - (1 << (i-1)); }
    inline size_t get_se_close(size_t i) { return 0 + (vps << (i-1)) + (1 << (i-1)); }

    // TODO: make sure that all the randomness that can happen does happen
    void diamond_square(Vertex * vertices, size_t idx, size_t depth, size_t r, size_t center) {

        if(r==0)
            return;

        size_t i = r-1;
        Vertex * vs = vertices;

        // only perturb the newly created vertices (rest have been copied from parent)
        if(r == 1) {
            float center_z = (
                                vs[center + get_nw_far(i)].pos.z +
                                vs[center + get_ne_far(i)].pos.z +
                                vs[center + get_sw_far(i)].pos.z +
                                vs[center + get_se_far(i)].pos.z
                              ) * 0.25;

            float scale = vs[center].pos.x - vs[center + get_west(i)].pos.x; // measure the distance to western-neighbor
            float enable_perturb = 1.0; // make 0 to disable perturb

            float x = nodes[idx].x; // TODO
            float y = nodes[idx].y; // TODO
            
            vs[center].pos.z = center_z + enable_perturb*perturb(depth, x, y, scale, center);
        }

        vs[center + get_north(i)].pos.z = (vs[center + get_nw_far(i)].pos.z + vs[center + get_ne_far(i)].pos.z)*0.5;
        vs[center + get_south(i)].pos.z = (vs[center + get_sw_far(i)].pos.z + vs[center + get_se_far(i)].pos.z)*0.5;
        vs[center + get_east(i)].pos.z  = (vs[center + get_ne_far(i)].pos.z + vs[center + get_se_far(i)].pos.z)*0.5;
        vs[center + get_west(i)].pos.z  = (vs[center + get_nw_far(i)].pos.z + vs[center + get_sw_far(i)].pos.z)*0.5;

        // NOTE: "depth" should not increase in this recursion because we aren't descending in the tree, we are staying in the same node
        diamond_square(vertices, idx, depth, r-1, center + get_ne_close(i));
        diamond_square(vertices, idx, depth, r-1, center + get_nw_close(i));
        diamond_square(vertices, idx, depth, r-1, center + get_sw_close(i));
        diamond_square(vertices, idx, depth, r-1, center + get_se_close(i));
    }

    inline size_t vps_to_r() {
        size_t i = vps-1;
        size_t r = 0;
        while(i > 1) {
            i = (i >> 1);
            r++;
        }
        return r;
    }

    size_t newGridMesh(
        size_t            idx           = 0,
        size_t            depth         = 0,
        glm::vec3 const & position      = glm::vec3(0.0f),
        float             sl            = 1.0f,
        glm::vec2 const & uv_nw         = glm::vec2(0.0f),
        glm::vec2 const & uv_se         = glm::vec2(1.0f,1.0f),
        size_t            parentMeshIdx = SIZE_MAX,
        corner_t          corner        = NW)
    {
        MeshCompMesh * nm = new MeshCompMesh;
        nm->make_grid(vps, position, sl, uv_nw, uv_se);

        // TODO: confirm const correctness
        std::vector<MeshCompMesh *> const & meshes = getMeshes();
        
        if(parentMeshIdx != SIZE_MAX) // by default, we don't copy anything (root node)
            copy_parent_vertices(nm->getVertices(), meshes[parentMeshIdx]->getVertices(), corner, vps);
        
        size_t r = vps_to_r(); // TODO: shouldn't have to calculate this every time
        size_t center = get_center();
        diamond_square(nm->getVertices(), idx, depth, r, center);

        return addMesh(nm);
    }

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
        return nodeIdx*4+1;
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

    void subdivide(size_t nodeIdx, size_t depth = 0) { // TODO: see if depth can be eliminated
        QuadTreeNode & node = nodes[nodeIdx];
        node.leaf = false;
        disableMesh(node.mesh_index);

        size_t child0 = firstChildIdx(nodeIdx);
        for(size_t i=0; i < 4; i++) {
            QuadTreeNode & child = nodes[child0+i];
            float childSide = node.hsl;
            float childHalfSide = node.hsl/2.0f;
            glm::vec3 childPos = glm::vec3(child.x, child.y, 0.0f);
            glm::vec2 uv_nw = glm::vec2(child.x - childHalfSide + 0.5f, child.y - childHalfSide + 0.5f)*texZoom;
            glm::vec2 uv_se = glm::vec2(childSide, childSide)*texZoom;
            if(child.mesh_index == SIZE_MAX)
                child.mesh_index = newGridMesh(child0+i, depth, childPos, childSide, uv_nw, uv_se, node.mesh_index, (corner_t) i);
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
    float texZoom;
};
