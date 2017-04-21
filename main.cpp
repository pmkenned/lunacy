// moon.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Game.h"

// Questions:
// About QuadTree:
// * Should nodes meshes share the IBO somehow?
// * Should merging nodes, should the child data be freed? Scheduled for freeing?
// About GameObjects and Components:
// * Should subclassing of GameObjects be allowed?
// * Do GameObjects need to be able to have children? Is this a useful feature?
// About rendering:
// * Is there a better way to deal with the shader handle?
// * Can the render data be separated from the world data? (Would I still have a render component?)
// * Should renders be skipped if tick is late? How to do this?
// * Should the mesh component contain VAOs or only the render component?
// * Should the mesh component know about physics?
// Other:
// * Can the mesh code be cleaned up?
// * Should subclasses be able to access the data of their base class directly? (protected data members)

int _tmain(int argc, _TCHAR* argv[])
{
    Game g(40, 1.0f);

    //g.init(SDL_WINDOW_FULLSCREEN_DESKTOP);
    if(g.init() < 0) {
        getchar();
        return 0;
    }

    g.loop();

	return 0;
}