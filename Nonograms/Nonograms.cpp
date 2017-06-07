/* Main executable and game loop. */

#include "stdafx.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <fstream>

#define COLOR_CLEAN   	0x00, 0x00, 0x00, 0x00
#define COLOR_SELECT   	0x00, 0x50, 0x50, 0x40
#define COLOR_GRID     	0x00, 0x00, 0x00, 0x90 
#define COLOR_BAD_CELL 	0xFF, 0xB8, 0xCF, 0x90

struct Texture {
	SDL_Texture* texture;
	int width;
	int height;
};

int nonoData[15][15] =
{
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
};

int nonoImage[15][15] =
{
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
};

int score = 0;
int penalty = 0;

int loadTexture(const std::string &file, Texture &texture);

SDL_Surface* loadSurface(const std::string &file);

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int CELL_SIZE = 40;

//The window we'll be rendering to
SDL_Window* window = NULL;

//The surface contained by the window
SDL_Surface* gSurface = NULL;

SDL_Renderer* gRenderer = NULL;

SDL_Texture* gTexture = NULL;

SDL_Surface* icon = NULL;

Texture bgTxtr;
Texture digTxtrs[226];
Texture txtMinus;

void load(std::string file)
{
	char str[1000];
	std::ifstream infile(file);
	int i = 0;
	while (infile >> str)
	{
		int j = 0;
		for (int j = 0; j < 15; j++)
		{
			nonoData[j][i] = 0;
			nonoImage[j][i] = str[j] == '*' ? 1 : 0;
			//nonoData[j][i] = str[j] == '*' ? 1 : 0;
		}
		i++;
	}

}

int init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return -1;
	}
	window = SDL_CreateWindow(
		"SDL Tutorial",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return -1;
	}

	gSurface = SDL_GetWindowSurface(window);
	if (gSurface == NULL)
	{
		printf("Surface could not be created! SDL_Error: %s\n", SDL_GetError());
		return -1;
	}

	gRenderer = SDL_CreateRenderer(window, -1, 0);
	if (gRenderer == NULL)
	{
		const char *tmp;
		tmp = SDL_GetError();
		printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());		
		
		return -1;
	}
	SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);

	gTexture = SDL_CreateTexture(gRenderer, gSurface->format->format,
		SDL_TEXTUREACCESS_TARGET, 9 * CELL_SIZE + 2, 9 * CELL_SIZE + 2); // might be 1 too little
	if (gTexture == NULL)
	{
		printf("Texture could not be created! SDL_Error: %s\n", SDL_GetError());
		return -1;
	}

	if (TTF_Init() < 0)
	{
		printf("TTF could not initialize! TTF_Error: %s\n", TTF_GetError());
		return -1;
	}

	SDL_Color color = { 0x00, 0x00, 0x00 };
	TTF_Font* font = TTF_OpenFont("res/lazy.ttf", 12);
	if (font != NULL)
	{
		for (int i = 0; i <= 225; i++)
		{
			std::string s = std::to_string(i);
			SDL_Surface* textSurface = TTF_RenderText_Blended(font, s.c_str(), color);

			if (textSurface == NULL)
			{
				printf("Unable to render text surface! SDL_ttf error: %s\n", TTF_GetError());
			}
			else
			{
				//Create texture from surface pixels
				SDL_Texture * texture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
				if (texture == NULL)
				{
					printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
				}
				else
				{
					Texture ft;
					ft.texture = texture;
					ft.width = textSurface->w;
					ft.height = textSurface->h;
					digTxtrs[i] = ft;
				}

				//Get rid of old surface
				SDL_FreeSurface(textSurface);
			}
		}

		std::string s = "-";
		SDL_Surface* textSurface = TTF_RenderText_Blended(font, s.c_str(), color);

		if (textSurface == NULL)
		{
			printf("Unable to render text surface! SDL_ttf error: %s\n", TTF_GetError());
		}
		else
		{
			//Create texture from surface pixels
			SDL_Texture *texture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
			if (texture == NULL)
			{
				printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
			}
			else
			{
				Texture ft;
				ft.texture = texture;
				ft.width = textSurface->w;
				ft.height = textSurface->h;
				txtMinus = ft;
			}

			//Get rid of old surface
			SDL_FreeSurface(textSurface);
		}
		TTF_CloseFont(font);

	}
	else
	{
		printf("Could not load font! SDL error: %s\n", TTF_GetError());
	}

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		return -1;
	}

	if (loadTexture("res/dark-tile.gif", bgTxtr) < 0)
	{
		printf("FAIL\n");
		return -1;
	}

	icon = loadSurface("res/sudoku_icon.png");
	SDL_SetWindowIcon(window, icon);
	SDL_SetWindowTitle(window, "Nonograms");

	//load("tree.txt");

	return 0;
}

int loadTexture(const std::string &file, Texture &texture)
{
	SDL_Surface* image = loadSurface(file);
	if (image == NULL)
	{
		printf("Can't load %s! SDL error: %s\n", file.c_str(), IMG_GetError());
		return -1;
	}

	SDL_Texture *bg = SDL_CreateTextureFromSurface(gRenderer, image);
	texture.width = image->w;
	texture.height = image->h;
	texture.texture = bg;
	SDL_FreeSurface(image);
}

bool hasEnding(std::string const &fullString, std::string const &ending) {
	if (fullString.length() >= ending.length()) {
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	}
	else {
		return false;
	}
}

SDL_Surface* loadSurface(const std::string &file)
{
	SDL_RWops *rwop = SDL_RWFromFile(file.c_str(), "rb");
	if (rwop == NULL)
	{
		printf("Can't find %s!\n", file.c_str());
		return NULL;
	}

	SDL_Surface *image = NULL;
	std::string sfx_gif = ".gif";
	std::string sfx_png = ".png";
	if (hasEnding(file, sfx_gif))
		image = IMG_LoadGIF_RW(rwop);
	else if (hasEnding(file, sfx_png))
		image = IMG_Load(file.c_str());

	if (image == NULL)
	{
		printf("Failed to load %s: SDL error: %s\n", file.c_str(), SDL_GetError());
	}
	return image;
}

void drawBackground()
{
	for (int x = 0; x < SCREEN_WIDTH; x += bgTxtr.width)
	{
		for (int y = 0; y < SCREEN_HEIGHT; y += bgTxtr.height)
		{
			SDL_Rect rect = { x, y, bgTxtr.width, bgTxtr.height };
			SDL_RenderCopy(gRenderer, bgTxtr.texture, NULL, &rect);
		}
	}

	int x = 100;
	int y = 500;

	
	
	//SDL_SetRenderDrawColor(gRenderer, COLOR_BAD_CELL);
	
	int startx = x;
	int width = CELL_SIZE * 9 / 18 / 2;
	int starty = y;
	int height = CELL_SIZE * 9 / 18 / 2;

	SDL_Rect select = { startx, starty, width, height };
	SDL_RenderFillRect(gRenderer, &select);

}

void drawGrid()
{
	int fullsize = 18 * CELL_SIZE + 1;

	SDL_SetRenderDrawColor(gRenderer, COLOR_GRID);
	for (int i = 0; i < 18; i++) {
		int x = i * (CELL_SIZE * 9 / 18);
		int y = i * (CELL_SIZE * 9 / 18);
		SDL_RenderDrawLine(gRenderer, 0, y, fullsize, y);
		SDL_RenderDrawLine(gRenderer, x, 0, x, fullsize);
		if (i % 5 == 0)
		{
			SDL_RenderDrawLine(gRenderer, 0, y + 1, fullsize, y + 1);
			SDL_RenderDrawLine(gRenderer, x + 1, 0, x + 1, fullsize);
		}
	}
}

void drawText()
{
	//Render to screen
	
	for (int i = 0; i < 18; i++)
	{
		for (int j = 0; j < 18; j++)
		{
			{
				if (i >= 15 && j < 15)
				{
					int dx = (i - 15) * 5;
					int count = nonoImage[dx][j] + nonoImage[dx + 1][j] +
						nonoImage[dx + 2][j] + nonoImage[dx + 3][j] + nonoImage[dx + 4][j];
					if (count > 0)
					{
						Texture ft = digTxtrs[count];

						int x = (i + 1) * CELL_SIZE * 9 / 18;
						int y = (j + 1) * CELL_SIZE * 9 / 18;

						int startx = ((i + 1) * CELL_SIZE * 9 / 18 - ft.width);
						int starty = ((j + 1) * CELL_SIZE * 9 / 18 - ft.height);

						SDL_Rect renderQuad = { startx, starty, ft.width, ft.height };

						//Render to screen
						SDL_RenderCopy(gRenderer, ft.texture, NULL, &renderQuad);
					}
				}
				else if (j >= 15 && i < 15)
				{
					int dx = (j - 15) * 5;
					int count = nonoImage[i][dx] + nonoImage[i][dx + 1] +
						nonoImage[i][dx + 2] + nonoImage[i][dx + 3] + nonoImage[i][dx + 4];
					if (count > 0)
					{
						Texture ft = digTxtrs[count];

						int x = (i + 1) * CELL_SIZE * 9 / 18;
						int y = (j + 1) * CELL_SIZE * 9 / 18;

						int startx = ((i + 1) * CELL_SIZE * 9 / 18 - ft.width);
						int starty = ((j + 1) * CELL_SIZE * 9 / 18 - ft.height);

						SDL_Rect renderQuad = { startx, starty, ft.width, ft.height };

						//Render to screen
						SDL_RenderCopy(gRenderer, ft.texture, NULL, &renderQuad);
					}
				}

			
				/////////////
				else if (i < 15 && j < 15)
				{
					int x = i * CELL_SIZE * 9 / 18;
					int y = j * CELL_SIZE * 9 / 18;

					SDL_SetRenderDrawColor(gRenderer, COLOR_CLEAN);
					if (nonoData[i][j] == 0)
					{
						SDL_SetRenderDrawColor(gRenderer, COLOR_CLEAN);
					}
					else if (nonoData[i][j] == 1)
					{
						SDL_SetRenderDrawColor(gRenderer, COLOR_GRID);
					}
					else if (nonoData[i][j] == 2)
					{
						SDL_SetRenderDrawColor(gRenderer, COLOR_SELECT);
					}					
					else if (nonoData[i][j] == 3)
					{
						SDL_SetRenderDrawColor(gRenderer, COLOR_BAD_CELL);
					}

					int startx = i % 5 == 0 ? x + 2 : x + 1;
					int width = CELL_SIZE * 9 / 18;
					int starty = j % 5 == 0 ? y + 2 : y + 1;
					int height = CELL_SIZE * 9 / 18;

					SDL_Rect select = { startx, starty, width, height };
					SDL_RenderFillRect(gRenderer, &select);
				}
				else if (i == 17 && j == 15)
				{
					if (score >= 0)
					{
						int x = i * CELL_SIZE * 9 / 18;
						int y = j * CELL_SIZE * 9 / 18;

						int startx = i % 5 == 0 ? x + 2 : x + 1;
						int width = CELL_SIZE * 9 / 18;
						int starty = j % 5 == 0 ? y + 2 : y + 1;
						int height = CELL_SIZE * 9 / 18;
						
						SDL_Rect renderQuad = { startx, starty, digTxtrs[score].width, digTxtrs[score].height };
						SDL_RenderCopy(gRenderer, digTxtrs[score].texture, NULL, &renderQuad);
					}
				}
				else if (i == 17 && j == 16)
				{
					if (penalty >= 0)
					{
						int x = i * CELL_SIZE * 9 / 18;
						int y = j * CELL_SIZE * 9 / 18;

						int startx = i % 5 == 0 ? x + 2 : x + 1;
						int width = CELL_SIZE * 9 / 18;
						int starty = j % 5 == 0 ? y + 2 : y + 1;
						int height = CELL_SIZE * 9 / 18;

						SDL_Rect renderQuad = { startx, starty, digTxtrs[penalty].width, digTxtrs[penalty].height };
						SDL_RenderCopy(gRenderer, digTxtrs[penalty].texture, NULL, &renderQuad);
					}
				}
			}
		}
	}	
}

void close()
{
	if (icon != NULL)
		SDL_FreeSurface(icon);
	if (gRenderer != NULL)
		SDL_DestroyRenderer(gRenderer);
	if (gSurface != NULL)
		SDL_FreeSurface(gSurface);
	if (window != NULL)
		SDL_DestroyWindow(window);
	if (gTexture != NULL)
		SDL_DestroyTexture(gTexture);
	if (bgTxtr.texture != NULL)
		SDL_DestroyTexture(bgTxtr.texture);

	for (int i = 0; i < 225; i++)
		SDL_DestroyTexture(digTxtrs[i].texture);

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char* args[])
{
	if (init() < 0)
		return -1;

	bool quit = false;
	while (!quit)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			//User requests quit
			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
			else if (event.type == SDL_KEYDOWN)
			{
				
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN)
			//do whatever you want to do after a mouse button was pressed,
			// e.g.:
			{
				int x = ((SDL_MouseButtonEvent)event.button).x;
				int y = ((SDL_MouseButtonEvent)event.button).y;
				int i = x / (CELL_SIZE * 9 / 18);
				int j = y / (CELL_SIZE * 9 / 18);
				if (i >= 0 && i < 15 && j >= 0 && j < 15)
				{
					nonoData[i][j] = (nonoData[i][j] + 1) % 4;
				}

				if (y >= 280 && y <= 300 && x >= 450 && x <= 550)
				{
					score = 0;

					for (i = 0; i < 15; i++)
					{
						for (j = 0; j < 15; j++)
						{
							if (nonoData[i][j] == 1 && nonoImage[i][j] == 1 || nonoData[i][j] == 0 && nonoImage[i][j] == 0)
							{
								score++;								
							}
							nonoData[i][j] = nonoImage[i][j];
						}
					}

					score -= penalty;
					score = score < 0 ? 0 : score;
				}
				if (y >= 216 && y <= 236 && x >= 450 && x <= 550)
				{
					for (i = 0; i < 15; i++)
					{
						for (j = 0; j < 15; j++)
						{
							if (nonoData[i][j] != nonoImage[i][j])
							{
								nonoData[i][j] = nonoImage[i][j];
								goto end;
							}
							nonoData[i][j] = nonoImage[i][j];
						}
					}

					end:
					penalty++;
				}

				if (y >= 385 && y <= 405 && x >= 450 && x <= 550)
				{
					score = 0;
					penalty = 0;
					load("tree.txt");
				}
				if (y >= 410 && y <= 435 && x >= 450 && x <= 550)
				{
					score = 0;
					penalty = 0;
					load("house.txt");
				}
				if (y >= 450 && y <= 470 && x >= 450 && x <= 550)
				{
					score = 0;
					penalty = 0;
					load("cat.txt");
				}
			}
		break;

		}

		//Fill the surface white
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);
		drawBackground();

		SDL_SetRenderTarget(gRenderer, gTexture);
		SDL_RenderClear(gRenderer);

		drawGrid();
		drawText();
		SDL_SetRenderTarget(gRenderer, NULL);

		int size = 9 * CELL_SIZE + 2;
		SDL_Rect dst = { 0, 0, size, size };
		SDL_RenderCopy(gRenderer, gTexture, NULL, &dst);

		// draw everything
		SDL_RenderPresent(gRenderer);

		SDL_Delay(16.6);
	}
	close();

	//Destroy window
	SDL_DestroyWindow(window);

	//Quit SDL subsystems
	SDL_Quit();


	return 0;
}