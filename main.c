#define SDL_MAIN_HANDLED
#include <stdio.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>

//Game Constants
const int SCREEN_WIDTH = 1366;
const int SCREEN_HEIGHT = 768;
const int PADDLE_HEIGHT = 20;
const int PADDLE_WIDTH = 100;
const int BALL_SIZE = 15;
int PADDLE_SPEED = 15;
int BALL_SPEED = 9;
const float BALL_SPEEDCAP = 14;
typedef struct 
{
    float x,y;
    int w,h;
    float vel;
} Paddle;
typedef struct 
{
    float x,y;
    int w,h;
    float vel_x,vel_y;
} Ball;
//Game Objects
Paddle leftPaddle;
Paddle rightPaddle;
Ball ball;
int leftScore = 0;
int rightScore = 0;
//SDL globals
SDL_Window *window;
SDL_Renderer *renderer; 
TTF_Font *font;
//reset the ball
void resetBall()
{
    ball.x = (SCREEN_WIDTH - BALL_SIZE) / 2;
    ball.y = (SCREEN_HEIGHT - BALL_SIZE) / 2;
    ball.vel_x = BALL_SPEED * (rand() % 2 ? 1 : -1);
    ball.vel_y = BALL_SPEED * (rand() % 2 ? 1 : -1);
}
//Update Paddles
void updatePaddles()
{
    //left paddle
    leftPaddle.y += leftPaddle.vel;
    if(leftPaddle.y < 0) leftPaddle.y = 0;
    if(leftPaddle.y + leftPaddle.h > SCREEN_HEIGHT)
        leftPaddle.y = SCREEN_HEIGHT - leftPaddle.h;
    //right paddle
    rightPaddle.y += rightPaddle.vel;
    if(rightPaddle.y < 0) rightPaddle.y = 0;
    if(rightPaddle.y + rightPaddle.h > SCREEN_HEIGHT)
        rightPaddle.y = SCREEN_HEIGHT - rightPaddle.h;
}
//update ball
void updateBall()
{
    ball.x -= ball.vel_x;
    ball.y -= ball.vel_y;
    //Wall collision
    if(ball.y < 0 | ball.y + ball.h > SCREEN_HEIGHT)
        ball.vel_y *= -1;
    //Paddle collision
    SDL_Rect ballRect = {(int)ball.x,(int)ball.y,ball.w,ball.h};
    SDL_Rect leftRect = {(int)leftPaddle.x,(int)leftPaddle.y,leftPaddle.h,leftPaddle.w};
    SDL_Rect rightRect = {(int)rightPaddle.x,(int)rightPaddle.y,rightPaddle.h,rightPaddle.w};

    if(SDL_HasIntersection(&ballRect, &leftRect) | SDL_HasIntersection(&ballRect, &rightRect))
    {
        ball.vel_x *= -1;
    }   
    //Score points
    if(ball.x < 0)
    {
        rightScore++;
        resetBall();
    }
    if(ball.x + ball.w > SCREEN_WIDTH)
    {
        leftScore++;
        resetBall();
    }
    
}
SDL_Texture *createTextTexture(const char* text, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderText_Solid(font,text,color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer,surface);
    SDL_FreeSurface(surface);
    return texture;
}
//render
void render()
{
    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    SDL_RenderClear(renderer);
    //Draw paddles and ball
    SDL_SetRenderDrawColor(renderer,255,255,255,255);
    SDL_RenderFillRect(renderer,&(SDL_Rect){leftPaddle.x,leftPaddle.y,leftPaddle.h,leftPaddle.w});
    SDL_RenderFillRect(renderer,&(SDL_Rect){rightPaddle.x,rightPaddle.y,rightPaddle.h,rightPaddle.w});
    SDL_RenderFillRect(renderer,&(SDL_Rect){ball.x,ball.y,ball.h,ball.w});
    //Draw Scores variables
    SDL_Color white = {255,255,255,255};
    char scoreStr[16];
    //left score
    sprintf(scoreStr,"%d",leftScore);
    SDL_Texture *leftText = createTextTexture(scoreStr,white);
    int w,h;
    SDL_QueryTexture(leftText,NULL,NULL,&w,&h);
    SDL_RenderCopy(renderer,leftText,NULL,&(SDL_Rect){SCREEN_HEIGHT/4 - w/2,20,w,h});
    SDL_DestroyTexture(leftText);
    //right score
    sprintf(scoreStr,"%d",rightScore);
    SDL_Texture *rightText = createTextTexture(scoreStr,white);
    SDL_QueryTexture(rightText,NULL,NULL,&w,&h);
    SDL_RenderCopy(renderer,rightText,NULL,&(SDL_Rect){3*SCREEN_WIDTH/4 - w/2,20,w,h});
    SDL_DestroyTexture(rightText);
    SDL_RenderPresent(renderer); //Update the screen with any rendering performed since the previous call.
}
//Cleans up the whole thing
void cleanUp()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}
//main function
int main()
{
    //Initialization of SDl
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL init error: %s\n", SDL_GetError());
        exit(1);
    }
    //TTF init
    if(TTF_Init() == -1)
    {
        fprintf(stderr,"TTF init error: %s\n", TTF_GetError());
        exit(1);
    }
    //create window
    window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
    //window error check
    if(!window)
    {
        fprintf(stderr,"window create error: %s\n",SDL_GetError());
        return 1;
    }
    //create renderer
    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    //error check
    if(!renderer)
    {
        fprintf(stderr,"renderer create error: %s\n",SDL_GetError());
        return 1;
    }
    //get font
    font = TTF_OpenFont("fonts/Arial.ttf",24);
    if (!font)
    {
        fprintf(stderr,"Font load error %s\n", TTF_GetError());
        return 1;
    }
    //Initialize left paddle
    leftPaddle.w = PADDLE_WIDTH;
    leftPaddle.h = PADDLE_HEIGHT;
    leftPaddle.x = 20;
    leftPaddle.y = (SCREEN_HEIGHT - PADDLE_HEIGHT) / 2;
    //now right paddle
    rightPaddle.w = PADDLE_WIDTH;
    rightPaddle.h = PADDLE_HEIGHT;
    rightPaddle.x = SCREEN_WIDTH - 20 - PADDLE_WIDTH;
    rightPaddle.y = (SCREEN_HEIGHT - PADDLE_HEIGHT) / 2;
    //init the ball
    ball.w = BALL_SIZE;
    ball.h = BALL_SIZE;
    resetBall(); //resets the ball
    //Main loop
    bool running = true;
    SDL_Event event;
    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                running = false;
            }
            //Handle Input
            if(event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_w:leftPaddle.vel = -PADDLE_SPEED;break;
                case SDLK_s:leftPaddle.vel = PADDLE_SPEED;break;
                case SDLK_UP:rightPaddle.vel = -PADDLE_SPEED;break;
                case SDLK_DOWN:rightPaddle.vel = PADDLE_SPEED;break;
                case SDLK_ESCAPE:running = false;break;
                }
            }
            if (event.type == SDL_KEYUP)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_w:
                case SDLK_s:leftPaddle.vel = 0;break;
                case SDLK_UP:
                case SDLK_DOWN:rightPaddle.vel = 0;break;
                }
            } 
        }
        //clear screen
        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderClear(renderer);
        updateBall();
        updatePaddles();
        render();
        SDL_Delay(16);
    }
    cleanUp(); //cleans up   
}