#include "SDL.h"

void Destroy();
void gameOver();

int ScreenX = 800;
int ScreenY = 600;

SDL_Window *window;

SDL_Renderer * renderer;

SDL_Surface *background;
SDL_Surface *ball;
SDL_Surface *bat;
SDL_Surface *brick;

SDL_Texture *backgroundTexture;
SDL_Texture *ballTexture;
SDL_Texture *batTexture;
SDL_Texture *brickTexture;

SDL_Rect brickRect[3][7];
SDL_Rect ballRect;
SDL_Rect batRect;
SDL_Rect backgroundRect;

SDL_Event event;

bool quit = false;
int FRAMES_PER_SECOND = 60;

int ballX = 10;	//Determines Start Location
int ballY = 300; //Determines Start Location
int ballVelX = 1;
int ballVelY = 1;

int bgw = ScreenX;
int bgh = ScreenY;
int bgwmin = 0;
int bghmin = 0;

int batX = bgw / 2;
int batY = bgh - 30;
int batSpeed = 1;

int deleteBrickCount = 0;
int numberOfBricks = 21;

int brickw = 80;
int brickh = 35;



void InitializeBrick() {
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
	if (ballY < bghmin) {
		ballVelY = -ballVelY;
	}
	if (ballY > bgh + 60) {
		gameOver();
	}

	int ballScaling = 20;
	if (ballY + ballScaling >= batY && ballY + ballScaling <= batY + 30 && ballX+ballScaling >= batX && ballX+ballScaling <= batX+60) {
		ballVelY = -ballVelY;
	}
}

bool ballCollisionDetect(SDL_Rect rect1, SDL_Rect rect2) {
	if (rect1.x > rect2.x + rect2.w) {
		return false;
	}
	if (rect1.x + rect1.w < rect2.x) {
		return false;
	}
	if (rect1.y > rect2.y + rect2.h) {
		return false;
	}
	if (rect1.y + rect1.h < rect2.y) {
		return false;
	}
	return true;
}	

void ballBrickCollision() {
	bool a;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 7; j++) {
			a = ballCollisionDetect(brickRect[i][j], ballRect);
			if (a) {
				brickRect[i][j].x = 30000;
				ballVelY = -ballVelY;
				deleteBrickCount++;
			}
		}
	}
}

void Destroy(){
	SDL_DestroyTexture(batTexture);
	SDL_DestroyTexture(brickTexture);
	SDL_DestroyTexture(backgroundTexture);
	SDL_DestroyTexture(ballTexture);
	SDL_FreeSurface(bat);
	SDL_FreeSurface(brick);
	SDL_FreeSurface(background);
	SDL_FreeSurface(ball);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	
}

void win() {
	SDL_Surface *win = SDL_LoadBMP("Assets/Images/win.bmp");
	SDL_Texture *winTexture = SDL_CreateTextureFromSurface(renderer, win);
	SDL_Rect winRect = { 250, 200, 350, 350 };
	SDL_RenderCopy(renderer, winTexture, NULL, &winRect);
	SDL_RenderPresent(renderer);
	SDL_Delay(10000);
	Destroy();
	SDL_Quit;
}

void gameOver() {
	SDL_Surface *go = SDL_LoadBMP("Assets/Images/win.bmp");
	SDL_Texture *goTexture = SDL_CreateTextureFromSurface(renderer, go);
	SDL_Rect goRect = { 0, 0, bgw, bgh };
	SDL_RenderCopy(renderer, goTexture, NULL, &goRect);
	SDL_RenderPresent(renderer);
	SDL_Delay(20000);
	Destroy();
	SDL_Quit();
}

int main(int argc, char * argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);
		window = SDL_CreateWindow("The Game", 
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ScreenX, ScreenY, 0);
		renderer = SDL_CreateRenderer(window, -1, 0);

		int frame = 0;
		bool cap = true;
		//SDL_Timer fps;

		//Rect
		backgroundRect = { 0,0, ScreenX, ScreenY };
		InitializeBrick();
		//Surface
		background = SDL_LoadBMP("Assets/Images/bk.bmp");
		ball = SDL_LoadBMP("Assets/Images/ball.bmp");
		bat = SDL_LoadBMP("Assets/Images/bat.bmp");
		brick = SDL_LoadBMP("Assets/Images/brick.bmp");
		//Texture
		backgroundTexture = SDL_CreateTextureFromSurface(renderer, background);
		ballTexture = SDL_CreateTextureFromSurface(renderer, ball);
		batTexture = SDL_CreateTextureFromSurface(renderer, bat);
		brickTexture = SDL_CreateTextureFromSurface(renderer, brick);

		SDL_RenderCopy(renderer, backgroundTexture, NULL, &backgroundRect);

		while (!quit) {
			SDL_Delay(1);
			EventHandler();
			batRect = { (int)batX, (int)batY, 60, 30 };
			ballRect = { (int)ballX , (int)ballY, 20, 30 };
			moveBall();
			ballCollision();
			ballBrickCollision();
			if (deleteBrickCount == numberOfBricks) {
				win();
			}
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