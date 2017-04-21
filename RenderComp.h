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
        image = SOIL_load_image("moon_spec3.png", &width, &height, 0, SOIL_LOAD_RGB);
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

        glUniform1i(glGetUniformLocation(getGameObject()->getProgram(), "textured"), 1); // TODO: consult mesh component
        glUniform1i(glGetUniformLocation(getGameObject()->getProgram(), "specTextured"), 0); // TODO

        // TODO: maybe the transform component should have a pre-computed model matrix which updates whenever the transform is changed
        GLint modelLoc = glGetUniformLocation(getGameObject()->getProgram(), "model");

        // TODO: maybe tidy this up a bit
        glm::mat4 identity;
        glm::mat4 TranslationMatrix = glm::translate(identity, tc->getPosition());
        glm::mat4 RotationMatrix = glm::toMat4(tc->getOrientation());
        glm::mat4 ScaleMatrix = glm::scale(identity, tc->getScale());
        glm::mat4 model = TranslationMatrix * RotationMatrix * ScaleMatrix;

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        std::vector<MeshCompMesh *> const & meshes = mc->getMeshes();
        for(size_t i = 0; i < meshes.size(); i++) {
            if(!meshes[i]->getEnabled())
                continue;
            size_t num_triangles = meshes[i]->numTriangles();
            GLuint VAO = meshes[i]->getVAO();
            glBindVertexArray(VAO);
                glDrawElements(GL_TRIANGLES, 3*num_triangles, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }

    void enable() { active = true; }
    void disable() { active = false; }

private:
    GLuint moonMap; // TODO
    GLuint specularMap;
    MeshComp * mc;
    TransformComp * tc;
    bool active;
};