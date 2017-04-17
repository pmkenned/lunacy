#pragma once

#include "stdafx.h"

class GameObject;

class Component {
public:
    Component(GameObject * _game_object) : game_object(_game_object) {}
    virtual ~Component() {}
    virtual void update() = 0;
    GameObject * getGameObject() { return game_object; }
private:
    GameObject * game_object;
};
