#include <SDL2/SDL.h>
#include <SDL2/SDL_scancode.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <vector>
#include <stdlib.h>

#define PI 3.14f

#define RESX 3200
#define RESY 1800

#define SHIP 1
#define CIRCLE 2

#define NONE 0
#define TST_GRAV 1
#define GEN_GRAV 2
#define KIL_COLI 4

#define FACING_SPEED 3*PI
#define ENGINE_ACC 1000.0f

#define BOUNCE_DESACC 0.5f

using namespace std;

struct vec2
{
    float x;
    float y;
};
typedef struct vec2 Vec2;

struct element
{
    Vec2 pos;
    Vec2 spd;
    Vec2 acc;
    int rgba[4];
    float r;
    float mass;
    int type;
    int flags;
    float facing;
};
typedef struct element Element;

int err;
SDL_Window* window;
SDL_Renderer* renderer;
vector<Element*> world;
Element* player1;
Element* player2;

void draw(Element* e)
{
    if(e->type == CIRCLE)
    {
        SDL_SetRenderDrawColor(renderer, e->rgba[0],e->rgba[1],e->rgba[2],e->rgba[3]);
        int num_p = e->r;
        float step = 2.0f*PI/num_p;
        for(int i=0;i<num_p;i++)
        {
            SDL_RenderDrawLine(renderer,e->pos.x+e->r*cos(i*step),e->pos.y+e->r*sin(i*step),e->pos.x+e->r*cos((i+1)*step),e->pos.y+e->r*sin((i+1)*step));
        }
    }
    else if(e->type == SHIP)
    {
        SDL_Rect r = { (int) (e->pos.x - e->r), (int) (e->pos.y - e->r), (int) 2*e->r, (int) 2*e->r }; 
        SDL_SetRenderDrawColor(renderer, e->rgba[0],e->rgba[1],e->rgba[2],e->rgba[3]);

        SDL_RenderDrawLine(renderer,e->pos.x,e->pos.y,e->pos.x+e->r*cos(e->facing),e->pos.y+e->r*sin(e->facing));
        SDL_RenderDrawLine(renderer,e->pos.x,e->pos.y,e->pos.x+e->r*cos(e->facing+2.0f*PI/3.0f),e->pos.y+e->r*sin(e->facing+2.0f*PI/3.0f));
        SDL_RenderDrawLine(renderer,e->pos.x,e->pos.y,e->pos.x+e->r*cos(e->facing-2.0f*PI/3.0f),e->pos.y+e->r*sin(e->facing-2.0f*PI/3.0f));
        SDL_RenderDrawLine(renderer,e->pos.x+e->r*cos(e->facing),e->pos.y+e->r*sin(e->facing),e->pos.x+e->r*cos(e->facing-2.0f*PI/3.0f),e->pos.y+e->r*sin(e->facing-2.0f*PI/3.0f));
        SDL_RenderDrawLine(renderer,e->pos.x+e->r*cos(e->facing),e->pos.y+e->r*sin(e->facing),e->pos.x+e->r*cos(e->facing+2.0f*PI/3.0f),e->pos.y+e->r*sin(e->facing+2.0f*PI/3.0f));
    }
}

float dist(Vec2* v,Vec2* w)
{
    return sqrt((v->x - w->x)*(v->x - w->x) + (v->y - w->y)*(v->y - w->y));
}

void updatePositions(unsigned long _dt)
{
    float dt = ((float) _dt) / 1000.0f;
    for(int i=0;i<world.size();i++)
    {
        world[i]->pos.x += world[i]->spd.x*dt;
        world[i]->pos.y += world[i]->spd.y*dt;

        world[i]->spd.x += world[i]->acc.x*dt;
        world[i]->spd.y += world[i]->acc.y*dt;
    }
}

void colideBorder(Element* e)
{
    if(e->pos.x + e->r > RESX)
    {
        e->pos.x = RESX - e->r;
        e->spd.x = -BOUNCE_DESACC * abs(e->spd.x);
    }
    if(e->pos.y + e->r > RESY)
    {
        e->pos.y = RESY - e->r;
        e->spd.y = -BOUNCE_DESACC *abs(e->spd.y);
    }

    if(e->pos.x - e->r < 0)
    {
        e->pos.x = 0 + e->r;
        e->spd.x = BOUNCE_DESACC * abs(e->spd.x);
    }
    if(e->pos.y - e->r < 0)
    {
        e->pos.y = 0 + e->r;
        e->spd.y = BOUNCE_DESACC * abs(e->spd.y);
    } 
}

void colideElements(Element* a,Element* b)
{
    if( dist(&(a->pos),&(b->pos)) < (a->r + b->r))
    {
        Vec2 eix = {-(a->pos.y-b->pos.y),(a->pos.x-b->pos.x)};
        float fat = (a->spd.x*eix.x + a->spd.y*eix.y)/(eix.x*eix.x+eix.y*eix.y);
        Vec2 proj = { eix.x*fat, eix.y*fat };
        a->spd.x = -(a->spd.x - proj.x) + proj.x;
        a->spd.y = -(a->spd.y - proj.y) + proj.y;

        a->pos.x += 10*(a->pos.x - b->pos.x)/sqrt((a->pos.x - b->pos.x)*(a->pos.x - b->pos.x)+(a->pos.y - b->pos.y)*(a->pos.y - b->pos.y));
        a->pos.y += 10*(a->pos.y - b->pos.y)/sqrt((a->pos.x - b->pos.x)*(a->pos.x - b->pos.x)+(a->pos.y - b->pos.y)*(a->pos.y - b->pos.y));

        a->spd.x *= BOUNCE_DESACC;
        a->spd.y *= BOUNCE_DESACC;
    }
}


void colision()
{
    for(int i=0;i<world.size();i++)
    {
        colideBorder(world[i]);
        for(int j=0;j<world.size();j++)
        {
            if(i==j) continue;
            colideElements(world[i],world[j]);
        }
    }
}

void gravity()
{
    for(int i=0;i<world.size();i++)
    {
        if(world[i]->flags & TST_GRAV)
        {
            world[i]->acc.x = 0.0f;
            world[i]->acc.y = 0.0f;
            for(int j=0;j<world.size();j++)
            {
                if(i==j) continue;
                if(!(world[j]->flags & GEN_GRAV)) continue;
                float dist2 = (world[i]->pos.y-world[j]->pos.y)*(world[i]->pos.y-world[j]->pos.y)+(world[i]->pos.x-world[j]->pos.x)*(world[i]->pos.x-world[j]->pos.x);
                world[i]->acc.x += 100*world[j]->mass/dist2*(world[j]->pos.x-world[i]->pos.x);
                world[i]->acc.y += 100*world[j]->mass/dist2*(world[j]->pos.y-world[i]->pos.y);
            }
        }
    }
}

void drawElements()
{
    for(int i=0;i<world.size();i++)
    {
        draw(world[i]);
    }
}

void keyboardManagement(unsigned long _dt)
{
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    float dt = ((float) _dt) / 1000.0f;
    if(state[SDL_SCANCODE_UP]){
            player1->spd.x += ENGINE_ACC*cos(player1->facing)*dt;
            player1->spd.y += ENGINE_ACC*sin(player1->facing)*dt;
    }
    if(state[SDL_SCANCODE_DOWN]){
            player1->spd.x -= ENGINE_ACC*cos(player1->facing)*dt;
            player1->spd.y -= ENGINE_ACC*sin(player1->facing)*dt;
    }
    if(state[SDL_SCANCODE_LEFT]){
            player1->facing -= FACING_SPEED*dt;
    }
    if(state[SDL_SCANCODE_RIGHT]){
            player1->facing += FACING_SPEED*dt;
    }
    if(player1->facing>2*PI) player1->facing -= 2*PI;
    else if(player1->facing<0) player1->facing += 2*PI;
    if(player2->facing>2*PI) player2->facing -= 2*PI;
    else if(player2->facing<0) player2->facing += 2*PI;

}

void mouseManagement(unsigned long _dt)
{
    float dt = ((float) _dt) / 1000.0f;
    int x,y;
    int state = SDL_GetMouseState(&x,&y);
    if((float)x-player2->pos.x!=0){
        if((float)x>player2->pos.x) player2->facing = atan(((float)y-player2->pos.y)/((float)x-player2->pos.x));
        else player2->facing = atan(((float)y-player2->pos.y)/((float)x-player2->pos.x)) + PI;
    }
    if(state & SDL_BUTTON(SDL_BUTTON_RIGHT)){
            player2->spd.x += ENGINE_ACC*cos(player2->facing)*dt;
            player2->spd.y += ENGINE_ACC*sin(player2->facing)*dt;
    }
}

void display()
{
    SDL_SetRenderDrawColor(renderer, 0x00,0x00,0x00,0x00);
    SDL_RenderFillRect(renderer, NULL);

    drawElements(); 
    SDL_RenderPresent(renderer); 
}

int main (int argc, char* args[])
{
    err = SDL_Init(SDL_INIT_EVERYTHING);
    assert(err == 0);
    window = SDL_CreateWindow("Game 3200x1800",0, 0, RESX, RESY, SDL_WINDOW_FULLSCREEN);
    assert(window != NULL);
    renderer = SDL_CreateRenderer(window, -1, 0);
    assert(renderer != NULL);

    unsigned long last_time = SDL_GetTicks();
    unsigned long new_time;
    unsigned long time_elapsed;

    Element ball = {{2100,900},{0,-300},{0,0},{0x99,0x00,0xBB,0x00},100,300,CIRCLE,GEN_GRAV | TST_GRAV,0};
    Element ball2 = {{1600,900},{0,0},{0,0},{0x99,0x55,0x3B,0x00},200,1000,CIRCLE,GEN_GRAV,0};
    Element _player1 = {{200,900},{0,0},{0,0},{0xFF,0x00,0x00,0x00},30,30,SHIP,TST_GRAV,0};
    Element _player2 = {{3000,900},{0,0},{0,0},{0x00,0x00,0xFF,0x00},30,30,SHIP,TST_GRAV,0};
    player1 = &_player1;
    player2 = &_player2;
    world.push_back(player1);
    world.push_back(player2);
    world.push_back(&ball);
    world.push_back(&ball2);
    SDL_Event e;

    while (1)
    {
        new_time = SDL_GetTicks();
        time_elapsed = new_time - last_time;

        if(SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT) break;
        }
        keyboardManagement(time_elapsed);
        mouseManagement(time_elapsed);
        updatePositions(time_elapsed);
        colision();
        gravity();
        display();

        last_time = new_time;
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

	return 0;
}