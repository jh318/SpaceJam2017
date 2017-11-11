#include "SDL.h"

int ScreenX = 800;
int ScreenY = 600;

bool quit = false;
SDL_Event event;

float ballX = 10;
float ballY = 10;
float ballVelX = 0.05;
float ballVelY = 0.05;

int bgw = ScreenX;
int bgh = ScreenY;
int bgwmin = 0;
int bghmin = 0;

float batX = bgw / 2;
float batY = bgh - 30;
float batSpeed = 0.05;

int brickw = 80;
int brickh = 35;
SDL_Surface *brick;
SDL_Texture *brickTexture;
SDL_Rect brickRect[3][7];

void InitializeBrick() {
	//brickRect[0][0] = { 50,50,brickw, brickh };
	//brickRect[0][1] = { 150, 50, brickw, brickh };
	//brickRect[0][2] = { 250, 50, brickw, brickh };
	//brickRect[0][3] = { 350, 50, brickw, brickh };
	//brickRect[0][4] = { 450, 50, brickw, brickh };
	//brickRect[0][5] = { 550, 50, brickw, brickh };
	//brickRect[0][2] = { 650, 50, brickw, brickh };
	
	
	int currSpaceX = 50;
	int currSpaceY = 50;
	
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 7; j++) {
			brickRect[i][j] = { currSpaceX, currSpaceY, brickw, brickh };
			currSpaceX += 100;
		}
		currSpaceY += 100;
		currSpaceX = 50;
	}
}

void EventHandler() {
	SDL_PollEvent(&event);
	if (event.type == SDL_QUIT) {
		quit = true;
	}
	else if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT && batX > 0) {
			batX -= batSpeed;
		}
		if (event.key.keysym.sym == SDLK_RIGHT && batX < bgw - 60) {
			batX += batSpeed;
		}
	}

}

void moveBall() {
	ballX = ballX + ballVelX;
	ballY = ballY + ballVelY;

}

void ballCollision() {
	if (ballX < bgwmin || ballX > bgw - 30) {
		ballVelX = -ballVelX;
	}
	if (ballY < bghmin || ballY > bgh - 30) {
		ballVelY = -ballVelY;
	}

	int ballScaling = 20;
	if (ballY + ballScaling >= batY && ballY + ballScaling <= batY + 30 && ballX+ballScaling >= batX && ballX+ballScaling <= batX+60) {
		ballVelY = -ballVelY;
	}
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);
		SDL_Window *window = SDL_CreateWindow("The Game", 
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ScreenX, ScreenY, 0);
		SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
		SDL_Rect backgroundRect = { 0,0, ScreenX, ScreenY };
		SDL_Surface *background = SDL_LoadBMP("Assets/Images/bk.bmp");
		SDL_Surface *ball = SDL_LoadBMP("Assets/Images/ball.bmp");
		SDL_Surface *bat = SDL_LoadBMP("Assets/Images/bat.bmp");
		brick = SDL_LoadBMP("Assets/Images/brick.bmp");

		SDL_Texture *backgroundTexture = SDL_CreateTextureFromSurface(renderer, background);
		SDL_Texture *ballTexture = SDL_CreateTextureFromSurface(renderer, ball);
		SDL_Texture *batTexture = SDL_CreateTextureFromSurface(renderer, bat);
		brickTexture = SDL_CreateTextureFromSurface(renderer, brick);

		SDL_RenderCopy(renderer, backgroundTexture, NULL, &backgroundRect);
		//Render Bricks
		

		while (!quit) {
			EventHandler();
			SDL_Rect ballRect = { ballX,ballY,20,30 };
			SDL_Rect batRect = { batX, batY, 60, 30 };
			moveBall();
			ballCollision();
			InitializeBrick();
			SDL_RenderCopy(renderer, backgroundTexture, NULL, &backgroundRect);
			SDL_RenderCopy(renderer, ballTexture, NULL, &ballRect);
			SDL_RenderCopy(renderer, batTexture, NULL, &batRect);
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 7; j++) {
					SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[i][j]);
				}
			}
			SDL_RenderPresent(renderer);
			SDL_RenderClear(renderer);

		}
		
		SDL_Quit();


	return 0;
}