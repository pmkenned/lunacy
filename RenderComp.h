#pragma once

#include "stdafx.h"
#include "Component.h"
#include "MeshComp.h"

class RenderComp : public Component {
public:

    RenderComp(GameObject * _game_object, bool _active = true) : Component(_game_object), active(_active) {

        glGenTextures(1, &moonMap);
        glGenTextures(1, &specularMap);
        int width, height;
        unsigned char* image;

        // TODO: move the file name to the mesh, make this a loop over the textured meshes
        image = SOIL_load_image("moon.png", &width, &height, 0, SOIL_LOAD_RGB);
        glBindTexture(GL_TEXTURE_2D, moonMap);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
            glGenerateMipmap(GL_TEXTURE_2D);
            SOIL_free_image_data(image);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Specular map
        image = SOIL_load_image("container2_specular.png", &width, &height, 0, SOIL_LOAD_RGB);
        glBindTexture(GL_TEXTURE_2D, specularMap);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
            glGenerateMipmap(GL_TEXTURE_2D);
            SOIL_free_image_data(image);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);

        mc = getGameObject()->getComponent<MeshComp>();
        tc = getGameObject()->getComponent<TransformComp>();

        std::vector<MeshCompMesh *> const & meshes = mc->getMeshes();
        for(auto i = meshes.begin(); i != meshes.end(); i++) {

            size_t num_vertices = (*i)->numVertices();
            Vertex * vertices = (*i)->getVertices();
            size_t num_triangles = (*i)->numTriangles();
            Triangle * triangles = (*i)->getTriangles();

            GLuint VBO, IBO;
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &IBO);

            GLuint VAO;
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

                VAOs.push_back(VAO);

                // Vertex buffer
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
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
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Triangle) * num_triangles, triangles, GL_STATIC_DRAW);

                numTri.push_back(num_triangles);

            glBindVertexArray(0);
        }

    }

    void update() {
        if(!active)
            return;

        // Bind cube texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, moonMap);

        // Bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        //size_t num_triangles = mc->numTriangles();

        glUniform1i(glGetUniformLocation(getGameObject()->getProgram(), "textured"), 1); // TODO: consult mesh component
        glUniform1i(glGetUniformLocation(getGameObject()->getProgram(), "specTextured"), 0); // TODO

        GLint modelLoc = glGetUniformLocation(getGameObject()->getProgram(), "model");
        glm::mat4 model;
        model = glm::mat4();
        model = glm::translate(model, tc->getPosition() );
        //model = glm::rotate(model, radians, axis);
        model = glm::toMat4(tc->getOrientation()) * model; // use quaternion for rotation
        model = glm::scale(model, tc->getScale() );

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        std::vector<MeshCompMesh *> const & meshes = mc->getMeshes();
        for(size_t i = 0; i < VAOs.size(); i++) {
            size_t num_triangles = numTri[i];
            glBindVertexArray(VAOs[i]);
                glDrawElements(GL_TRIANGLES, 3*num_triangles, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }

    void enable() { active = true; }
    void disable() { active = false; }

private:

    std::vector<GLuint> VAOs;
    //std::vector<GLuint> VBOs;
    //std::vector<GLuint> IBOs;
    std::vector<size_t> numTri;

    GLuint moonMap; // TODO
    GLuint specularMap;
    MeshComp * mc;
    TransformComp * tc;
    bool active;
};