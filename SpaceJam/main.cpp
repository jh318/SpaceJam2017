#include "SDL.h"
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>

using namespace std;

void Destroy();
void gameOver();

int ScreenX = 800;
int ScreenY = 600;

Mix_Music *gMusic = NULL;

Mix_Chunk *gScratch = NULL;
Mix_Chunk *gBrickBreak = NULL;
Mix_Chunk *gDefenderBreak = NULL;
Mix_Chunk *gPlayerHit = NULL;
Mix_Chunk *gGameOver = NULL;
Mix_Chunk *gBallWall = NULL;
Mix_Chunk *gBrickReset = NULL;

SDL_Window *window;

SDL_Renderer * renderer;

SDL_Surface *background;
SDL_Surface *ball;
SDL_Surface *bat;
SDL_Surface *brick;
SDL_Surface *defender;

SDL_Texture *backgroundTexture;
SDL_Texture *ballTexture;
SDL_Texture *batTexture;
SDL_Texture *brickTexture;
SDL_Texture *defenderTexture;

SDL_Rect brickRect[3][7];
SDL_Rect defenderRect[2][7];
SDL_Rect ballRect;
SDL_Rect batRect;
SDL_Rect backgroundRect;
SDL_Rect dst;

SDL_Event event;

//Text
string scoreText;
TTF_Font *font;
SDL_Color color;
SDL_Surface *textSurface;
SDL_Texture *text;
SDL_Rect textRect;





bool quit = false;
bool startgame = false;
bool resetEnemy = true;
bool gameEnd = false;

int totalScore = 0;

int bgw = ScreenX;
int bgh = ScreenY;
int bgwmin = 0;
int bghmin = 0;

int ballX = bgw/2;	//Determines Start Location
int ballY = bgh - 160; //Determines Start Location
int ballVelX = 3;
int ballVelY = -3;

int batX = bgw / 2;
int batY = bgh - 170;
int batSpeed = 5;

int deleteBrickCount = 0;
int numberOfBricks = 21;

int brickw = 60;
int brickh = 35;

int defenderw = 40;
int defenderh = 35;

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

void InitializeDefenders() {
	int currSpaceX = 90;
	int currSpaceY = ScreenY - 120;

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 7; j++) {
			defenderRect[i][j] = { currSpaceX, currSpaceY, defenderw, defenderh };
			currSpaceX += 100;
		}
		currSpaceY += 50;
		currSpaceX = 40;
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
		if (event.key.keysym.sym == SDLK_SPACE) {
			startgame = true;
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
		Mix_PlayChannel(-1, gBallWall, 0);
	}
	if (ballY < bghmin) {
		ballVelY = -ballVelY;
		Mix_PlayChannel(-1, gBallWall, 0);
	}
	if (ballY > bgh + 60) {
		gameOver();
		//ballVelY = -ballVelY;
	}

	int ballScaling = 20;
	if (ballY + ballScaling >= batY && ballY + ballScaling <= batY + 30 && ballX+ballScaling >= batX && ballX+ballScaling <= batX+60) {
		ballVelY = -ballVelY;
		if(startgame) Mix_PlayChannel(-1, gPlayerHit, 0);
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
	//Mix_PlayChannel(-1, gScratch, 0);
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
				Mix_PlayChannel(-1, gBrickBreak, 0);
			}
		}
	}
}

void resetBricks() {
	int currSpaceX = 50;

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 7; j++) {
			brickRect[i][j].x = currSpaceX;
			currSpaceX += 100;
		}
		currSpaceX = 50;
	}
}

void ballDefenderCollision() {
	bool a;
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 7; j++) {
			a = ballCollisionDetect(defenderRect[i][j], ballRect);
			if (a) {
				defenderRect[i][j].x = 30000;
				ballVelY = -ballVelY;
				Mix_PlayChannel(-1, gDefenderBreak, 0);
			}
		}
	}
}

void Destroy(){
	SDL_DestroyTexture(batTexture);
	SDL_DestroyTexture(brickTexture);
	SDL_DestroyTexture(backgroundTexture);
	SDL_DestroyTexture(ballTexture);
	SDL_DestroyTexture(defenderTexture);
	SDL_FreeSurface(bat);
	SDL_FreeSurface(brick);
	SDL_FreeSurface(background);
	SDL_FreeSurface(ball);
	SDL_FreeSurface(defender);
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
	SDL_Surface *go = SDL_LoadBMP("Assets/Images/destroyedearth.bmp");
	SDL_Texture *goTexture = SDL_CreateTextureFromSurface(renderer, go);
	SDL_Rect goRect = { 0, 0, bgw, bgh };
	Mix_PlayChannel(-1, gGameOver, 0);
	SDL_RenderCopy(renderer, goTexture, NULL, &goRect);
	SDL_RenderCopy(renderer, text, NULL, &textRect);
	gameEnd = true;
	SDL_RenderPresent(renderer);
	SDL_Delay(20000);
	Destroy();
	SDL_Quit();
}

int main(int argc, char * argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
		window = SDL_CreateWindow("The Game", 
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ScreenX, ScreenY, 0);
		renderer = SDL_CreateRenderer(window, -1, 0);

		//Load Audio
		Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
		gScratch = Mix_LoadWAV("Assets/Audio/test.wav");
		gDefenderBreak = Mix_LoadWAV("Assets/Audio/Scream.wav");
		gGameOver = Mix_LoadWAV("Assets/Audio/slowdeath.wav");
		gBrickBreak = Mix_LoadWAV("Assets/Audio/explosion1.wav");
		gPlayerHit = Mix_LoadWAV("Assets/Audio/energy1.wav");
		gBallWall = Mix_LoadWAV("Assets/Audio/explosion2.wav");
		gBrickReset = Mix_LoadWAV("Assets/Audio/evillaugh.wav");

		//Draw Text
		scoreText = "FinalScore: " + std::to_string(totalScore);
		font = TTF_OpenFont("Assets/Fonts/arialnarrow.ttf", 100);
		color = { 255,77,77,255 };
		textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), color);
		text = SDL_CreateTextureFromSurface(renderer, textSurface);
		textRect;
		textRect.x = (ScreenX / 2) - 300;
		textRect.y = (ScreenY / 2) - 200;
		SDL_QueryTexture(text, NULL, NULL, &textRect.w, &textRect.h);

		int frame = 0;
		bool cap = true;
		//SDL_Timer fps;

		//Rect
		backgroundRect = { 0,0, ScreenX, ScreenY };
		InitializeBrick();
		InitializeDefenders();
		//Surface
		background = SDL_LoadBMP("Assets/Images/spacebg.bmp");
		ball = SDL_LoadBMP("Assets/Images/energyball.bmp");
		bat = SDL_LoadBMP("Assets/Images/playership.bmp");
		brick = SDL_LoadBMP("Assets/Images/enemyship2.bmp");
		defender = SDL_LoadBMP("Assets/Images/playership2.bmp");
		//Texture
		backgroundTexture = SDL_CreateTextureFromSurface(renderer, background);
		ballTexture = SDL_CreateTextureFromSurface(renderer, ball);
		batTexture = SDL_CreateTextureFromSurface(renderer, bat);
		brickTexture = SDL_CreateTextureFromSurface(renderer, brick);
		defenderTexture = SDL_CreateTextureFromSurface(renderer, defender);

		SDL_RenderCopy(renderer, backgroundTexture, NULL, &backgroundRect);
		

		while (!quit) {
			SDL_Delay(10);
			EventHandler();
			batRect = { (int)batX, (int)batY, 40, 30 };
			ballRect = { (int)ballX , (int)ballY, 20, 30 };

			if (startgame) { 
				moveBall(); 
			}
			ballCollision();
			ballBrickCollision();
			ballDefenderCollision();
			if (deleteBrickCount == numberOfBricks) {
				totalScore += deleteBrickCount;
				//win();
				deleteBrickCount = 0;
				//resetEnemy = true;
				resetBricks();
				Mix_PlayChannel(-1, gBrickBreak, 0);
			}

			SDL_RenderCopy(renderer, backgroundTexture, NULL, &backgroundRect);
			SDL_RenderCopy(renderer, ballTexture, NULL, &ballRect);
			SDL_RenderCopy(renderer, batTexture, NULL, &batRect);
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 7; j++) {
					SDL_RenderCopy(renderer, brickTexture, NULL, &brickRect[i][j]);
				}
			}

			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < 7; j++) {
					SDL_RenderCopy(renderer, defenderTexture, NULL, &defenderRect[i][j]);
				}
			}
			SDL_RenderPresent(renderer);
			SDL_RenderClear(renderer);
		}
		SDL_Quit();


	return 0;
}