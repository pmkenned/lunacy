#pragma once

#include "stdafx.h"
#include "Camera.h"
#include "GameObject.h"
#include "TransformComp.h"
#include "RenderComp.h"
#include "MeshComp.h"
#include "QuadTree.h"
#include "LoadShaders.h"

ShaderInfo shaders[] = {
    { GL_VERTEX_SHADER, "lighting.vs" },
    { GL_FRAGMENT_SHADER, "lighting.fs" },
    { GL_NONE, NULL }
};

struct PointLight {
    glm::vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

PointLight pointlights[] = {
    { glm::vec3( 0.7f,  0.2f,  2.0f) , 1.0f, 0.09f, 0.032f, glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f) },
    { glm::vec3( 2.3f, -3.3f, -4.0f) , 1.0f, 0.09f, 0.032f, glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f) },
    { glm::vec3(-4.0f,  2.0f, -12.0f), 1.0f, 0.09f, 0.032f, glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f) },
    { glm::vec3( 0.0f,  0.0f, -3.0f) , 1.0f, 0.09f, 0.032f, glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f) }
};

size_t num_lights = sizeof(pointlights)/sizeof(PointLight);

class BoxObject : public GameObject {
public:
    BoxObject(GLuint _program) : GameObject(_program) {

        addComponent(new TransformComp(this));

        MeshComp * mc = new MeshComp(this);
        addComponent(mc);
        //mc->load("cube.obj");
        //mc->copy_from_mesh();

        //mc->make_cube(0.1f, 0.1f, 0.1f, 0.0f, 0.0f, 0.0f);
        //mc->make_square(1.0f, 1.0f);
        mc->newSquareMesh();

        addComponent(new RenderComp(this));
    }

private:
};

class GameClock {
public:
    GameClock(Uint32 tps = 40, float rate = 1.0f) : ticks_per_second(tps), sim_rate(rate), tick_count(0), game_ms(0), late_count(0), late_ms(0), after(0), before(0) {
        set_tick_interval();
    }

    void init() {
        before = SDL_GetTicks();
    }

    void tick() {
        after = SDL_GetTicks();
        Uint32 update_duration_ms = after - before; // measure how long it took to do the update
        int surplus_ms = tick_interval_ms - update_duration_ms;
        bool is_late = (surplus_ms < 0) ? true : false;
        Uint32 sleep_time_ms = is_late ? 1 : surplus_ms;
        before = after + sleep_time_ms;
        SDL_Delay(sleep_time_ms);
        game_ms += (Uint32) (tick_interval_ms * sim_rate);
        if(is_late) {
            late_count++;
            late_ms += -surplus_ms; // surplus will be negative
        }
        tick_count++;
    }

    Uint32 get_late_count() { return late_count; }
    float get_sim_rate() { return sim_rate; }
    Uint32 get_ticks_per_second() { return ticks_per_second; }
    Uint32 get_tick_interval_ms() { return tick_interval_ms; }
    float get_dt_per_tick() { return tick_interval_ms / 1000.0f; }

    void set_ticks_per_second(Uint32 tps) {
        ticks_per_second = tps;
        set_tick_interval();
    }

    void set_sim_rate(float rate) {
        sim_rate = rate;
        set_tick_interval();
    }

    void sim_rate_factor(float factor) {
        sim_rate *= factor;
        set_tick_interval();
    }

    void print_stats() {
        std::cout << "================================\n";
        std::cout << "Total game time: " << game_ms/1000.0f << std::endl;
        std::cout << "Total wall-clock time: " << SDL_GetTicks()/1000.0f << std::endl;
        std::cout << "Total game ticks: " << tick_count << std::endl;
        std::cout << "Total late ticks: " << late_count << std::endl;
        std::cout << "Total late time (s): " << late_ms/1000.0f << std::endl;
        std::cout << "================================\n";
    }

private:

    void set_tick_interval() {
        tick_interval_ms = (Uint32)(1000.0f/(ticks_per_second*sim_rate));
        std::cout << "ticks per second: " << ticks_per_second << " simulation rate: " << sim_rate << " tick interval (ms): " << tick_interval_ms << std::endl; 
    }

    Uint32 ticks_per_second;
    float sim_rate;
    int tick_count;
    Uint32 game_ms;
    Uint32 late_count;
    Uint32 late_ms;
    Uint32 after, before;
    Uint32 tick_interval_ms; // a function of sim_rate and ticks_per_second

};

class Game {
public:

    Game(Uint32 tps = 40.0f, float rate = 1.0f) :
        window(0),
        done(false),
        windowWidth(1600), windowHeight(900), sdl_flags(SDL_WINDOW_OPENGL),
        clk(tps, rate),
        camera(glm::vec3(0.0f, 0.0f, 3.0f)),
        wireframe(false)
    {
        for(int i=0; i<NUM_KEYS; i++) {
            keys[i] = false;
        }
    }

    GLuint getProgram() { return program; }

    int init(Uint32 flags = 0) {

        // SDL initialization
        SDL_Init(SDL_INIT_VIDEO);

        sdl_flags |= flags;
        if(flags && SDL_WINDOW_FULLSCREEN_DESKTOP) {
            SDL_DisplayMode dm;
            if (SDL_GetDesktopDisplayMode(0, &dm) != 0) {
                SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
            }
            else {
                windowWidth = dm.w;
                windowHeight = dm.h;
            }
        }
        window = SDL_CreateWindow("", 50, 50, windowWidth, windowHeight, sdl_flags);
        SDL_GL_CreateContext(window);

        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        glClearDepth( 1.0f );
        glEnable( GL_DEPTH_TEST );
        glDepthFunc( GL_LEQUAL );
        glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
        glViewport( 0, 0, ( GLsizei )windowWidth, ( GLsizei )windowHeight );

        //glEnable(GL_CULL_FACE);

        SDL_SetRelativeMouseMode(SDL_TRUE);

        // GLEW initialization
        GLenum err = glewInit();
        if (err != GLEW_OK) {
            std::cerr << "glewInit returned " << err << std::endl;
            exit(1);
        }
        if (!GLEW_VERSION_2_1) { // check that the machine supports the 2.1 API.
            std::cerr << "!GLEW_VERSION_2_1" << std::endl;
            exit(1);
        }

        program = LoadShaders(shaders);
        if(program == 0)
            return -1;

        // Set texture units
        glUseProgram(program);
        glUniform1i(glGetUniformLocation(program, "material.diffuse"),  0);
        glUniform1i(glGetUniformLocation(program, "material.specular"), 1);

        glUniform1i(glGetUniformLocation(program, "invertTexCoords"), 1);

        // Test object:
        //BoxObject * boxObj = new BoxObject(program);
        //boxObj->getComponent<TransformComp>()->setPosition(glm::vec3(0.0f, 0.0f, -3.0f));
        //objects.push_back(boxObj);

        objects.push_back(new QuadTree(program));
        objects[0]->getComponent<TransformComp>()->setPosition(glm::vec3(0.0f, 0.0f, -3.0f));

        return 0;
    }

    void loop() {

        clk.init();

        while(!done) {
            SDL_PumpEvents();
            process_events();
            render();
            for(auto i = objects.begin(); i != objects.end(); i++) {
                (*i)->update();
            }
            glFlush();
            SDL_GL_SwapWindow(window);
            clk.tick();
        }

        clk.print_stats();
        //std::getchar();

        SDL_Quit();
    }

private:

    void render()
    {
        // Clear the colorbuffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glUseProgram(program);

        GLint viewPosLoc = glGetUniformLocation(program, "viewPos");
        glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);

        glUniform1f(glGetUniformLocation(program, "material.shininess"), 32.0f);

        // Directional light
        glUniform3f(glGetUniformLocation(program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
        glUniform3f(glGetUniformLocation(program, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
        glUniform3f(glGetUniformLocation(program, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
        glUniform3f(glGetUniformLocation(program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);

        // Point lights
        for(size_t i=0; i < num_lights; i++) {
            char uniformName[100];
            sprintf_s(uniformName, 100, "pointLights[%d].position", i);
            glUniform3f(glGetUniformLocation(program, uniformName), pointlights[i].position.x, pointlights[i].position.y, pointlights[i].position.z);
            sprintf_s(uniformName, 100, "pointLights[%d].ambient", i);
            glUniform3f(glGetUniformLocation(program, uniformName), pointlights[i].ambient.x, pointlights[i].ambient.y, pointlights[i].ambient.z);
            sprintf_s(uniformName, 100, "pointLights[%d].diffuse", i);
            glUniform3f(glGetUniformLocation(program, uniformName), pointlights[i].diffuse.x, pointlights[i].diffuse.y, pointlights[i].diffuse.z);
            sprintf_s(uniformName, 100, "pointLights[%d].specular", i);
            glUniform3f(glGetUniformLocation(program, uniformName), pointlights[i].specular.x, pointlights[i].specular.y, pointlights[i].specular.z);
            sprintf_s(uniformName, 100, "pointLights[%d].constant", i);
            glUniform1f(glGetUniformLocation(program, uniformName), pointlights[i].constant);
            sprintf_s(uniformName, 100, "pointLights[%d].linear", i);
            glUniform1f(glGetUniformLocation(program, uniformName), pointlights[i].linear);
            sprintf_s(uniformName, 100, "pointLights[%d].quadratic", i);
            glUniform1f(glGetUniformLocation(program, uniformName), pointlights[i].quadratic);
        }

        glUniform1i(glGetUniformLocation(program, "numActiveLights"), num_lights);

        // Create camera transformations
        glm::mat4 view;
        view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)windowWidth / (GLfloat)windowHeight, 0.1f, 100.0f); // TODO: initialize in init()

        GLint viewLoc  = glGetUniformLocation(program, "view");
        GLint projLoc  = glGetUniformLocation(program, "projection");

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    }

    void process_events() {
        static bool first_mouse = true;
        SDL_Event event;

        while(SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT)) {
            switch(event.type) {
                case SDL_KEYDOWN:
                    if ( event.key.keysym.sym == SDLK_ESCAPE ) { done = true; }
                    if ( event.key.keysym.sym == SDLK_COMMA )  { clk.sim_rate_factor(1.0f/1.1f); }
                    if ( event.key.keysym.sym == SDLK_PERIOD ) { clk.sim_rate_factor(1.1f); }
                    if ( event.key.keysym.sym == SDLK_w ) { keys[W_KEY] = true; }
                    if ( event.key.keysym.sym == SDLK_s ) { keys[S_KEY] = true; }
                    if ( event.key.keysym.sym == SDLK_a ) { keys[A_KEY] = true; }
                    if ( event.key.keysym.sym == SDLK_d ) { keys[D_KEY] = true; }
                    if ( event.key.keysym.sym == SDLK_q ) { }
                    if ( event.key.keysym.sym == SDLK_e ) { }
                    if ( event.key.keysym.sym == SDLK_z ) { wireframe = !wireframe; }
                    break;

                case SDL_KEYUP:
                    if ( event.key.keysym.sym == SDLK_w ) { keys[W_KEY] = false; }
                    if ( event.key.keysym.sym == SDLK_s ) { keys[S_KEY] = false; }
                    if ( event.key.keysym.sym == SDLK_a ) { keys[A_KEY] = false; }
                    if ( event.key.keysym.sym == SDLK_d ) { keys[D_KEY] = false; }
                    break;

                case SDL_MOUSEMOTION:
                    if(first_mouse) {
                        first_mouse = false;
                    }
                    else {
                        camera.ProcessMouseMovement((float)event.motion.xrel, (float)event.motion.yrel*-1.0f);
                    }
                    break;

                case SDL_WINDOWEVENT:
                    if(event.window.event == SDL_WINDOWEVENT_CLOSE) { done = true; }
                    break;

                default:
                    break;
            }
        }

        if(keys[W_KEY])
            camera.ProcessKeyboard(FORWARD, clk.get_dt_per_tick());
        if(keys[S_KEY])
            camera.ProcessKeyboard(BACKWARD, clk.get_dt_per_tick());
        if(keys[A_KEY])
            camera.ProcessKeyboard(LEFT, clk.get_dt_per_tick());
        if(keys[D_KEY])
            camera.ProcessKeyboard(RIGHT, clk.get_dt_per_tick());
    }

    SDL_Window * window;
    bool done;
    int windowWidth, windowHeight;
    Uint32 sdl_flags;
    GameClock clk;
    Camera camera;
    GLuint program;
    std::vector<GameObject *> objects;

    bool wireframe;
    enum KEYS { W_KEY = 0, S_KEY, A_KEY, D_KEY, NUM_KEYS };
    bool keys[NUM_KEYS];
};