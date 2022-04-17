#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <string.h>
#pragma warning(disable : 4996)

extern "C" {
#include "../szablon2/SDL2-2.0.10/include/SDL.h"
#include "../szablon2/SDL2-2.0.10/include/SDL_main.h"
}

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480

enum levelConst{
	levelWidth = SCREEN_WIDTH * 4,
	levelHeight = SCREEN_HEIGHT * 3,

	xCamera = 0,
	yCamera = levelHeight / 2,
};

enum unicornConst{ //stałe dotyczące jednorożca
	lifesNumber = 3,

	unicornWidth = SCREEN_WIDTH / 10,
	unicornHeight = SCREEN_HEIGHT / 10,
	halfHeightUni = unicornHeight / 2,
	halfWidthUni = unicornWidth / 2,

	xInitial = SCREEN_WIDTH / 10,
	yInitial = levelHeight / 2,
	
	jumpHeight = SCREEN_HEIGHT / 6,
	jumpLenght = SCREEN_WIDTH / 20,
	jumpSpeed = 43,
	initialSpeed = 45,

	speedTime = 15,
	dashTime = 3,			//w sekundach
	dashedTime = 1,
};

enum pltformsConst {//stałe dotyczęce platform (współrzędna x, współrzędna y, szerokość platformy)
	numberOfPlatforms = 8,

	widthFloor = SCREEN_WIDTH,		//platforma startowa (podłoże)
	heightFloor = SCREEN_HEIGHT / 20,
	yFloor = levelHeight / 2 + heightFloor / 2 + halfHeightUni,
	xFloor = SCREEN_WIDTH / 2,

	xOne = xFloor + SCREEN_WIDTH * 5 / 4,
	yOne = yFloor + SCREEN_HEIGHT / 2,
	widthOne = SCREEN_WIDTH * 3 / 4,

	xTwo = xOne + SCREEN_WIDTH * 5 / 4,
	yTwo = yOne,
	widthTwo = SCREEN_WIDTH / 4,

	xThree = xTwo + SCREEN_WIDTH / 3,
	yThree = yTwo - jumpHeight,
	widthThree = SCREEN_WIDTH / 8,

	xFour = xThree + SCREEN_WIDTH / 3,
	yFour = yFloor,
	widthFour = SCREEN_WIDTH / 4,

	xFive = xFloor + SCREEN_WIDTH * 2,
	yFive = levelHeight - SCREEN_HEIGHT / 2,
	widthFive = SCREEN_HEIGHT * 3 / 2,

	xSix = SCREEN_WIDTH * 2,
	ySix = yFloor - 2 * heightFloor - jumpHeight,
	widthSix = SCREEN_WIDTH * 2,

	xSeven = xSix,
	ySeven = ySix - SCREEN_HEIGHT / 3 + jumpHeight / 8,
	widthSeven = SCREEN_WIDTH / 2,

	/*xEight = xSix,
	yEight = ySix - SCREEN_HEIGHT / 3 + jumpHeight / 8,
	widthEigh = SCREEN_WIDTH / 2,*/
};

enum obstaclesConst {//stałe dotyczące przeszkód
	numberOfObstacles = 4,
	widthObst = unicornHeight,
	heightObst = unicornHeight,

	xZeroObst = SCREEN_WIDTH * 3 / 5,
	yZeroObst = yFloor - heightFloor / 2 - heightObst / 2,

	xOneObst = xSix - SCREEN_WIDTH * 3 / 4,
	yOneObst = ySix - heightFloor / 2 - heightObst / 2,

	xTwoObst = xSix + SCREEN_WIDTH * 3 / 4,
	yTwoObst = ySix - heightFloor / 2 - heightObst / 2,

	xThreeObst = xOne + SCREEN_WIDTH / 8,
	yThreeObst = yOne - heightFloor / 2 - heightObst / 2,
};

enum stalactiteConst {
	numberOfStalactites = 2,
	widthStal = unicornHeight * 2 / 5,
	heightStal = unicornHeight * 3,

	xZeroStal = SCREEN_WIDTH / 4,
	yZeroStal = (levelHeight - SCREEN_HEIGHT) / 2 + heightStal / 3,

	xOneStal = xSix - halfHeightUni,
	yOneStal = ySeven - SCREEN_HEIGHT / 2 + heightStal / 3,
};

enum starsConst {
	numberOfStars = 1,
	widthStars = halfHeightUni,
	heightStars = halfHeightUni,

	xZeroStar = xSeven,
	yZeroStar = ySeven - heightFloor / 2 - heightStars / 2,
};


struct unicorn {
	int lifes = lifesNumber;			//liczba żyć

	int width = unicornWidth;			//szerokość jednorożca
	int height = unicornHeight;			//wysokość jednorożca

	double x1 = xInitial;				//wcześniejsz współrzędna x jednorożca (zanim wykonał ruch)
	double x2 = xInitial;				//współrzędna x jednorożca obecnie (po wykonaniu ruchu)
    double y1 = yInitial;				//współrzędna y jednorożca przed double jump
	double y2 = yInitial;				//aktualna współrzędna y jednorożca
	double y_ground = yInitial;

	int speed = initialSpeed;			//prędkość jednorożca
	bool speed_up = false;

	double y_jump;						//zmienna przechowująca informacje o tym jak wysoko jednorożec ma skoczyć
	double y_double_jump;
	bool jump = false;					//zmienna przechowująca stan o skoku (czy jednorożec aktualnie skacze)
	bool double_jump = false;			//zmienna przechowująca stan o double-jump

	bool dash = false;							//zmienna przechowująca stan o zrywie (czy jednorożec aktualnie jest w zrywie)
	bool have_dashed = false;					//zmienna przechowująca stan o skończonym zrywie
	double dash_time = dashTime;				//długość trwania zrywu
	double dashed_time = dashedTime;			//przedział czasu w którym naciśnięcie z umożliwia kolejny skok
	int dash_speed = speed + initialSpeed;		//szybkość ruchu jednorożca podczas zrywu
	bool dash_jump = false;

	double bottom = y2 + height / 2;			//krawędzie jednorożca (do wykrywania kolizji oraz zapobiegania wypadnięciu)
	double top = y2 - height / 2;
	double left = x2 - width / 2;
	double right = x2 + width / 2;

	SDL_Surface *sprite;
}unicorn;

struct level {
	int width = levelWidth;			//szerokość lewelu
	int height = levelHeight;		//wysokość lewelu
	
	SDL_Surface* level;
}level;

struct camera {
	int width = SCREEN_WIDTH;			//szerokości wyświetlanego obrazu
	int height = SCREEN_HEIGHT / 2;		//połowa wysokości wyświetlanego obrazu

	double x = xCamera;					//współrzędna x zagnieżdżenia kamery
	double y = yCamera;					//współrzędna y wyświetalnego obrazu

	double xFromUni = xInitial;					//odległość współrzędnej x środka wyświetlanego obrazu od współrzędnej x środka jednorożca
	double yFromUni = SCREEN_HEIGHT * 3 / 2 - yInitial;		//odległość współrzędnej y środka wyświetlanego obrazu od współrzędnej y środka jednorożca

	int number = 0;						//zmienna do przechowywania ile razy kamara przejechała przez cały poziom
}camera;

struct obstacle {
	int width = widthObst;				//wysokość przeszkody
	int height = widthObst;				//szerokość przeszkody

	double x;					//współrzędna x środka przeszkody
	double y;				//współrzędna y środka przeszkody

	double bottom;				//krawędzie przeszkody (do określenia kolizji)
	double top;
	double left;
	double right;

	SDL_Surface* obstacle;
} obstacles[numberOfObstacles], stalactites[numberOfStalactites], platforms[numberOfPlatforms], stars[numberOfStars];

struct values {//tablice przechowujące stałe poszczególnych typów
	double xObst[numberOfObstacles] = { xZeroObst, xOneObst, xTwoObst, xThreeObst};
	double yObst[numberOfObstacles] = {yZeroObst, yOneObst, yTwoObst, yThreeObst};

	double xStal[numberOfStalactites] = { xZeroStal, xOneStal };
	double yStal[numberOfStalactites] = { yZeroStal, yOneStal };

	double xPlatf[numberOfPlatforms] = {xFloor, xOne, xTwo, xThree, xFour, xFive, xSix, xSeven};
	double yPlatf[numberOfPlatforms] = {yFloor, yOne, yTwo, yThree, yFour, yFive, ySix, ySeven};
	double widthPlatf[numberOfPlatforms] = {widthFloor, widthOne, widthTwo, widthThree, widthFour, widthFive, widthSix, widthSeven};

	double xStars[numberOfStars] = { xZeroStar};
	double yStars[numberOfStars] = { yZeroStar };
}values;



// narysowanie napisu txt na powierzchni screen, zaczynając od punktu (x, y)
// charset to bitmapa 128x128 zawierająca znaki
void DrawString(SDL_Surface *screen, int x, int y, const char *text, SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while(*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
		};
	};


// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt środka obrazka sprite na ekranie
void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
	};


// rysowanie pojedynczego pixela
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
	};


// rysowanie linii o długości l w pionie (gdy dx = 0, dy = 1) 
// bądź poziomie (gdy dx = 1, dy = 0)
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for(int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
		};
	};


// rysowanie prostokąta o długości boków l i k
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for(i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
	};


void resetUnicornValues() {
	unicorn.x1 = xInitial;
	unicorn.x2 = xInitial;
	unicorn.y1 = yInitial;
	unicorn.y2 = yInitial;
	unicorn.y_ground = yInitial;
	unicorn.jump = false;
	unicorn.double_jump = false;
	unicorn.dash = false;
	unicorn.have_dashed = false;

	unicorn.bottom = unicorn.y2 + unicorn.height / 2;
	unicorn.top = unicorn.y2 - unicorn.height / 2;
	unicorn.left = unicorn.x2 - unicorn.width / 2;
	unicorn.right = unicorn.x2 + unicorn.width / 2;
}

void resetValues(int* t1, int* t2, double* worldTime, double* distance, int* defeat, int* nav, int* new_game, int* speed) {
	*t1 = 0;
	*t2 = 0;
	*nav = 1;
	*worldTime = 0;
	*distance = 0;
	*defeat = 0;
	*new_game = 0;
	*speed = initialSpeed;

	resetUnicornValues();
	unicorn.speed = initialSpeed;
	unicorn.dash_speed = unicorn.speed + initialSpeed;
	unicorn.dashed_time = dashedTime;
	unicorn.dash_speed = dashedTime;
	unicorn.lifes = lifesNumber;

	camera.x = 0;
	camera.y = levelHeight / 2;
}


int collision(obstacle *object, int number_of_objects) {
	for (int i = 0; i < number_of_objects; i++) {
		if (unicorn.right >= object[i].left && unicorn.bottom >= object[i].top && unicorn.left <= object[i].right && unicorn.top <= object[i].bottom) {
			//kolizja od frontu
			unicorn.lifes--;
			return 1;
		}
	}
	return 0;
}

int collisionWithPlatformsAndStars() {
	if (unicorn.top < 0 || unicorn.bottom > levelHeight) {
		unicorn.lifes--;
		return 1;
	}
	for (int i = 0; i < numberOfPlatforms; i++) {
		if (unicorn.right >= platforms[i].left && unicorn.top <= platforms[i].bottom && unicorn.left <= platforms[i].right && unicorn.bottom > platforms[i].top) {
			//uderzenie w przeszkodę u góry
			unicorn.lifes--;
			return 1;
		}
	}
	for (int i = 0; i < numberOfStars; i++) {
		if (unicorn.right >= stars[i].left && unicorn.bottom >= stars[i].top && unicorn.left <= stars[i].right && unicorn.top <= stars[i].bottom && unicorn.dash == false) {
			//kolizja od frontu
			unicorn.lifes--;
			return 1;
		}
	}
	return 0;
}

void jump(double dt) {
	if (unicorn.double_jump && unicorn.y2 >= unicorn.y_double_jump && unicorn.dash_jump == false) {
		unicorn.y2 -= (jumpSpeed * dt);
		unicorn.bottom = unicorn.y2 + unicorn.height / 2;
		unicorn.top = unicorn.y2 - unicorn.height / 2;
	}else if (unicorn.double_jump && unicorn.y2 <= unicorn.y_double_jump && unicorn.dash_jump == false) {
		unicorn.y2 = unicorn.y2;
		unicorn.bottom = unicorn.y2 + unicorn.height / 2;
		unicorn.top = unicorn.y2 - unicorn.height / 2;
	}else if (unicorn.jump && unicorn.double_jump == false && unicorn.dash_jump == false) {
		unicorn.y2 -= (jumpSpeed * dt);
		unicorn.bottom = unicorn.y2 + unicorn.height / 2;
		unicorn.top = unicorn.y2 - unicorn.height / 2;
		if (unicorn.y2 <= unicorn.y_jump && unicorn.double_jump == false) {
			unicorn.jump = false;
		}
	}else if (unicorn.dash_jump == true) {
		unicorn.y2 -= (jumpSpeed * dt);
		unicorn.bottom = unicorn.y2 + unicorn.height / 2;
		unicorn.top = unicorn.y2 - unicorn.height / 2;
		if (unicorn.y2 <= unicorn.y_jump && unicorn.double_jump == false) {
			unicorn.dash_jump = false;
			unicorn.jump = false;
		}
	}
	
}

void fall(double dt) {
	if ((unicorn.y_ground - unicorn.y2) < jumpSpeed * dt && unicorn.y2 < unicorn.y_ground) {
		unicorn.y2 += (unicorn.y_ground - unicorn.y2);
		unicorn.top += (unicorn.y_ground - unicorn.y2);
		unicorn.bottom += (unicorn.y_ground - unicorn.y2);
	}else {
		unicorn.y2 += jumpSpeed * dt;
		unicorn.top += jumpSpeed * dt;
		unicorn.bottom += jumpSpeed * dt;
	}
}

void jumpController(int navigation, int defeat, double delta) {
	if (navigation == 0 && defeat != 1 && unicorn.dash == false) {
		if ((unicorn.jump || unicorn.dash_jump)) {
			// && unicorn.top > 0 <- warunek na nie wychodzenie nad planszę
			jump(delta);
		}else if (unicorn.y2 != unicorn.y_ground) {
			fall(delta);
		}
	}
}


void setCamera(SDL_Surface *level){//ustawienie parametrów wyświetlanego obszaru
	
	if (unicorn.x1 != unicorn.x2){
		double dx = unicorn.x2 - unicorn.x1;
		camera.x += dx;
		unicorn.x1 = unicorn.x2;
	}
	if (unicorn.y2 != unicorn.y1 && unicorn.y2 >= SCREEN_HEIGHT / 2 && unicorn.y2 <= levelHeight - SCREEN_HEIGHT / 2 && unicorn.y1 >= SCREEN_HEIGHT / 2 && unicorn.y1 <= levelHeight - SCREEN_HEIGHT / 2) {
		double dy = unicorn.y2 - unicorn.y1;
		camera.y += dy;
		unicorn.y1 = unicorn.y2;
	}else {
		unicorn.y1 = unicorn.y2;
	}
}

void checkCameraPossition() {//sprawdzenie, czy obszar kamery pokazuje koniec poziomu
	if (camera.x >= levelWidth) {
		camera.x = 0;
		unicorn.x1 = xInitial;
		unicorn.x2 = xInitial;
		unicorn.left = unicorn.x2 - unicorn.width / 2;
		unicorn.right = unicorn.x2 + unicorn.width / 2;
	}
}

int checkGround() {
	if (camera.x < levelWidth && camera.x >= levelWidth - unicorn.width && unicorn.bottom >= platforms[0].top) {
		return platforms[0].top + unicorn.height / 2;
	}
	
	for (int i = 0; i < numberOfPlatforms; i++) {
		if ((platforms[i].left - unicorn.width) <= unicorn.left && (platforms[i].right + unicorn.width) >= unicorn.right && unicorn.bottom <= platforms[i].top) {
			bool check = true;
			for (int j = 0; j < numberOfPlatforms; j++) {
				if (platforms[j].y < platforms[i].y && (platforms[j].left - unicorn.width) <= unicorn.right && (platforms[j].right + unicorn.width) >= unicorn.right && unicorn.bottom <= platforms[j].top) {
					check = false;
				}
			}
			if (check == true) {
				return platforms[i].top - halfHeightUni;
			}
		}
	}
	//return levelHeight - halfHeightUni      <- zapobieganie wypadnięciu poza planszę
	return levelHeight + unicornHeight;
	
}

void setUniPossition(int *t1, int *t2, double *worldTime, int navigation, int *speed, double *distance, int defeat) {
	double delta;
	delta = (*t2 - *t1) * 0.001;
	*t1 = *t2;
	*worldTime += delta;
	if (navigation == 0) {
		if (unicorn.dash == true && unicorn.dash_time > 0) {
			unicorn.dash_time -= delta;
		}
		else if (unicorn.dash_time <= 0 && unicorn.dash == true) {
			unicorn.dash_time = dashTime;
			*speed = unicorn.speed;
			unicorn.dash = false;
			unicorn.have_dashed = true;
		}else if (unicorn.have_dashed == true && unicorn.dashed_time > 0) {
			unicorn.dashed_time -= delta;
			if (unicorn.dashed_time <= 0) {
				unicorn.have_dashed = false;
				unicorn.dashed_time = dashedTime;
			}
		}
		unicorn.x2 += *speed * delta;
		unicorn.left = unicorn.x2 - halfWidthUni;
		unicorn.right = unicorn.x2 + halfWidthUni;
		*distance += unicorn.x2 - unicorn.x1;
		jumpController(navigation, defeat, delta);
	}
	unicorn.y_ground = checkGround();
}

void speedUp(double time) {
	if (floor(10 * fmod(time, speedTime)) == 0.0 && unicorn.speed_up == false && time > 1.0) {
		//floor(10 * fmod(time, 5.0)) == 0.0  <-  sprawdzenie czy czas który upłynął jest wielokrotnością czasu co ile ma następować przyspieszenie
		//sprawdzenie, czy zaokrąglenie dziesięciokrotności reszty z dzielenia czasu, który upłynął przez zmianę czasu co ile występuje przysp. jest > 0
		unicorn.speed += initialSpeed;
		unicorn.dash_speed += initialSpeed;
		unicorn.speed_up = true;				//zapamiętanie, że jednorożec przyspieszył, żeby uniknąć kolejnego przyspieszenia
	}else if (floor(10 * fmod(time, speedTime)) > 0.0 && unicorn.speed_up == true) {
		unicorn.speed_up = false;				//gdy jest wiadome, że resza z dzielenia jest większa od zera, zresetowanie informacji o przysp.
	}
}



void basicNavigation(SDL_Event event, double *distance, int *t1) {
	if (event.key.keysym.sym == SDLK_DOWN && unicorn.y2 < unicorn.y_ground) {
		if (*t1 == 0) {
			*t1 = SDL_GetTicks();
		}
		unicorn.y2 += 2.0;			//zmiana wartości y środka jednorożca
		unicorn.bottom += 2.0;		//przesunięcie poziomych krawędzi jednorożca
		unicorn.top += 2.0;
	}else if (event.key.keysym.sym == SDLK_RIGHT) {
		if (*t1 == 0) {
			*t1 = SDL_GetTicks();
		}
		unicorn.x2 += 2.0;			//zmiana wartości x środka jednorożca
		unicorn.left += 2.0;		//przesunięcie pionowych krawędzi jednorożca
		unicorn.right += 2.0;
		*distance += (unicorn.x2 - unicorn.x1);
	}else if (event.key.keysym.sym == SDLK_UP) {
		if (*t1 == 0) {
			*t1 = SDL_GetTicks();
		}
		unicorn.y2 -= 2.0;			//zmiana wartości y środka jednorożca
		unicorn.bottom -= 2.0;		//przesunięcie poziomych krawędzi jednorożca
		unicorn.top -= 2.0;
	}//else if (event.key.keysym.sym == SDLK_LEFT && defeat != 1) x_ini -= 2.0;        NIE PRZYWIDUJEMY COFANIA JEDNOROŻCA
}

int advancedNavigation(SDL_Event event) {
	if (event.key.keysym.sym == SDLK_z && unicorn.dash == false && unicorn.dash_jump == false) {
		if (unicorn.jump == false && unicorn.y2 >= unicorn.y1) {// && unicorn.top >= unicornHeight  <- warunek na nie wychodzenie nad planszę
			unicorn.jump = true;
			unicorn.y_jump = unicorn.y_ground - jumpHeight;
			unicorn.y_double_jump = unicorn.y_jump - jumpHeight;
		}else if(unicorn.y2 >= unicorn.y_ground - jumpHeight && unicorn.dash_jump == false && unicorn.jump == true){
			unicorn.double_jump = true;
		}else if (unicorn.have_dashed == true) {
			unicorn.dash_jump = true;
			unicorn.have_dashed = false;
			unicorn.dashed_time = dashTime / 3;
			unicorn.y_jump = unicorn.y2 - jumpHeight;
			unicorn.y_double_jump = unicorn.y_jump;
		}
	}else if (event.key.keysym.sym == SDLK_x && unicorn.dash == false) {
		if (unicorn.jump == true) {
			unicorn.jump = false;
		}
		unicorn.dash = true;
		return unicorn.dash_speed;
	}
	return unicorn.speed;
}

void Navigation(SDL_Event *event, int *quit, int *navigation, int *defeat, int *speed, double *distance, int *new_game, int *t1) {
	switch ((*event).type) {
	case SDL_KEYDOWN:
		if ((*event).key.keysym.sym == SDLK_ESCAPE) *quit = 1;
		else if ((*event).key.keysym.sym == SDLK_n) *new_game = 1;
		else if ((*event).key.keysym.sym == SDLK_d) {
			if (*t1 == 0) {
				*t1 = SDL_GetTicks();
			}
			*navigation = 0;
		}
		else if (*defeat != 1 && *navigation == 1) {
			basicNavigation((*event), distance, t1);
		}else if (*defeat != 1 && *navigation != 1) {
			*speed = advancedNavigation((*event));
		}
		else if (*defeat > 0 && unicorn.lifes > 0 && (*event).key.keysym.sym == SDLK_k) {
			resetUnicornValues();

			camera.x = xCamera;
			camera.y = yCamera;
			
			*defeat = 0;
		}
		break;
	case SDL_KEYUP:
		if (unicorn.double_jump == true && unicorn.dash_jump == false) {
			unicorn.double_jump = false;
			unicorn.jump = false;
		}
		break;
	case SDL_QUIT:
		*quit = 1;
		break;
	};
	
}


void renderUnicorn() {

	int pink = SDL_MapRGB(level.level->format, 0xFF, 0x00, 0xFF);
	double uni;

	if (camera.y < levelHeight - SCREEN_HEIGHT / 2 && camera.y > SCREEN_HEIGHT / 2) {
		uni = SCREEN_HEIGHT / 2;
	}else if (unicorn.y2 <= SCREEN_HEIGHT / 2) {
		uni = unicorn.y2;
	}else {
		uni = unicorn.y2 - levelHeight + SCREEN_HEIGHT;
	}

	DrawSurface(level.level, unicorn.sprite, xInitial, uni);
	SDL_FillRect(unicorn.sprite, NULL, pink);
}

void renderObstacles(obstacle *object, int number_of_objects, int color) {
	for (int i = 0; i < number_of_objects; i++) {
		double y, x;
		double dy = object[i].y - camera.y;
		double dx = object[i].left - camera.x;

		if (dy < SCREEN_HEIGHT / 2 && dy > -SCREEN_HEIGHT / 2 && camera.y > SCREEN_HEIGHT / 2 && camera.y < levelHeight - SCREEN_HEIGHT / 2) {
			y = SCREEN_HEIGHT / 2 + dy;
		}
		else if (dy < SCREEN_HEIGHT / 2 && dy > -SCREEN_HEIGHT / 2 && (camera.y <= SCREEN_HEIGHT / 2 || camera.y >= levelHeight - SCREEN_HEIGHT / 2)) {
			y = fmod(object[i].y, SCREEN_HEIGHT);
		}
		else {
			y = SCREEN_HEIGHT / 2 + dy;
		}
		if (object[i].left <= SCREEN_WIDTH && camera.x > levelWidth - SCREEN_WIDTH) {
			x = levelWidth + dx + object[i].width / 2;
		}
		else {
			x = dx + object[i].width / 2;
		}

		DrawSurface(level.level, object[i].obstacle, x, y);
		SDL_FillRect(object[i].obstacle, NULL, color);
	}
}


void drawLevel(double worldTime, double distance, SDL_Surface* charset, SDL_Texture* scrtex, SDL_Renderer* renderer, int nav) {	//rysowanie poziomu
	char text[128];
	int black = SDL_MapRGB(level.level->format, 0x00, 0x00, 0x00);
	int grey = SDL_MapRGB(level.level->format, 0xC6, 0xCE, 0xCE);
	int red = SDL_MapRGB(level.level->format, 0xFF, 0x00, 0x00);
	int blue = SDL_MapRGB(level.level->format, 0x11, 0x11, 0xCC);
	int brown = SDL_MapRGB(level.level->format, 0x4D, 0x1F, 0x1C);
	int yellow = SDL_MapRGB(level.level->format, 0xFE, 0xFE, 0x33);

	SDL_FillRect(level.level, NULL, grey);
	
	setCamera(level.level);
	renderObstacles(obstacles, numberOfObstacles, black);
	renderObstacles(platforms, numberOfPlatforms, brown);
	renderObstacles(stalactites, numberOfStalactites, black);
	renderObstacles(stars, numberOfStars, yellow);
	renderUnicorn();

	// tekst informacyjny / info text
	DrawRectangle(level.level, 4, 4, SCREEN_WIDTH - 8, 54, red, blue);
	//            "template for the second project, elapsed time = %.1lf s  %.0lf frames / s"
	sprintf(text, "Czas trwania = %.1f s, przebyty dystans = %.1f", worldTime, distance);
	DrawString(level.level, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, 10, text, charset);
	if (nav == 1) {
		sprintf(text, "n - nowa gra, Esc - wyjscie, d - przelacz strowanie");
		DrawString(level.level, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, 26, text, charset);
		sprintf(text, "\030 - w gore, \031 - w dol, \033 - w prawo");
		DrawString(level.level, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, 44, text, charset);
	}else {
		sprintf(text, "n - nowa gra, Esc - wyjscie");
		DrawString(level.level, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, 26, text, charset);
		sprintf(text, "z - skok, przytrzymanie z - podwojny skok, x - zryw");
		DrawString(level.level, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, 44, text, charset);
	}

	for (int i = 1; i <= unicorn.lifes; i++) {
		DrawRectangle(level.level, SCREEN_WIDTH - (20 * i), 62, 15, 15, red, red);
	}

	SDL_UpdateTexture(scrtex, NULL, level.level->pixels, level.level->pitch);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void drawDefeat(SDL_Surface* charset, SDL_Texture* scrtex, SDL_Renderer* renderer, double time, double distance) {	
	//rysowanie panelu o przegranej
	char text[128];
	int grey = SDL_MapRGB(level.level->format, 0x36, 0x45, 0x4F);
	
	SDL_FillRect(level.level, NULL, grey);

	sprintf(text, "Przegrana :(");
	DrawString(level.level, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2 - 16, text, charset);
	//	      "Esc - exit, \030 - faster, \031 - slower" , \030 - przyspieszenie, \031 - zwolnienie
	sprintf(text, "n - nowa gra, Esc - wyjscie");
	DrawString(level.level, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2 + 16, text, charset);

	sprintf(text, "Czas trwania = %.1f s, przebyty dystans = %.1f", time, distance);
	DrawString(level.level, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2, text, charset);

	SDL_UpdateTexture(scrtex, NULL, level.level->pixels, level.level->pitch);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void drawQuestion(SDL_Surface* charset, SDL_Texture* scrtex, SDL_Renderer* renderer) {
	char text[128];
	int grey = SDL_MapRGB(level.level->format, 0x36, 0x45, 0x4F);

	SDL_FillRect(level.level, NULL, grey);

	sprintf(text, "Przegrana. Chcesz kontynuowac gre?");
	DrawString(level.level, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2 - 10, text, charset);
	sprintf(text, "k - kontynuuj, n - nowa gra, Esc - wyjscie");
	DrawString(level.level, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2 + 10, text, charset);

	SDL_UpdateTexture(scrtex, NULL, level.level->pixels, level.level->pitch);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);
	SDL_RenderPresent(renderer);
}


void setObjectsCoordinates() {
	for (int i = 0; i < numberOfPlatforms; i++) {
		platforms[i].x = values.xPlatf[i];
		platforms[i].y = values.yPlatf[i];
		platforms[i].width = values.widthPlatf[i];
		platforms[i].height = heightFloor;
	}
	for (int i = 0; i < numberOfObstacles; i++) {
		obstacles[i].x = values.xObst[i];
		obstacles[i].y = values.yObst[i];
	}
	for (int i = 0; i < numberOfStalactites; i++) {
		stalactites[i].x = values.xStal[i];
		stalactites[i].y = values.yStal[i];
		stalactites[i].height = heightStal;
		stalactites[i].width = widthStal;
	}
	for (int i = 0; i < numberOfStars; i++) {
		stars[i].x = values.xStars[i];
		stars[i].y = values.yStars[i];
		stars[i].height = heightStars;
		stars[i].width = widthStars;
	}
}

void createObjects(obstacle *object, int number_of_objects) {

	for (int i = 0; i < number_of_objects; i++) {
		object[i].bottom = object[i].y + object[i].height / 2;
		object[i].top = object[i].y - object[i].height / 2;
		object[i].left = object[i].x - object[i].width / 2;
		object[i].right = object[i].x + object[i].width / 2;
		object[i].obstacle = SDL_CreateRGBSurface(0, object[i].width, object[i].height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	}
}


void createSurfaces() {
	level.level = SDL_CreateRGBSurface(0, level.width, level.height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	unicorn.sprite = SDL_CreateRGBSurface(0, unicornWidth, unicornHeight, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	
	setObjectsCoordinates();

	createObjects(platforms, numberOfPlatforms);

	createObjects(obstacles, numberOfObstacles);

	createObjects(stalactites, numberOfStalactites);

	createObjects(stars, numberOfStars);
}



void move(SDL_Surface* charset, SDL_Texture* scrtex, SDL_Window* window, SDL_Renderer* renderer) {
	int t1, t2, quit, rc, start, defeat, navigation, speed, new_game;
	double delta, worldTime, distance;
	SDL_Event event;

	quit = 0;
	resetValues(&t1, &t2, &worldTime, &distance, &defeat, &navigation, &new_game, &speed);

	while (!quit) {
		if (new_game == 1) {
			resetValues(&t1, &t2, &worldTime, &distance, &defeat, &navigation, &new_game, &speed);
		}
		if (defeat == 1 && unicorn.lifes == 0) {
			drawDefeat(charset, scrtex, renderer, worldTime, distance);
		}else if (defeat == 1 && unicorn.lifes != 0) {
			drawQuestion(charset, scrtex, renderer);
		}else {
			checkCameraPossition();
			if (t1 != 0) {
				t2 = SDL_GetTicks();
			}
			setUniPossition(&t1, &t2, &worldTime, navigation, &speed, &distance, defeat);
			drawLevel(worldTime, distance, charset, scrtex, renderer, navigation);
			//defeat = collision();
			defeat = collisionWithPlatformsAndStars();
			if (defeat != 1) {
				defeat = collision(obstacles, numberOfObstacles);
			}
			if (defeat != 1) {
				defeat = collision(stalactites, numberOfStalactites);
			}
			speedUp(worldTime);
		}
		while (SDL_PollEvent(&event)) {
			Navigation(&event, &quit, &navigation, &defeat, &speed, &distance, &new_game, &t1);
		};
	};

}


void freeSurface(SDL_Texture* scrtex, SDL_Window* window, SDL_Renderer* renderer, SDL_Surface* charset) {
	SDL_FreeSurface(level.level);
	SDL_FreeSurface(unicorn.sprite);
	for (int i = 0; i < numberOfObstacles; i++) {
		SDL_FreeSurface(obstacles[i].obstacle);
	}
	for (int i = 0; i < numberOfPlatforms; i++) {
		SDL_FreeSurface(platforms[i].obstacle);
	}
	for (int i = 0; i < numberOfStalactites; i++) {
		SDL_FreeSurface(stalactites[i].obstacle);
	}
	SDL_FreeSurface(charset);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
}


// main
#ifdef __cplusplus
extern "C"
#endif

int main(int argc, char **argv) {
	int rc;
	SDL_Surface *charset;
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;

	// okno konsoli nie jest widoczne, jeżeli chcemy zobaczyć
	// komunikaty wypisywane printf-em trzeba w opcjach:
	// project -> szablon2 properties -> Linker -> System -> Subsystem
	// zmienić na "Console"
	printf("wyjscie printfa trafia do tego okienka\n");
	printf("printf output goes here\n");

	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
	}

	// tryb pełnoekranowy / fullscreen mode
	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
	if(rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
	};
	
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(window, "Szablon do zdania drugiego 2017");

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

	// wyłączenie widoczności kursora myszy
	SDL_ShowCursor(SDL_DISABLE);

	// wczytanie obrazka cs8x8.bmp
	charset = SDL_LoadBMP("../szablon2/cs8x8.bmp");
	if(charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());

		freeSurface(scrtex, window, renderer, charset);

		SDL_Quit();
		return 1;
	};
	SDL_SetColorKey(charset, true, 0x000000);

	createSurfaces();
	
	move(charset, scrtex, window, renderer);
	

	// zwolnienie powierzchni / freeing all surfaces
	freeSurface(scrtex, window, renderer, charset);

	SDL_Quit();
	return 0;
};
