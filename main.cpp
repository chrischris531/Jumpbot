/*This source code copyrighted by Lazy Foo' Productions (2004-2022)
and may not be redistributed without written permission.*/

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>
#include <conio.h>
#include <iomanip>
#include <stdio.h>
#include <vector>
#include <sstream>
#include <time.h>

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 480;
const int PADDING = 10;

class LTexture {
public:
	LTexture();
	~LTexture();
	bool loadFromFile(std::string path);
#if defined(SDL_TTF_MAJOR_VERSION)
	bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
#endif
	void free();
	void setColor(Uint8 red, Uint8 green, Uint8 blue);
	void setBlendMode(SDL_BlendMode blending);
	void setAlpha(Uint8 alpha);
	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
	int getWidth();
	int getHeight();

private:
	int mWidth;
	int mHeight;
	SDL_Texture* mTexture;
};

class LTimer {
public:
	LTimer();
	void start();
	void stop();
	Uint32 getTicks();
	bool isStarted();

private:
	Uint32 mStartTicks;
	bool mStarted;
};

enum POWERUP_TYPE {
	POINT,
	SPEED,
	INVIS,
	DOUBLE,
	POWERUP_TOTAL,
};
const int DURATIONS[POWERUP_TOTAL] = { 0, 3000, 3000, 3000 };
class Powerup {
public:
	static const int POWERUP_WIDTH = 20;
	static const int POWERUP_HEIGHT = 20;
	Powerup(POWERUP_TYPE type);
	void render();
	SDL_Rect getCollider();
	void move();
	Uint32 getTimeDifference();
	Uint32 getLastTick();
	void setLastTick();

private:
	int mType;
	int mPosX, mPosY;
	Uint32 mLastTick;
};

class Dot {
public:
	static const int DOT_WIDTH = 20;
	static const int DOT_HEIGHT = 20;
	static const int GRAVITY = 30;
	static const int JUMP_VEL = -10;
	Dot(int x, int y);
	void handleEvent(SDL_Event& e, LTimer* timer);
	void move(SDL_Rect& wall, Powerup* point, Powerup* speed, Powerup* invis, Powerup* dbl, LTimer* timer, bool& quit);
	void render();
	int getScore();

private:
	int mPosX, mPosY;
	int mVelX, mVelY;
	int mInitVelY;
	bool mOrientation; // 1 is normal, 0 is upside-down
	SDL_Rect mCollider;
	int mScore;
	int mMultiplier;
	int mHorizontalVel;
	double mAngle;
};

class Obstacle {
public:
	static const int OBSTACLE_WIDTH = 50;
	static const int OBSTACLE_HEIGHT = 50;
	Obstacle();
	SDL_Rect getObstacle();
	void move(LTimer* timer);
	void render();

private:
	int mPosX, mPosY;
	int mVelX, mVelY;
	SDL_Rect mObstacle;
};

bool init();
bool loadMedia();
void close();
bool checkCollision(SDL_Rect a, SDL_Rect b);

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
TTF_Font* gFont = NULL;	

LTexture gScoreTextTexture;
LTexture gDoubleTextTexture;
LTexture gSpeedTextTexture;
LTexture gInvisTextTexture;

LTexture gDotTexture;
LTexture gPointTexture;
LTexture gSpeedTexture;
LTexture gInvisTexture;
LTexture gDoubleTexture;

LTexture::LTexture() {
	mWidth = 0;
	mHeight = 0;
	mTexture = NULL;
}
LTexture::~LTexture() {
	free();
}
bool LTexture::loadFromFile(std::string path) {
	free();
	SDL_Texture* newTexture = NULL;
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL) {
		printf("Img load error: %s\n", IMG_GetError());
	}
	else {
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL) {
			printf("Create texture error: %s\n", SDL_GetError());
		}
		else {
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}
		SDL_FreeSurface(loadedSurface);
	}

	mTexture = newTexture;
	return mTexture != NULL;
}

#if defined(SDL_TTF_MAJOR_VERSION)
bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor) {
	free();
	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
	if (textSurface != NULL) {
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL) {
			printf("Create texture error: %s\n", SDL_GetError());
		}
		else {
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}
		SDL_FreeSurface(textSurface);
	}
	else {
		printf("Render text error: %s\n", TTF_GetError());
	}
	return mTexture != NULL;
}
#endif

void LTexture::free() {
	if (mTexture != NULL) {
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}
void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue) {
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}
void LTexture::setBlendMode(SDL_BlendMode blending) {
	SDL_SetTextureBlendMode(mTexture, blending);
}
void LTexture::setAlpha(Uint8 alpha) {
	SDL_SetTextureAlphaMod(mTexture, alpha);
}
void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip) {
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };
	if (clip != NULL) {
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}
int LTexture::getWidth() {
	return mWidth;
}
int LTexture::getHeight() {
	return mHeight;
}

LTimer::LTimer() {
	// initialise variables
	mStartTicks = 0;

	mStarted = false;
}
void LTimer::start() {
	// start timer
	mStarted = true;

	// get current clock time
	mStartTicks = SDL_GetTicks();
}
void LTimer::stop() {
	// stop the timer
	mStarted = false;

	// clear tick variables
	mStartTicks = 0;
}
Uint32 LTimer::getTicks() {
	// the actual timer time
	Uint32 time = 0;

	// if the timer is running
	if (mStarted) {
		// return the current time minus the start time
		time = SDL_GetTicks() - mStartTicks;
	}
	
	return time;
}
bool LTimer::isStarted() {
	// timer is running
	return mStarted;
}

Dot::Dot(int x, int y) {
	mPosX = x;
	mPosY = y;
	mCollider.w = DOT_WIDTH;
	mCollider.h = DOT_HEIGHT;
	mVelX = 0;
	mVelY = 0;
	mInitVelY = 0;
	mOrientation = 1;
	mScore = 0;
	mHorizontalVel = 5;
	mMultiplier = 1;
	mAngle = 0.0;
}
void Dot::handleEvent(SDL_Event& e, LTimer* timer) {
	if (e.type == SDL_KEYDOWN) {
		if (mInitVelY == 0) {
			switch (e.key.keysym.sym) {
			case SDLK_UP:
				mInitVelY = JUMP_VEL;
				timer->start();
				mOrientation = 1;
				break;
			case SDLK_DOWN:
				mInitVelY = JUMP_VEL;
				timer->start();
				mOrientation = 0;
			}
		}

		if (e.key.repeat == 0) {
			switch (e.key.keysym.sym) {
			case SDLK_RIGHT:
				mVelX += mHorizontalVel;
				break;
			case SDLK_LEFT:
				mVelX -= mHorizontalVel;
				break;
			}
		}
	}
	else if (e.type == SDL_KEYUP) {
		switch (e.key.keysym.sym) {
		case SDLK_LEFT:
			mVelX += mHorizontalVel;
			break;
		case SDLK_RIGHT:
			mVelX -= mHorizontalVel;
			break;
		}
	}
}
void Dot::move(SDL_Rect& obstacle, Powerup* point, Powerup* speed, Powerup* invis, Powerup* dbl, LTimer* timer, bool& quit) {
	mVelY = mInitVelY + GRAVITY * timer->getTicks() / 1000;
	if (!mOrientation) {
		mVelY *= -1;
	}

	// move the dot up or down
	mPosX += mVelX;
	mCollider.x = mPosX;
	mPosY += mVelY;
	mCollider.y = mPosY;

	// if collided with obstacle
	if (checkCollision(mCollider, obstacle)) {
		if (invis->getTimeDifference() > DURATIONS[INVIS]) {
			quit = true;
		}
	}

	// if over point
	SDL_Rect pointCollider = point->getCollider();
	if (checkCollision(mCollider, pointCollider)) {
		// move point
		point->move();
		// increase score
		int amount = mMultiplier;
		if (dbl->getTimeDifference() < DURATIONS[DOUBLE] && dbl->getLastTick() != 0) {
			amount *= 2;
		}
		mScore += amount;
	}

	// if over speed
	SDL_Rect speedCollider = speed->getCollider();
	if (checkCollision(mCollider, speedCollider)) {
		// move speed
		speed->move();
		speed->setLastTick();
		if (abs(mHorizontalVel) == 5) {
			mHorizontalVel += 5;

			if (mVelX != 0) {
				if (mVelX > 0) {
					mVelX += 5;
				}
				else {
					mVelX -= 5;
				}
			}
		}
	}
	else {
		// reset speed 
		if (speed->getTimeDifference() > DURATIONS[SPEED] && mHorizontalVel != 5) {
			mHorizontalVel = 5;

			if (mVelX != 0) {
				if (mVelX > 0) {
					mVelX -= 5;
				}
				else {
					mVelX += 5;
				}
			}
		}
	}

	SDL_Rect invisCollider = invis->getCollider();
	if (checkCollision(mCollider, invisCollider)) {
		// move invis
		invis->move();
		invis->setLastTick();
	}

	SDL_Rect doubleCollider = dbl->getCollider();
	if (checkCollision(mCollider, doubleCollider)) {
		dbl->move();
		dbl->setLastTick();
	}

	if ((mOrientation && mPosY > SCREEN_HEIGHT / 2) || (!mOrientation && mPosY < SCREEN_HEIGHT / 2)) {
		mInitVelY = 0;
		mPosY = SCREEN_HEIGHT / 2;
		timer->stop();
	}
	
	if (mInitVelY == 0) {
		if (mAngle != 0) {
			mAngle = 0;
		}
	}
	else {
		if (mVelX >= 0) {
			if (mOrientation) {
				mAngle = (mInitVelY - mVelY) * 180 / mInitVelY;
			}
			else {
				mAngle = -1 * (mInitVelY - mVelY) * 180 / mInitVelY;
			}
		}
		else {
			if (mOrientation) {
				mAngle = -1 * (mInitVelY - mVelY) * 180 / mInitVelY;
			}
			else {
				mAngle = -1 * (mInitVelY - mVelY) * 180 / mInitVelY;
			}
		}
	}
	
}
void Dot::render() {
	gDotTexture.render(mPosX, mPosY, NULL, mAngle);
}
int Dot::getScore() {
	return mScore;
}

Obstacle::Obstacle() {
	mPosX = SCREEN_WIDTH;
	mPosY = (SCREEN_HEIGHT / 4) + rand() % (SCREEN_HEIGHT / 2);
	mObstacle.w = (OBSTACLE_WIDTH / 2) + (rand() % (OBSTACLE_WIDTH / 2));
	mObstacle.h = (OBSTACLE_HEIGHT / 2) + (rand() % (OBSTACLE_HEIGHT / 2));
	mObstacle = { mPosX, mPosY, mObstacle.w, mObstacle.h};
	mVelX = 0;
	mVelY = 0;
}
SDL_Rect Obstacle::getObstacle() {
	return mObstacle;
}
void Obstacle::move(LTimer* timer) {
	mVelX = 10 + timer->getTicks() / 10000;
	if (mVelX > 20) {
		mVelX = 20;
	}
	mPosX -= mVelX;

	if (mPosX < OBSTACLE_WIDTH * -1) {
		mPosY = (SCREEN_HEIGHT / 4) + rand() % (SCREEN_HEIGHT / 2);
		mObstacle.w = (OBSTACLE_WIDTH / 2) + (rand() % (OBSTACLE_WIDTH / 2));
		mObstacle.h = (OBSTACLE_HEIGHT / 2) + (rand() % (OBSTACLE_HEIGHT / 2));
		mPosX = SCREEN_WIDTH;
	}
}
void Obstacle::render() {
	mObstacle.x = mPosX;
	mObstacle.y = mPosY;

	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderDrawRect(gRenderer, &mObstacle);
}

void Powerup::move() {
	mPosX = rand() % SCREEN_WIDTH;
	mPosY = SCREEN_HEIGHT / 4 + (rand() % SCREEN_HEIGHT) / 2;
}
Powerup::Powerup(POWERUP_TYPE type) {
	move();
	mType = type;
	mLastTick = 0;
}
void Powerup::render() {
	switch (mType) {
	case POINT:
		gPointTexture.render(mPosX, mPosY);
		break;
	case SPEED:
		gSpeedTexture.render(mPosX, mPosY);
		break;
	case INVIS:
		gInvisTexture.render(mPosX, mPosY);
		break;
	case DOUBLE:
		gDoubleTexture.render(mPosX, mPosY);
		break;
	}
}
SDL_Rect Powerup::getCollider() {
	return { mPosX, mPosY, POWERUP_WIDTH, POWERUP_HEIGHT };
}
Uint32 Powerup::getTimeDifference() {
	return SDL_GetTicks() - mLastTick;
}
Uint32 Powerup::getLastTick() {
	return mLastTick;
}
void Powerup::setLastTick() {
	mLastTick = SDL_GetTicks();
}

bool init() {
	bool success = true;
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Init error: %s\n", SDL_GetError());
		success = false;
	}
	else {
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
			printf("Warning: linear texture filtering not enabled");
		}

		gWindow = SDL_CreateWindow("Unnamed game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL) {
			printf("Create window error: %s\n", SDL_GetError());
			success = false;
		}
		else {
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL) {
				printf("Create renderer error: %s\n", SDL_GetError());
				success = false;
			}
			else {
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags)) {
					printf("Img init error: %s\n", SDL_GetError());
					success = false;
				}
				if (TTF_Init() == -1) {
					printf("TTF init error: %s\n", TTF_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia() {
	bool success = true;

	if (!gDotTexture.loadFromFile("game_1/dot.bmp")) {
		printf("Load dot texture error\n");
		success = false;
	}

	if (!gPointTexture.loadFromFile("game_1/point.bmp")) {
		printf("Load point texture error\n");
		success = false;
	}

	if (!gSpeedTexture.loadFromFile("game_1/speed.bmp")) {
		printf("Load speed texture error\n");
		success = false;
	}

	if (!gInvisTexture.loadFromFile("game_1/invis.bmp")) {
		printf("Load invis texture error\n");
		success = false;
	}

	if (!gDoubleTexture.loadFromFile("game_1/double.bmp")) {
		printf("Load double texture error\n");
		success = false;
	}

	gFont = TTF_OpenFont("game_1/lazy.ttf", 28);
	if (gFont == NULL) {
		printf("Load font error: %s\n", TTF_GetError());
		success = false;
	}

	return success;
}

void close() {
	gDotTexture.free();
	gPointTexture.free();
	gSpeedTexture.free();
	gInvisTexture.free();
	gDoubleTexture.free();

	gScoreTextTexture.free();
	gSpeedTextTexture.free();
	gDoubleTexture.free();
	gInvisTextTexture.free();


	TTF_CloseFont(gFont);
	gFont = NULL;

	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gRenderer = NULL;
	gWindow = NULL;

	IMG_Quit();
	SDL_Quit();
	TTF_Quit();
}

bool checkCollision(SDL_Rect a, SDL_Rect b) {
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	leftA = a.x;
	rightA = a.x + a.w;
	topA = a.y;
	bottomA = a.y + a.h;

	leftB = b.x;
	rightB = b.x + b.w;
	topB = b.y;
	bottomB = b.y + b.h;

	// if any of the sides from A outside B
	if (bottomA <= topB) return false;
	if (topA >= bottomB) return false;
	if (rightA <= leftB) return false;
	if (leftA >= rightB) return false;

	return true;
}

int main(int argc, char* args[]) {
	if (!init()) {
		printf("Init error\n");
	}
	else {
		if (!loadMedia()) {
			printf("Load media error\n");
		}
		else {
			std::srand(time(0));
			bool quit = false;
			bool gameOver = false;
			SDL_Event e;

			Dot dot(20, SCREEN_HEIGHT / 2);

			LTimer dotTimer;
			LTimer gameTimer;

			Obstacle obstacle;

			Powerup point(POINT);
			Powerup speed(SPEED);
			Powerup invis(INVIS);
			Powerup dbl(DOUBLE);

			gameTimer.start();

			SDL_Color textColor = { 0,0,0,255 };

			std::stringstream scoreText;
			std::stringstream speedText;
			std::stringstream doubleText;
			std::stringstream invisText;
			while (!quit) {
				while (SDL_PollEvent(&e)) {
					if (e.type == SDL_KEYDOWN) {
						switch(e.key.keysym.sym) {
						case SDLK_q:
							quit = true;
							break;
						case SDLK_ESCAPE:
							quit = true;
							break;
						case SDLK_r:
							gameOver = false;
							break;
						}
					}
				}

				while (!gameOver) {
					while (SDL_PollEvent(&e)) {
						if (e.type == SDL_QUIT) {
							gameOver = true;
							quit = true;
						}
						else if (e.type == SDL_KEYDOWN) {
							if (e.key.keysym.sym == SDLK_ESCAPE) {
								gameOver = true;
								quit = true;
							}
						}
						dot.handleEvent(e, &dotTimer);
					}
					SDL_Rect wall = obstacle.getObstacle();
					dot.move(wall, &point, &speed, &invis, &dbl, &dotTimer, gameOver);

					SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
					SDL_RenderClear(gRenderer);

					SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
					SDL_Rect doubleBar = { SCREEN_WIDTH - 100 - PADDING, PADDING, 100, gDoubleTextTexture.getHeight() };
					SDL_Rect speedBar = { SCREEN_WIDTH - 100 - PADDING, PADDING + gDoubleTextTexture.getHeight(), 100, gSpeedTextTexture.getHeight() };
					SDL_Rect invisBar = { SCREEN_WIDTH - 100 - PADDING, PADDING + gDoubleTextTexture.getHeight() + gSpeedTextTexture.getHeight(), 100, gInvisTextTexture.getHeight() };
					SDL_RenderDrawRect(gRenderer, &doubleBar);
					SDL_RenderDrawRect(gRenderer, &speedBar);
					SDL_RenderDrawRect(gRenderer, &invisBar);

					SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0x00);
					SDL_Rect doubleFill = { SCREEN_WIDTH - PADDING - 100 * (DURATIONS[DOUBLE] - dbl.getTimeDifference()) / DURATIONS[DOUBLE], PADDING, 100 * (DURATIONS[DOUBLE] - dbl.getTimeDifference()) / DURATIONS[DOUBLE], gDoubleTextTexture.getHeight() };
					if (dbl.getTimeDifference() < DURATIONS[DOUBLE] && dbl.getLastTick() != 0) {
						SDL_RenderDrawRect(gRenderer, &doubleFill);
					}
					SDL_Rect speedFill = { SCREEN_WIDTH - PADDING - 100 * (DURATIONS[SPEED] - speed.getTimeDifference()) / DURATIONS[SPEED], PADDING + gDoubleTextTexture.getHeight(), 100 * (DURATIONS[SPEED] - speed.getTimeDifference()) / DURATIONS[SPEED], gSpeedTextTexture.getHeight() };
					if (speed.getTimeDifference() < DURATIONS[SPEED] && speed.getLastTick() != 0) {
						SDL_RenderDrawRect(gRenderer, &speedFill);
					}
					SDL_Rect invisFill = { SCREEN_WIDTH - PADDING - 100 * (DURATIONS[INVIS] - invis.getTimeDifference()) / DURATIONS[INVIS], PADDING + gDoubleTextTexture.getHeight() + gSpeedTextTexture.getHeight(), 100 * (DURATIONS[INVIS] - invis.getTimeDifference()) / DURATIONS[INVIS], gInvisTextTexture.getHeight()};
					if (invis.getTimeDifference() < DURATIONS[INVIS] && invis.getLastTick() != 0) {
						SDL_RenderDrawRect(gRenderer, &invisFill);
					}

					scoreText.str("");
					speedText.str("");
					doubleText.str("");
					invisText.str("");

					scoreText << "Score:" << dot.getScore();
					speedText << "Extra speed: ";
					doubleText << "Double points: ";
					invisText << "Invisibility: ";

					// render text
					if (scoreText.str().size() > 0) {
						if (!gScoreTextTexture.loadFromRenderedText(scoreText.str().c_str(), textColor)) {
							printf("Render score texture error\n");
						}
						gScoreTextTexture.render(PADDING, PADDING);
					}
					if (doubleText.str().size() > 0) {
						if (!gDoubleTextTexture.loadFromRenderedText(doubleText.str().c_str(), textColor)) {
							printf("Render double texture error\n");
						}
						gDoubleTextTexture.render(SCREEN_WIDTH - 100 - gDoubleTextTexture.getWidth() - PADDING, PADDING);
					}
					if (speedText.str().size() > 0) {
						if (!gSpeedTextTexture.loadFromRenderedText(speedText.str().c_str(), textColor)) {
							printf("Render time texture error\n");
						}
						gSpeedTextTexture.render(SCREEN_WIDTH - 100 - gSpeedTextTexture.getWidth() - PADDING, PADDING + gDoubleTextTexture.getHeight());
					}
					if (invisText.str().size() > 0) {
						if (!gInvisTextTexture.loadFromRenderedText(invisText.str().c_str(), textColor)) {
							printf("Render time texture error\n");
						}
						gInvisTextTexture.render(SCREEN_WIDTH - 100 - gInvisTextTexture.getWidth() - PADDING, PADDING + gDoubleTextTexture.getHeight() + gSpeedTextTexture.getHeight());
					}

					obstacle.move(&gameTimer);
					obstacle.render();

					// invis powerup: check if we need to change alpha property
					if (invis.getTimeDifference() < DURATIONS[INVIS] && invis.getLastTick() != 0) {
						gDotTexture.setAlpha(127);
					}
					else {
						gDotTexture.setAlpha(255);
					}
					dot.render();
					point.render();
					speed.render();
					invis.render();
					dbl.render();

					SDL_RenderPresent(gRenderer);
				}
			}
		}
	}

	close();

	return 0;
}