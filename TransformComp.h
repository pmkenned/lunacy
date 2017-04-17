#pragma once

#include "stdafx.h"

#include "Component.h"

class TransformComp : public Component {
public:
    TransformComp(GameObject * _game_object) : Component(_game_object), scale(1.0f) {}
    
    void setPosition(glm::vec3 const & _pos) { pos = _pos; }
    void setScale(glm::vec3 const & _scale) { scale = _scale; }
    void setOrientation(glm::quat const & _ori) { ori = _ori; }

    glm::vec3 const & getPosition()    { return pos; }
    glm::vec3 const & getScale()       { return scale; }
    glm::quat const & getOrientation() { return ori; }

    void update() {}

private:
    glm::vec3 pos;
    glm::vec3 scale;
    glm::quat ori;
};