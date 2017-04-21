#include "stdafx.h"

#include "MeshComp.h"

void MeshCompMesh::make_cube(float xs, float ys, float zs, float xc, float yc, float zc) {
    num_vertices    = 8;
    vertices        = new Vertex[num_vertices];
    num_triangles   = 12;
    triangles       = new Triangle[num_triangles];

    glm::vec4 center(xc, yc, zc, 0.0f);

    vertices[0].pos = glm::vec4(xs*(0.0f - 0.5f), ys*(0.0f - 0.5f), zs*(0.0f - 0.5f), 1.0f) + center;
    vertices[1].pos = glm::vec4(xs*(1.0f - 0.5f), ys*(0.0f - 0.5f), zs*(0.0f - 0.5f), 1.0f) + center;
    vertices[2].pos = glm::vec4(xs*(1.0f - 0.5f), ys*(0.0f - 0.5f), zs*(1.0f - 0.5f), 1.0f) + center;
    vertices[3].pos = glm::vec4(xs*(0.0f - 0.5f), ys*(0.0f - 0.5f), zs*(1.0f - 0.5f), 1.0f) + center;
    vertices[4].pos = glm::vec4(xs*(0.0f - 0.5f), ys*(1.0f - 0.5f), zs*(0.0f - 0.5f), 1.0f) + center;
    vertices[5].pos = glm::vec4(xs*(1.0f - 0.5f), ys*(1.0f - 0.5f), zs*(0.0f - 0.5f), 1.0f) + center;
    vertices[6].pos = glm::vec4(xs*(1.0f - 0.5f), ys*(1.0f - 0.5f), zs*(1.0f - 0.5f), 1.0f) + center;
    vertices[7].pos = glm::vec4(xs*(0.0f - 0.5f), ys*(1.0f - 0.5f), zs*(1.0f - 0.5f), 1.0f) + center;

    triangles[0]  = Triangle(0,1,3);
    triangles[1]  = Triangle(1,2,3);
    triangles[2]  = Triangle(0,4,1);
    triangles[3]  = Triangle(1,4,5);
    triangles[4]  = Triangle(0,7,4);
    triangles[5]  = Triangle(0,3,7);
    triangles[6]  = Triangle(1,5,6);
    triangles[7]  = Triangle(1,6,2);
    triangles[8]  = Triangle(2,6,7);
    triangles[9]  = Triangle(2,7,3);
    triangles[10] = Triangle(4,7,5);
    triangles[11] = Triangle(5,7,6);

    // make red
    for(size_t i = 0; i < num_vertices; i++) {
        vertices[i].color.x = 1.0f;
        vertices[i].color.y = 0.0f;
        vertices[i].color.z = 0.0f;
        vertices[i].color.w = 1.0f;
    }

    calculate_normals(vertices, num_vertices, triangles, num_triangles);
}

void MeshCompMesh::make_pyramid(float xs, float ys, float zs, float xc, float yc, float zc) {

    num_triangles   = 6;
    triangles       = new Triangle[num_triangles];
    num_vertices    = 5;
    vertices        = new Vertex[num_vertices];

    glm::vec4 COM(0.5f*xs + xc, 0.177f*ys + yc, 0.5f*zs + zc, 0.0f);

    vertices[0].pos = glm::vec4(0.0f*xs, 0.0f*ys, 1.0f*zs, 1.0f) - COM;
    vertices[1].pos = glm::vec4(1.0f*xs, 0.0f*ys, 1.0f*zs, 1.0f) - COM;
    vertices[2].pos = glm::vec4(1.0f*xs, 0.0f*ys, 0.0f*zs, 1.0f) - COM;
    vertices[3].pos = glm::vec4(0.0f*xs, 0.0f*ys, 0.0f*zs, 1.0f) - COM;
    vertices[4].pos = glm::vec4(0.5f*xs, 0.707f*ys, 0.5f*zs, 1.0f) - COM;

    triangles[0] = Triangle(0,3,1);
    triangles[1] = Triangle(1,3,2);
    triangles[2] = Triangle(0,1,4);
    triangles[3] = Triangle(1,2,4);
    triangles[4] = Triangle(2,3,4);
    triangles[5] = Triangle(3,0,4);

    // make red
    for(size_t i = 0; i < num_vertices; i++) {
        vertices[i].color.x = 1.0f;
        vertices[i].color.y = 0.0f;
        vertices[i].color.z = 0.0f;
        vertices[i].color.w = 1.0f;
    }

    calculate_normals(vertices, num_vertices, triangles, num_triangles);
}

void MeshCompMesh::make_square(float x, float y) {

    num_triangles   = 2;
    triangles       = new Triangle[num_triangles];
    num_vertices    = 4;
    vertices        = new Vertex[num_vertices];

    vertices[0].pos = glm::vec4(-0.5f*x, -0.5f*y, 0.0f, 1.0f);
    vertices[1].pos = glm::vec4(-0.5f*x,  0.5f*y, 0.0f, 1.0f);
    vertices[2].pos = glm::vec4( 0.5f*x, -0.5f*y, 0.0f, 1.0f);
    vertices[3].pos = glm::vec4( 0.5f*x,  0.5f*y, 0.0f, 1.0f);

    for(size_t i=0; i<4; i++)
        vertices[i].color = glm::vec4( 1.0f,  0.0f, 0.0f, 1.0f);

    vertices[0].texCoord = glm::vec2(0.0f, 0.0f)*2.0f;
    vertices[1].texCoord = glm::vec2(0.0f, 1.0f)*2.0f;
    vertices[2].texCoord = glm::vec2(1.0f, 0.0f)*2.0f;
    vertices[3].texCoord = glm::vec2(1.0f, 1.0f)*2.0f;

    triangles[0] = Triangle(0,2,1);
    triangles[1] = Triangle(1,2,3);

    calculate_normals(vertices, num_vertices, triangles, num_triangles);
}

// TODO: consider defining normals here (as 0.0, 0.0, 1.0) to skip calculate_normals step in make_grid
Vertex * grid_of_vertices(size_t n, glm::vec3 const & center, float side_length, glm::vec2 const & uv_nw = glm::vec2(0.0f), glm::vec2 const & uv_se = glm::vec2(1.0f, 1.0f)) {
    float nw_x = uv_nw.x;
    float nw_y = uv_nw.y;
    float se_x = uv_se.x;
    float se_y = uv_se.y;
    Vertex * vertices = new Vertex[n*n];
    for(size_t i=0; i<n; i++) { // i varies across the y-axis (each i value is a row)
        for(size_t j=0; j<n; j++) { // j varies across x-axis (each j value is a column in row i)
            vertices[i*n+j].pos = glm::vec4(center + glm::vec3((1.0*j)/(n-1) - 0.5, (-1.0*i)/(n-1) + 0.5, 0.0) * side_length, 1.0f);
            vertices[i*n+j].color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f); // gray
            vertices[i*n+j].texCoord = glm::vec2(se_x*j/(n-1)+nw_x, se_y*(n-i-1)/(n-1)+nw_y);
        }
    }
    return vertices;
}

Triangle * grid_of_triangles(size_t n) {
    size_t num_tri = (n - 1)*(n - 1)*2;
    Triangle * triangles = new Triangle[num_tri];
    for(size_t i=0; i < n-1; i++) {
        for(size_t j=0; j < n-1; j++) {
            size_t idx = i*2*(n-1) + 2*j;
            triangles[idx]   = Triangle(i*n+j,   n*(i+1)+j, i*n+j+1);
            triangles[idx+1] = Triangle(i*n+j+1, n*(i+1)+j, n*(i+1)+j+1);
        }
    }
    return triangles;
}

void MeshCompMesh::make_grid(size_t vps, glm::vec3 const & position, float sl, glm::vec2 const & uv_nw, glm::vec2 const & uv_se) {

    vertices = grid_of_vertices(vps, position, sl, uv_nw, uv_se); // sets position and color
    num_vertices = vps*vps;

    triangles = grid_of_triangles(vps);
    num_triangles = 2*(vps-1)*(vps-1);

    calculate_normals(vertices, num_vertices, triangles, num_triangles); // TODO: consider removing this
}

MeshComp::MeshComp(GameObject * _game_object) : Component(_game_object) {
}

void MeshCompMesh::copy_from_mesh() {
    // allocate
    num_vertices = mesh.vertices.size();
    vertices = new Vertex[num_vertices];
    num_triangles = mesh.triangles.size();
    triangles = new Triangle[num_triangles];

    // copy
    for(size_t i = 0; i < num_vertices; i++) {
        vertices[i].pos.x = mesh.vertices[i].position.x;
        vertices[i].pos.y = mesh.vertices[i].position.y;
        vertices[i].pos.z = mesh.vertices[i].position.z;
        vertices[i].pos.w = 1.0f;
        // white color
        vertices[i].color.x = 1.0f;
        vertices[i].color.y = 0.0f;
        vertices[i].color.z = 0.0f;
        vertices[i].color.w = 1.0f;
        vertices[i].texCoord = mesh.vertices[i].tex_coord;
    }

    for(size_t i = 0; i < num_triangles; i++) {
        triangles[i].v0 = mesh.triangles[i].vertices[0];
        triangles[i].v1 = mesh.triangles[i].vertices[1];
        triangles[i].v2 = mesh.triangles[i].vertices[2];
    }

    calculate_normals(vertices, num_vertices, triangles, num_triangles);
}

void MeshComp::update() {
}

// TODO: make this a member function
void calculate_normals(Vertex * vertices, size_t num_vertices, Triangle * triangles, size_t num_triangles) {
        for(size_t i=0; i<num_vertices; i++)
            vertices[i].norm = glm::vec3(0.0f, 0.0f, 0.0f);

        for(size_t i=0; i < num_triangles; i++)
        {
            size_t vertex1 = triangles[i].v0;
            size_t vertex2 = triangles[i].v1;
            size_t vertex3 = triangles[i].v2;

            glm::vec4 vector1 = vertices[vertex2].pos - vertices[vertex1].pos;
            glm::vec4 vector2 = vertices[vertex3].pos - vertices[vertex1].pos;
            glm::vec3 vector1_3d(vector1.x, vector1.y, vector1.z);
            glm::vec3 vector2_3d(vector2.x, vector2.y, vector2.z);

            glm::vec3 normal = normalize(cross(vector1_3d, vector2_3d));
            vertices[vertex1].norm += normal;
            vertices[vertex2].norm += normal;
            vertices[vertex3].norm += normal;
        }

        for(size_t i=0; i<num_vertices; i++)
            vertices[i].norm = normalize(vertices[i].norm);
}

Mesh::Mesh() { }
Mesh::~Mesh() { }

bool Mesh::load()
{
    std::cout << "Loading mesh from '" << filename << "'..." << std::endl;

    std::string line;
    std::ifstream file( filename.c_str() );

    typedef std::vector< glm::vec3 > PositionList;
    typedef std::vector< glm::vec3 > NormalList;
    typedef std::vector< glm::vec2 > UVList;
    typedef std::vector< Face > FaceList;

    static const char* scan_vertex = "%d";
    static const char* scan_vertex_uv = "%d/%d";
    static const char* scan_vertex_normal = "%d//%d";
    static const char* scan_vertex_uv_normal = "%d/%d/%d";

    size_t num_vertex;
    TriIndex tri[4];

    FaceList face_list;
    PositionList position_list;
    NormalList normal_list;
    UVList uv_list;

    int line_num = 0;

    std::string token;

    triangles.clear();

    ObjFormat format = VERTEX_ONLY;

    typedef std::map< TriIndex, unsigned int > VertexMap;
    VertexMap vertex_map;

    if ( !file.is_open() ) {
        std::cout << "Error opening file '" << filename << "' for mesh loading.\n";
        return false;
    }

    while ( getline( file, line ) )
    {
        std::stringstream stream( line );
        stream >> token;
        line_num++;

        if ( token == "v" ) {

            glm::vec3 position;
            stream >> position.x >> position.y >> position.z;

            if ( stream.fail() ) {
                std::cerr << "position syntax error on line " << line_num << std::endl;
                return false;
            }

            position_list.push_back( position );

        } else if ( token == "vn" ) {
            glm::vec3 normal;
            stream >> normal.x >> normal.y >> normal.z;

            if( stream.fail() ) {
                std::cerr << "normal syntax error on line " << line_num << std::endl;
                return false;
            }
            normal_list.push_back( normal );

        } else if ( token == "vt" ) {

            glm::vec2 uv;
            stream >> uv.x >> uv.y;

            if ( stream.fail() ) {
                std::cerr << "uv syntax error on line " << line_num << std::endl;
                return false;
            }

            uv_list.push_back( uv );

        } else if ( token == "f" ) {

            std::vector< std::string > face_tokens;
            std::string vert;

            while ( true ) {
                stream >> vert;
                if( stream.fail() )
                    break;
                face_tokens.push_back( vert );
            }

            // if it's the first time parsing a face, figure out the face format
            if ( face_list.size() == 0 ) {
                std::string token = face_tokens[0];

                if ( token.find( "//" ) != std::string::npos ) {
                    format = VERTEX_NORMAL;
                } else if ( token.find( '/' ) == std::string::npos ) {
                    format = VERTEX_ONLY;
                } else {
                    size_t p1 = token.find( '/' );
                    size_t p2 = token.rfind( '/' );
                    if ( p1 == p2 ) {
                        format = VERTEX_UV;
                    } else {
                        format = VERTEX_UV_NORMAL;
                    }
                }
            }

            num_vertex = face_tokens.size();

            if ( num_vertex > 4 || num_vertex < 3 ) {
                std::cerr << "Syntax error at line " << line_num
                          << ", face has incorrect number of vertices" << std::endl;
                return false;
            }

            for ( size_t i = 0; i < num_vertex; ++i ) {
                switch ( format )
                {
                case VERTEX_ONLY:
#ifdef WIN32
                    sscanf_s(
#else
                    sscanf(
#endif
                            face_tokens[i].c_str(),
                            scan_vertex,
                            &tri[i].vertex );
                    tri[i].normal = 0;
                    tri[i].tcoord = 0;
                    break;

                case VERTEX_UV:
#ifdef WIN32
                    sscanf_s(
#else
                    sscanf(
#endif
                            face_tokens[i].c_str(),
                            scan_vertex_uv,
                            &tri[i].vertex,
                            &tri[i].tcoord );
                    tri[i].normal = 0;
                    break;

                case VERTEX_NORMAL:
#ifdef WIN32
                    sscanf_s(
#else
                    sscanf(
#endif
                            face_tokens[i].c_str(),
                            scan_vertex_normal,
                            &tri[i].vertex,
                            &tri[i].normal );
                    tri[i].tcoord = 0;
                    break;

                case VERTEX_UV_NORMAL:
#ifdef WIN32
                    sscanf_s(
#else
                    sscanf(
#endif
                            face_tokens[i].c_str(),
                            scan_vertex_uv_normal,
                            &tri[i].vertex,
                            &tri[i].tcoord,
                            &tri[i].normal );
                    break;

                default:
                    std::cerr << "Syntax error, unrecongnized face format at line "
                              << line_num << std::endl;
                    break;
                }
            }

            for ( size_t i = 0; i < num_vertex; ++i ) {
                tri[i].vertex--;
                tri[i].normal--;
                tri[i].tcoord--;
            }

            Face f1 = { { tri[0], tri[1], tri[2] } };
            face_list.push_back( f1 );

            if ( num_vertex == 4 ) {
                Face f2 = { { tri[2], tri[3], tri[0] } };
                face_list.push_back( f2 );
            }

        } else if ( token == " " ) {

        } else {
            //std::cerr << "Unknown token on line " << line_num << std::endl;
        }

        token.clear();
        line.clear();
    }

    // build vertex list using map for shared vertices

    triangles.reserve( face_list.size() );
    vertices.reserve( face_list.size() * 2 );

    // current vertex index, for creating new vertices
    unsigned int vert_idx_counter = 0;

    for ( size_t i = 0; i < face_list.size(); ++i ) {
        const Face& face = face_list[i];
        MeshTriangle tri;
        for ( size_t j = 0; j < 3; ++j ) {
            // two vertices are only actually the same one if the vertex,
            // normal, and tcoord are all the same. use the map to check this.
            std::pair< VertexMap::iterator, bool > rv = vertex_map.insert( std::make_pair( face.v[j], vert_idx_counter ) );
            if ( rv.second ) {
                MeshVertex v;
                v.position = position_list[face.v[j].vertex];
                int nidx = face.v[j].normal;
                v.normal = nidx == -1 ? glm::vec3(0.0f) : normal_list[nidx];
                int tidx = face.v[j].tcoord;
                v.tex_coord = tidx == -1 ? glm::vec2(0.0f) : uv_list[tidx];
                vertices.push_back( v );
                vert_idx_counter++;
            }

            tri.vertices[j] = rv.first->second;
        }
        triangles.push_back( tri );
    }

    std::cout << "Successfully loaded mesh '" << filename << "'.\n";
    return true;
}
