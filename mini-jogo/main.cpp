#include <SDL2/SDL.h>
#include <SDL2/SDL_scancode.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <list>
#include <stdlib.h>
#include <string.h>

#define PI 3.14f

#define SHIP 1
#define CIRCLE 2

#define NONE 0
#define TEST_FOR_GRAVITY 1
#define GENERATE_GRAVITY 2
#define KILL_ON_COLISION 4
#define DAMAGES 8
#define IGNORE_BORDER 16
#define IGNORE_COLISION 32
#define BOUNCE_ONCE 64
#define DESACC_ON_COLISION 128

#define FACING_SPEED 3*PI
#define ENGINE_ACC 0.3f
#define BOUNCE_DESACC 0.5f

#define SHIP_RADIUS 0.015f
#define P1_COLOR {0xFF,0x00,0x00,0x00}
#define P2_COLOR {0x30,0xD0,0xFF,0x00}

#define SHOT_SPEED 0.25
#define SHOT_RATE 150
#define SHOT_RADIUS 0.007f

#define TOP_BAR_HEIGHT 0.03f
#define SCORE_STEP 0.1f

using namespace std;

int resolution_x; // 3200
int resolution_y; // 1800

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
list<Element*> world;

Element* player1;
Element* player2;

unsigned long time_til_next_shot_p1 = 0;
unsigned long time_til_next_shot_p2 = 0;

int score;

bool r_pressed = false;
bool game_close = false;

void drawChar(char c)
{
    if(c == 'R')
    {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
        SDL_RenderDrawLine(renderer,resolution_x*0.4,resolution_y*0.8,resolution_x*0.4,resolution_y*0.2);
        SDL_RenderDrawLine(renderer,resolution_x*0.4,resolution_y*0.2,resolution_x*0.6,resolution_y*0.2);
        SDL_RenderDrawLine(renderer,resolution_x*0.6,resolution_y*0.2,resolution_x*0.6,resolution_y*0.4);
        SDL_RenderDrawLine(renderer,resolution_x*0.6,resolution_y*0.4,resolution_x*0.4,resolution_y*0.4);
        SDL_RenderDrawLine(renderer,resolution_x*0.5,resolution_y*0.4,resolution_x*0.6,resolution_y*0.8);
    }
}

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
    for(list<Element*>::iterator it=world.begin(); it != world.end(); it++)
    {
        (*it)->pos.x += (*it)->spd.x*dt;
        (*it)->pos.y += (*it)->spd.y*dt;
        (*it)->spd.x += (*it)->acc.x*dt;
        (*it)->spd.y += (*it)->acc.y*dt;
    }
}

bool colideBorder(Element* e)
{
    if(e->flags & IGNORE_COLISION) return false;
    if(e->flags & IGNORE_BORDER) return false;
    bool col = false;
    if(e->pos.x + e->r > resolution_x)
    {
        e->pos.x = resolution_x - e->r;
        e->spd.x = -abs(e->spd.x);
        if(e->flags & DESACC_ON_COLISION) e->spd.x *= BOUNCE_DESACC;
        col = true;
    }
    if(e->pos.y + e->r > resolution_y)
    {
        e->pos.y = resolution_y - e->r;
        e->spd.y = -abs(e->spd.y);
        if(e->flags & DESACC_ON_COLISION) e->spd.y *= BOUNCE_DESACC;
        col = true;
    }
    if(e->pos.x - e->r < 0)
    {
        e->pos.x = 0 + e->r;
        e->spd.x = abs(e->spd.x);
        if(e->flags & DESACC_ON_COLISION) e->spd.x *= BOUNCE_DESACC;
        col = true;
    }
    if(e->pos.y - e->r < (int) (TOP_BAR_HEIGHT*(resolution_x+resolution_y)/2.0))
    {
        e->pos.y = (int) (TOP_BAR_HEIGHT*(resolution_x+resolution_y)/2.0)+ e->r;
        e->spd.y = abs(e->spd.y);
        if(e->flags & DESACC_ON_COLISION) e->spd.y *= BOUNCE_DESACC;
        col = true;
    } 
    return col;
}

void colideElements(Element* a,Element* b)
{
    if(a->flags & IGNORE_COLISION) return;

    Vec2 dist = {(a->pos.x-b->pos.x),(a->pos.y-b->pos.y)};
    float dist_norm = sqrt((dist.x*dist.x) + (dist.y*dist.y));
    float spd_norm = sqrt((a->spd.x*a->spd.x) + (a->spd.y*a->spd.y));
    float cos_angle = (dist.x*a->spd.x + dist.y*a->spd.y)/(dist_norm*spd_norm);

    if(cos_angle <= 0)
    {
        Vec2 eix = {-(a->pos.y-b->pos.y),(a->pos.x-b->pos.x)};
        float fat = (a->spd.x*eix.x + a->spd.y*eix.y)/(eix.x*eix.x+eix.y*eix.y);
        Vec2 proj = { eix.x*fat, eix.y*fat };
        a->spd.x = -(a->spd.x - proj.x) + proj.x;
        a->spd.y = -(a->spd.y - proj.y) + proj.y;
    }
    a->pos.x += a->spd.x*0.06;
    a->pos.y += a->spd.y*0.06;
    //a->pos.x += 10*(a->pos.x - b->pos.x)/sqrt((a->pos.x - b->pos.x)*(a->pos.x - b->pos.x)+(a->pos.y - b->pos.y)*(a->pos.y - b->pos.y));
    //a->pos.y += 10*(a->pos.y - b->pos.y)/sqrt((a->pos.x - b->pos.x)*(a->pos.x - b->pos.x)+(a->pos.y - b->pos.y)*(a->pos.y - b->pos.y));

    if(a->flags & DESACC_ON_COLISION)
    {
        a->spd.x *= BOUNCE_DESACC;
        a->spd.y *= BOUNCE_DESACC;
    }

    if(a==player1 && b->flags & DAMAGES)
    {
        score-=(int)(SCORE_STEP*resolution_x);
        //b->flags = b->flags | KILL_ON_COLISION;
    }
    if(a==player2 && b->flags & DAMAGES)
    {
        score+=(int)(SCORE_STEP*resolution_x);
        //b->flags = b->flags | KILL_ON_COLISION;
    }
}

bool testColisionElements(Element* a,Element* b)
{
    if( dist(&(a->pos),&(b->pos)) < (a->r + b->r))
    {
        colideElements(a,b);
        colideElements(b,a);
        return true;
    }
    return false;
}
void colision()
{
    for(list<Element*>::iterator it=world.begin(); it != world.end(); it++)
    {
        if(colideBorder((*it)) && ((*it)->flags & KILL_ON_COLISION || (*it)->flags & BOUNCE_ONCE))
        {
            delete (*it);
            it = world.erase(it);
            it--;
            continue;
        }
        for(list<Element*>::iterator jt=it; jt != world.end(); jt++)
        {
            if(it==jt) continue;
            if(testColisionElements((*it),(*jt)))
            {
                if((*it)->flags & KILL_ON_COLISION)
                {
                    delete (*it);
                    it = world.erase(it);
                    it--;
                    break;
                }
                else if((*it)->flags & BOUNCE_ONCE)
                {
                    (*it)->flags -= BOUNCE_ONCE;
                    (*it)->flags += KILL_ON_COLISION;
                }
                if((*jt)->flags & KILL_ON_COLISION)
                {
                    delete (*jt);
                    jt = world.erase(jt);
                    jt--;
                    continue;
                }
                else if((*jt)->flags & BOUNCE_ONCE)
                {
                    (*jt)->flags -= BOUNCE_ONCE;
                    (*jt)->flags += KILL_ON_COLISION;
                }
            }
        }
    }
}

void gravity()
{
    for(list<Element*>::iterator it=world.begin(); it != world.end(); it++)
    {
        if((*it)->flags & TEST_FOR_GRAVITY)
        {
            (*it)->acc.x = 0.0f;
            (*it)->acc.y = 0.0f;
            for(list<Element*>::iterator jt=world.begin(); jt != world.end(); jt++)
            {
                if(it==jt) continue;
                if(!((*jt)->flags & GENERATE_GRAVITY)) continue;
                float dist2 = ((*it)->pos.y-(*jt)->pos.y)*((*it)->pos.y-(*jt)->pos.y)+((*it)->pos.x-(*jt)->pos.x)*((*it)->pos.x-(*jt)->pos.x);
                (*it)->acc.x += resolution_x*100*(*jt)->mass/dist2*((*jt)->pos.x-(*it)->pos.x);
                (*it)->acc.y += resolution_y*100*(*jt)->mass/dist2*((*jt)->pos.y-(*it)->pos.y);
            }
        }
    }
}

void drawElements()
{
    for(list<Element*>::iterator it=world.begin(); it != world.end(); it++)
    {
        draw((*it));
    }
}
void keyboardManagement(unsigned long _dt)
{
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    float dt = ((float) _dt) / 1000.0f;
    if(state[SDL_SCANCODE_ESCAPE])
    {
        game_close = true;
    }
    if(state[SDL_SCANCODE_UP])
    {
        player1->spd.x += ENGINE_ACC*(resolution_x+resolution_y)/2.0*cos(player1->facing)*dt;
        player1->spd.y += ENGINE_ACC*(resolution_x+resolution_y)/2.0*sin(player1->facing)*dt;
    }
    if(state[SDL_SCANCODE_DOWN])
    {
        player1->spd.x -= ENGINE_ACC*(resolution_x+resolution_y)/2.0*cos(player1->facing)*dt;
        player1->spd.y -= ENGINE_ACC*(resolution_x+resolution_y)/2.0*sin(player1->facing)*dt;
    }
    if(state[SDL_SCANCODE_LEFT])
    {
        player1->facing -= FACING_SPEED*dt;
    }
    if(state[SDL_SCANCODE_RIGHT])
    {
        player1->facing += FACING_SPEED*dt;
    }
    if(state[SDL_SCANCODE_SPACE] && (signed long)time_til_next_shot_p1<=0)
    {
        Element shot = {{player1->pos.x+2*player1->r*cos(player1->facing),player1->pos.y+2*player1->r*sin(player1->facing)},{player1->spd.x+SHOT_SPEED*(resolution_x+resolution_y)/2.0*cos(player1->facing),player1->spd.y+SHOT_SPEED*(resolution_x+resolution_y)/2.0*sin(player1->facing)},{0,0},{0xFF,0x0FF,0xFF,0x00},SHOT_RADIUS*(resolution_x+resolution_y)/2.0,0,CIRCLE,TEST_FOR_GRAVITY | BOUNCE_ONCE | DAMAGES,0};
        Element* nshot = new Element; *nshot = shot;
        world.push_front(nshot);
        time_til_next_shot_p1 = SHOT_RATE;
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
    if((float)x-player2->pos.x!=0)
    {
        if((float)x>player2->pos.x) player2->facing = atan(((float)y-player2->pos.y)/((float)x-player2->pos.x));
        else player2->facing = atan(((float)y-player2->pos.y)/((float)x-player2->pos.x)) + PI;
    }
    if(state & SDL_BUTTON(SDL_BUTTON_RIGHT))
    {
        player2->spd.x += ENGINE_ACC*(resolution_x+resolution_y)/2.0*cos(player2->facing)*dt;
        player2->spd.y += ENGINE_ACC*(resolution_x+resolution_y)/2.0*sin(player2->facing)*dt;
    }
    if(state & SDL_BUTTON(SDL_BUTTON_LEFT) && (signed long)time_til_next_shot_p2<=0)
    {
        Element shot = {{player2->pos.x+2*player2->r*cos(player2->facing),player2->pos.y+2*player2->r*sin(player2->facing)},{player2->spd.x+SHOT_SPEED*(resolution_x+resolution_y)/2.0*cos(player2->facing),player2->spd.y+SHOT_SPEED*(resolution_x+resolution_y)/2.0*sin(player2->facing)},{0,0},{0xFF,0x0FF,0xFF,0x00},SHOT_RADIUS*(resolution_x+resolution_y)/2.0,0,CIRCLE,TEST_FOR_GRAVITY | BOUNCE_ONCE | DAMAGES,0};
        Element* nshot = new Element; *nshot = shot;
        world.push_front(nshot);
        time_til_next_shot_p2 = SHOT_RATE;
    }
}
void displayScore()
{
    SDL_Rect p1 = { 0, 0, score, (int) (TOP_BAR_HEIGHT*(resolution_x+resolution_y)/2.0) };
    SDL_Rect p2 = { score, 0, resolution_x-score, (int) (TOP_BAR_HEIGHT*(resolution_x+resolution_y)/2.0) };
    SDL_SetRenderDrawColor(renderer, player1->rgba[0],player1->rgba[1],player1->rgba[2],player1->rgba[3]);
    SDL_RenderFillRect(renderer, &p1);
    SDL_SetRenderDrawColor(renderer, player2->rgba[0],player2->rgba[1],player2->rgba[2],player2->rgba[3]);
    SDL_RenderFillRect(renderer, &p2);
}

void display()
{
    SDL_SetRenderDrawColor(renderer, 0x00,0x00,0x00,0x00);
    SDL_RenderFillRect(renderer, NULL);
    drawElements(); 
    displayScore();
    SDL_RenderPresent(renderer); 
}

void displayR()
{
    SDL_SetRenderDrawColor(renderer, 0x00,0x00,0x00,0x00);
    SDL_RenderFillRect(renderer, NULL);
    drawChar('R'); 
    displayScore();
    SDL_RenderPresent(renderer);   
}


void checkForR()
{
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if(state[SDL_SCANCODE_R])
    {
        r_pressed = true;
    }
    if(state[SDL_SCANCODE_ESCAPE])
    {
        game_close = true;
    }
}

void resetGame()
{ 
    for(list<Element*>::iterator it=world.begin(); it != world.end(); it++)
    {
        delete (*it);
        it = world.erase(it);
        it--;
        continue;
    }

    time_til_next_shot_p1 = 0;
    time_til_next_shot_p2 = 0;
}

int main (int argc, char* args[])
{
    if(argc!=5)
    { 
        printf("Error: Map Name Missing!!!\nSyntax Expected: <.exe name> <Resolution X> <Resolution Y> <Fullscreen? y/n> <.map path file>\n");
        return 1;
    }

    resolution_x = atoi(args[1]);
    resolution_y = atoi(args[2]);

    unsigned long last_time;
    unsigned long new_time;
    unsigned long time_elapsed;

    err = SDL_Init(SDL_INIT_EVERYTHING);
    assert(err == 0);
    if(strcmp(args[3],"y")==0) window = SDL_CreateWindow("Space Ball", resolution_x/10, resolution_y/10, resolution_x, resolution_y, SDL_WINDOW_FULLSCREEN);
    else window = SDL_CreateWindow("Space Ball",resolution_x/10, resolution_y/10, resolution_x, resolution_y, 0);
    assert(window != NULL);
    renderer = SDL_CreateRenderer(window, -1, 0);
    assert(renderer != NULL);
    SDL_Event e;

    FILE* map;

    while(1)
    {
        last_time = SDL_GetTicks();

        score = resolution_x/2;

        map = fopen(args[4],"r");
        if(map==NULL)
        {
            printf("Error: Map Not Found!!!\n");
            return 1;
        }

        Element _player1 = {{0,0},{0,0},{0,0},P1_COLOR,SHIP_RADIUS*(resolution_x+resolution_y)/2.0,0,SHIP,TEST_FOR_GRAVITY | DESACC_ON_COLISION,0};
        fscanf(map," %f %f",&_player1.pos.x,&_player1.pos.y);
        Element _player2 = {{0,0},{0,0},{0,0},P2_COLOR,SHIP_RADIUS*(resolution_x+resolution_y)/2.0,0,SHIP,TEST_FOR_GRAVITY | DESACC_ON_COLISION,0};
        fscanf(map," %f %f",&_player2.pos.x,&_player2.pos.y);

        _player1.pos.x *= resolution_x;
        _player1.pos.y *= resolution_y;
        _player2.pos.x *= resolution_x;
        _player2.pos.y *= resolution_y;

        player1 = &_player1;
        player2 = &_player2;

        world.push_back(player1);
        world.push_back(player2);

        Element circ = {{0,0},{0,0},{0,0},{0,0,0,0},0,0,CIRCLE,0,0};
        while(fscanf(map," %f %f %f %f %f %f %d %d %d %d %f %f %d",&circ.pos.x,&circ.pos.y,&circ.spd.x,&circ.spd.y,&circ.acc.x,&circ.acc.y,&circ.rgba[0],&circ.rgba[1],&circ.rgba[2],&circ.rgba[3],&circ.r,&circ.mass,&circ.flags)==13)
        {      
            circ.pos.x *= resolution_x;
            circ.pos.y *= resolution_y;
            circ.spd.x *= resolution_x;
            circ.spd.y *= resolution_y;
            circ.acc.x *= resolution_x;
            circ.acc.y *= resolution_y;
            circ.r *= (resolution_x+resolution_y)/2.0f;
            Element* ncirc = new Element;
            *ncirc = circ;
            world.push_back(ncirc);
        }
        fclose(map);

        while (1)
        {
            new_time = SDL_GetTicks();
            time_elapsed = new_time - last_time;

            if(SDL_PollEvent(&e))
            {
                if (e.type == SDL_QUIT)
                {
                    game_close = true;
                    break;
                }
            }

            keyboardManagement(time_elapsed);
            mouseManagement(time_elapsed);
            updatePositions(time_elapsed);
            colision();
            gravity();
            display();
            if((signed long)time_til_next_shot_p1>0) time_til_next_shot_p1-=time_elapsed;
            if((signed long)time_til_next_shot_p2>0) time_til_next_shot_p2-=time_elapsed;
            if(score >= resolution_x || score <= 0)
            {
                break;
            }

            last_time = new_time;
            if(game_close) break;
        }
        if(game_close) break;
        r_pressed = false;
        while(r_pressed == false)
        {
            if(SDL_PollEvent(&e))
            {
                if (e.type == SDL_QUIT) break;
            }
            displayR();
            checkForR();
            if(game_close) break;
        }
        if(game_close) break;
        resetGame();
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}