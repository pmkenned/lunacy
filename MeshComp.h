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

    MeshCompMesh() : render_enabled(true) {}

    void load(std::string filename) {
        mesh.filename = filename;
        mesh.load();
    }

    void copy_from_mesh();

    // accessor methods
    size_t      numVertices()   { return num_vertices; }
    Vertex *    getVertices()   { return vertices; }     // TODO: oh god, I return pointers to private data members?! D:
    size_t      numTriangles()  { return num_triangles; }
    Triangle *  getTriangles()  { return triangles; }

    GLuint      getVAO()        { return VAO; }
    bool        getEnabled()    { return render_enabled; }

    void        enableRender()  { render_enabled = true; }
    void        disableRender() { render_enabled = false; }

    void make_cube(float xs = 1.0f, float ys = 1.0f, float zs = 1.0f, float xc = 0.0f, float yc = 0.0f, float zc = 0.0f );
    void make_pyramid(float xs = 1.0f, float ys = 1.0f, float zs = 1.0f, float xc = 0.0f, float yc = 0.0f, float zc = 0.0f );
    void make_square(float x, float y);
    void make_grid(size_t vps, glm::vec3 const & position, float sl, glm::vec2 const & uv_nw = glm::vec2(0.0f), glm::vec2 const & uv_se = glm::vec2(1.0f, 1.0f)); // TODO: reuse IBO somehow


    // TODO: make private
    GLuint VAO;
    GLuint VBO, IBO;

private:
    Mesh mesh;
    bool render_enabled;

    size_t num_vertices;
    Vertex * vertices;
    size_t num_triangles;
    Triangle * triangles;
};

class MeshComp : public Component {
public:

    MeshComp(GameObject * _game_object);

    void update();

    void disableMesh(size_t index) {
        meshes[index]->disableRender();
    }

    void enableMesh(size_t index) {
        meshes[index]->enableRender();
    }

    // TODO: write toggleMesh function

    size_t addMesh(MeshCompMesh * nm) {
        size_t num_vertices = nm->numVertices();
        Vertex * vertices = nm->getVertices();
        size_t num_triangles = nm->numTriangles();
        Triangle * triangles = nm->getTriangles();

        glGenBuffers(1, &nm->VBO);
        glGenBuffers(1, &nm->IBO);

        glGenVertexArrays(1, &nm->VAO);
        glBindVertexArray(nm->VAO);

            // Vertex buffer
            glBindBuffer(GL_ARRAY_BUFFER, nm->VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * num_vertices, vertices, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0); // Position ; Matches layout (location = 0)
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

            glEnableVertexAttribArray(1); // Normal   ; Matches layout (location = 1)
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)sizeof(glm::vec4));

            glEnableVertexAttribArray(2); // TexCoord ; Matches layout (location = 2)
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(glm::vec4) + sizeof(glm::vec3)));

            glEnableVertexAttribArray(3); // Colors   ; Matches layout (location = 3)
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(glm::vec4) + sizeof(glm::vec3) + sizeof(glm::vec2)));

            // Index buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, nm->IBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Triangle) * num_triangles, triangles, GL_STATIC_DRAW);

        glBindVertexArray(0);

        meshes.push_back(nm);

        return meshes.size()-1;
    }

    void newSquareMesh() {
        MeshCompMesh * nm = new MeshCompMesh;
        nm->make_square(1.0f, 1.0);
        // TODO: this needs to generate the buffers, etc.
        meshes.push_back(nm);
    }

    void newMeshFromFile(std::string filename) {
        MeshCompMesh * nm = new MeshCompMesh;
        nm->load(filename);
        nm->copy_from_mesh();
        // TODO: this needs to generate the buffers, etc.
        meshes.push_back(nm);
    }

    std::vector<MeshCompMesh *> const & getMeshes() { return meshes; }

private:
    std::vector<MeshCompMesh *> meshes;
};