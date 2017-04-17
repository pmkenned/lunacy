#pragma once

#include "stdafx.h"

class Component;

class GameObject {
public:
    GameObject() {}
    GameObject(GLuint _program) : program(_program) {}

    void update();
    void addComponent(Component * _component);

    template <typename T>
    T * getComponent() {
        for(auto i = components.begin(); i != components.end(); i++) {
            if(dynamic_cast<T*>(*i)) {
                return dynamic_cast<T*>(*i);
            }
        }
        return 0;
    }

    void setProgram(GLuint _program) { program = _program; }
    GLuint getProgram() { return program; }

    // TODO: determine if GameObjects need children
    //void addChild(GameObject * child) { children.push_back(child); }

private:
    std::vector<Component *> components;
    //std::vector<GameObject *> children;
    GLuint program; // TODO: move this somewhere else
};