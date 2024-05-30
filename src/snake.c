#include "SDL2/SDL_error.h"
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_keycode.h"
#include "SDL2/SDL_rect.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_stdinc.h"
#include "SDL2/SDL_timer.h"
#include "SDL2/SDL_video.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "./window.h"
#include "./snake.h"
#include <SDL2/SDL.h>

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Rect food;
SDL_Event event;

typedef struct{
  SDL_Rect *bodies;
  int length;
} Snake;

Snake snake;

int vX = 1;
int vY = 0;

void drawSnake();
void drawFood();
void snakeMovement();
void movesnake();
void windowCollision();
bool checkFoodCollision();
void growSnake();
bool checkBodyCollision();

int main(int argc, char* argv[]){
  int count = 0;
  time_t t;

  if(SDL_Init(SDL_INIT_VIDEO) < 0){
    printf("error initializing SDL: %s\n", SDL_GetError());
  }

  window = SDL_CreateWindow(
      "Snake", 
      SDL_WINDOWPOS_CENTERED, 
      SDL_WINDOWPOS_CENTERED,
      WINDOW_WIDTH,
      WINDOW_HEIGHT,
      0);

  if(!window){
    printf("error creating window: %s\n", SDL_GetError());
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if(!renderer){
    printf("error creating renderer: %s\n", SDL_GetError());
  }
  
  SDL_RenderClear(renderer);
  
  snake.length = SNAKE_INITIAL_STATE;
  snake.bodies = (SDL_Rect *)malloc(snake.length * sizeof(SDL_Rect));
  
  //snake's position and size
  for(int i = 0; i < snake.length; i++){
    snake.bodies[i].x = 0;
    snake.bodies[i].y = i * SNAKE_BODY_SIZE;
    snake.bodies[i].w = snake.bodies[i].h = SNAKE_BODY_SIZE;
  }
  
  //food's random position and size
  srand((unsigned) time(&t));
  food.x = food.y = (rand() % (WINDOW_WIDTH - FOOD_SIZE)) / FOOD_SIZE * FOOD_SIZE;
  food.w = food.h = FOOD_SIZE;

  bool gameRunning = true;
  Uint32 lastMoveTime = SDL_GetTicks();

  while (gameRunning){
    while (SDL_PollEvent(&event) != 0){

      if(event.type == SDL_QUIT){
        gameRunning = false;
      }

      else if(event.type == SDL_KEYDOWN){
        snakeMovement();
      }

    }

    Uint32 currentTime = SDL_GetTicks();
    Uint32 elapsedTime = currentTime - lastMoveTime;
    
    //smooth movement
    if(elapsedTime > (1000 / SNAKE_SPEED)){
      movesnake();
      lastMoveTime = currentTime;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    drawSnake();
    drawFood();
    SDL_RenderPresent(renderer);
    
    windowCollision();

    if(checkFoodCollision()){
      growSnake();
      food.x = food.y = (rand() % (WINDOW_WIDTH - FOOD_SIZE)) / FOOD_SIZE* FOOD_SIZE;
      count++;
    }

    if(checkBodyCollision() == true){
      gameRunning = false;
      printf("Game over! You scored: %d", count);
    }

  }

  free(snake.bodies);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

void drawSnake(){
  SDL_SetRenderDrawColor(renderer, 0, 150, 0, 255);
  for(int i = 0; i < 2; i++){
    SDL_RenderDrawRect(renderer, &snake.bodies[i]);
    SDL_RenderFillRect(renderer, &snake.bodies[i]);
  }
  
  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
  for(int i = 2; i < snake.length; i++){
    SDL_RenderDrawRect(renderer, &snake.bodies[i]);
    SDL_RenderFillRect(renderer, &snake.bodies[i]);
  }
  
}

void drawFood(){
  SDL_SetRenderDrawColor(renderer, 244, 255, 244, 255);
  SDL_RenderDrawRect(renderer, &food);
  SDL_RenderFillRect(renderer, &food);
}

void snakeMovement(){
  switch (event.key.keysym.sym){
    case SDLK_w:
      vX = 0;
      vY = -1;
      break;
    case SDLK_a:
      vX = -1;
      vY = 0;
      break;
    case SDLK_s:
      vX = 0;
      vY = 1;
      break;
    case SDLK_d:
      vX = 1;
      vY = 0;
      break;
  }
}

void movesnake(){
  for(int i = snake.length - 1; i > 0; i--){
    snake.bodies[i] = snake.bodies[i - 1];
  }
  snake.bodies[0].x += vX * SNAKE_BODY_SIZE;
  snake.bodies[0].y += vY * SNAKE_BODY_SIZE;
}

void windowCollision(){
  if(snake.bodies[0].x >= WINDOW_WIDTH){
    snake.bodies[0].x = 0;
  }
  else if (snake.bodies[0].x + SNAKE_BODY_SIZE <= 0) {
    snake.bodies[0].x = WINDOW_WIDTH - SNAKE_BODY_SIZE;
  }
  if(snake.bodies[0].y >= WINDOW_HEIGHT){
    snake.bodies[0].y = 0;
  }
  else if (snake.bodies[0].y + SNAKE_BODY_SIZE <= 0) {
    snake.bodies[0].y = WINDOW_HEIGHT - SNAKE_BODY_SIZE;
  }
}

bool checkFoodCollision(){
  return snake.bodies[0].x < food.x + food.w && snake.bodies[0].x + SNAKE_BODY_SIZE > food.x && snake.bodies[0].y < food.y + food.h &&
    snake.bodies[0].y + SNAKE_BODY_SIZE > food.y;
}

void growSnake(){
  snake.length++;
  snake.bodies = (SDL_Rect *)realloc(snake.bodies, snake.length * sizeof(SDL_Rect));
  snake.bodies[snake.length - 1] = snake.bodies[snake.length - 2];

}

bool checkBodyCollision(){
  for(int i = 1; i < snake.length; i++){
    if(snake.bodies[0].x == snake.bodies[i].x && snake.bodies[0].y == snake.bodies[i].y){
      return true;
    }
  }
  return false;
}
