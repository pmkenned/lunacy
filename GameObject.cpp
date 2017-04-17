#include "stdafx.h"
#include "GameObject.h"
#include "Component.h"

void GameObject::update() {
    for(auto i = components.begin(); i != components.end(); i++) {
        (*i)->update();
    }
    // TODO: determine if game objects should have children
    //for(auto i = children.begin(); i != children.end(); i++) {
    //    (*i)->update();
    //}
}

void GameObject::addComponent(Component * _component) {
    components.push_back(_component);
}