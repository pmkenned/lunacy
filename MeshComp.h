#pragma once

#include "stdafx.h"

#include "Component.h"

class Vertex {
public:
    glm::vec4 pos;
    glm::vec3 norm;
    glm::vec2 texCoord;
    glm::vec4 color;
};

class Triangle {
public:
    Triangle() {}
    Triangle(size_t _v0, size_t _v1, size_t _v2) : v0(_v0), v1(_v1), v2(_v2) {}
    size_t v0, v1, v2;
};

struct MeshVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coord;
};

struct MeshTriangle {
    unsigned int vertices[3];
};

class Mesh
{
public:
    Mesh();
    virtual ~Mesh();

    typedef std::vector< MeshTriangle > MeshTriangleList;
    typedef std::vector< MeshVertex > MeshVertexList;

    MeshTriangleList triangles;
    MeshVertexList vertices;
    std::string filename;

    bool load();

private:

    // prevent copy/assignment
    Mesh( const Mesh& );
    Mesh& operator=( const Mesh& );
};

struct TriIndex
{
    int vertex;
    int normal;
    int tcoord;

    bool operator<( const TriIndex& rhs ) const {
        if ( vertex == rhs.vertex ) {
            if ( normal == rhs.normal ) {
                return tcoord < rhs.tcoord;
            } else {
                return normal < rhs.normal;
            }
        } else {
            return vertex < rhs.vertex;
        }
    }
};

struct Face
{
    TriIndex v[3];
};

enum ObjFormat
{
    VERTEX_ONLY = 1 << 0,
    VERTEX_UV = 1 << 1,
    VERTEX_NORMAL = 1 << 2,
    VERTEX_UV_NORMAL = 1 << 3
};

void calculate_normals(Vertex * vertices, size_t num_vertices, Triangle * triangles, size_t num_triangles);

class MeshCompMesh {
public:

    void load(std::string filename) {
        mesh.filename = filename;
        mesh.load();
    }

    void copy_from_mesh();

    // accessor methods
    size_t     numVertices()  { return num_vertices; }
    Vertex *   getVertices()  { return vertices; }
    size_t     numTriangles() { return num_triangles; }
    Triangle * getTriangles() { return triangles; }

    void make_cube(float xs = 1.0f, float ys = 1.0f, float zs = 1.0f, float xc = 0.0f, float yc = 0.0f, float zc = 0.0f );
    void make_pyramid(float xs = 1.0f, float ys = 1.0f, float zs = 1.0f, float xc = 0.0f, float yc = 0.0f, float zc = 0.0f );
    void make_square(float x, float y);
    void make_grid(size_t vps, glm::vec3 const & position); // TODO: reuse IBO somehow

private:
    Mesh mesh;
    size_t num_vertices;
    Vertex * vertices;
    size_t num_triangles;
    Triangle * triangles;
};

class MeshComp : public Component {
public:

    MeshComp(GameObject * _game_object);

    void update();

    void newGridMesh(size_t vps, glm::vec3 const & position = glm::vec3(0.0f)) {
        MeshCompMesh * nm = new MeshCompMesh;
        nm->make_grid(vps, position);
        meshes.push_back(nm);
    }

    void newSquareMesh() {
        MeshCompMesh * nm = new MeshCompMesh;
        nm->make_square(1.0f, 1.0);
        meshes.push_back(nm);
    }

    void newMeshFromFile(std::string filename) {
        MeshCompMesh * nm = new MeshCompMesh;
        nm->load(filename);
        nm->copy_from_mesh();
        meshes.push_back(nm);
    }

    std::vector<MeshCompMesh *> const & getMeshes() { return meshes; }

private:
    std::vector<MeshCompMesh *> meshes;
};
