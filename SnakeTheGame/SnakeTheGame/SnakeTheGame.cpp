#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <SDL.h> 
#include <SDL_ttf.h> 

using namespace std;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Event event;
TTF_Font* font;

SDL_Color Red = { 255, 0, 0 };
SDL_Color White = { 255, 255, 255 };
SDL_Color Black = { 0, 0, 0 };
SDL_Color Yellow = { 255, 255, 0 };

static struct Scale
{
	// Розмір плитки
	int h = 24;
	int w = 24;
} scale;
class Player
{
public:
	SDL_Rect rect = { 0,0,0,0 };

	// tailLength збільшується щоразу, коли змія їсть їжу
	int tailLength = 0;

	// Вектори для зберігання позицій хвостових блоків
	vector<int> tailX;
	vector<int> tailY;

	// Змінні позиції гравця
	int x = scale.h * scale.w / 2;
	int y = scale.h * scale.w / 2;
	int prevX = 0;
	int prevY = 0;

	void Reset()
	{
		this->x = scale.h * scale.w / 2;
		this->y = scale.h * scale.w / 2;
		this->prevX = 0;
		this->prevY = 0;
		this->tailLength = 0;
		this->tailX.clear();
		this->tailY.clear();
	}
	void SetPrev()
	{
		this->prevX = x;
		this->prevY = y;
	}
	void MovePlayer(float delta, Scale scale)
	{
		if (delta * scale.h == 24)
		{
			// Оновлює розмір та положення хвоста
			if (tailX.size() != tailLength)
			{
				tailX.push_back(prevX);
				tailY.push_back(prevY);
			}

			// Прокручує кожен хвіст, переміщає усі блоки до найближчого блоку попереду
			// Оновлює блоки від кінця (найдальше від блоку гравця) до початку (найближчого до блоку гравця)
			for (int i = 0; i < tailLength; i++)
			{
				if (i > 0)
				{
					tailX[i - 1] = tailX[i];
					tailY[i - 1] = tailY[i];
				}
			}
			if (tailLength > 0) 
			{
				tailX[tailLength - 1] = prevX;
				tailY[tailLength - 1] = prevY;
			}
		}
	}
};
class Food
{
public:
	// Харчовий прямокутник
	SDL_Rect rect = { scale.h, scale.w, 0, 0 };

	int x = 0;
	int y = 0;

	void FoodSpawn(Player player)
	{
		bool valid = false;
		srand(time(0));

		while (!valid)
		{
			x = scale.h * (rand() % scale.w);
			y = scale.h * (rand() % scale.w);
			if (player.tailLength > 0)
			{
				for (int i = 0; i < player.tailLength; i++) {
					if ((x == player.tailX[i] && y == player.tailY[i]) || (x == player.rect.x && y == player.rect.x))
					{
						valid = false;
						break;
					}
					else 
					{
						cout << "Food spawned\n";
						valid = true;
					}
				}
			}
			else 
			{
				cout << "Food spawned\n";
				valid = true;
			}	
		}
		rect.x = x;
		rect.y = y;
	}
};
class GameManager
{
private:
	Player player;
	Food food;

	bool up = false;
	bool down = false;
	bool right = false;
	bool left = false;

	bool inputThisFrame = false;
	bool redo = false;
	float time = SDL_GetTicks() / 100;

	void gameOver()
	{
		// Отримує шрифт, який використовується для відображення тексту
		SDL_Surface* gameover = TTF_RenderText_Solid(font, "Game Over", Red);
		SDL_Surface* retry = TTF_RenderText_Solid(font, "Press Enter to retry", White);
		SDL_Surface* score = TTF_RenderText_Solid(font, (string("Score: ") + to_string(player.tailLength * 10)).c_str(), Black);
		SDL_Texture* gameoverMessage = SDL_CreateTextureFromSurface(renderer, gameover);
		SDL_Texture* retryMessage = SDL_CreateTextureFromSurface(renderer, retry);
		SDL_Texture* scoreMessage = SDL_CreateTextureFromSurface(renderer, score);
		SDL_Rect gameoverRect;
		SDL_Rect retryRect;
		SDL_Rect scoreRect;
		gameoverRect.w = 200;
		gameoverRect.h = 100;
		gameoverRect.x = ((scale.h * scale.w) / 2) - (gameoverRect.w / 2);
		gameoverRect.y = ((scale.h * scale.w) / 2) - (gameoverRect.h / 2) - 50;
		retryRect.w = 300;
		retryRect.h = 50;
		retryRect.x = ((scale.h * scale.w) / 2) - ((retryRect.w / 2));
		retryRect.y = (((scale.h * scale.w) / 2) - ((retryRect.h / 2)) + 150);
		scoreRect.w = 100;
		scoreRect.h = 30;
		scoreRect.x = ((scale.h * scale.w) / 2) - (scoreRect.w / 2);
		scoreRect.y = 0;
		SDL_RenderCopy(renderer, gameoverMessage, NULL, &gameoverRect);
		SDL_RenderCopy(renderer, retryMessage, NULL, &retryRect);
		SDL_RenderCopy(renderer, scoreMessage, NULL, &scoreRect);

		// Показує гру на екрані, поки простір не натиснуто
		while (true)
		{
			SDL_RenderPresent(renderer);

			if (SDL_PollEvent(&event))
			{
				if (event.type == SDL_QUIT)
				{
					exit(0);
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_RETURN)
				{
					return;
				}
			}
		}
	}
	void CheckWin()
	{
		if (player.tailLength >= 575)
		{
			youWin();
			player.Reset();
		}
	}
	void youWin()
	{
		SDL_Surface* gameover = TTF_RenderText_Solid(font, "You won!", Yellow);
		SDL_Surface* retry = TTF_RenderText_Solid(font, "Press Enter to play again", White);
		SDL_Surface* score = TTF_RenderText_Solid(font, (string("Score: ") + to_string(player.tailLength * 10)).c_str(), Black);
		SDL_Texture* gameoverMessage = SDL_CreateTextureFromSurface(renderer, gameover);
		SDL_Texture* retryMessage = SDL_CreateTextureFromSurface(renderer, retry);
		SDL_Texture* scoreMessage = SDL_CreateTextureFromSurface(renderer, score);
		SDL_Rect gameoverRect;
		SDL_Rect retryRect;
		SDL_Rect scoreRect;
		gameoverRect.w = 200;
		gameoverRect.h = 100;
		gameoverRect.x = ((scale.h * scale.w) / 2) - (gameoverRect.w / 2);
		gameoverRect.y = ((scale.h * scale.w) / 2) - (gameoverRect.h / 2) - 50;
		retryRect.w = 300;
		retryRect.h = 50;
		retryRect.x = ((scale.h * scale.w) / 2) - ((retryRect.w / 2));
		retryRect.y = (((scale.h * scale.w) / 2) - ((retryRect.h / 2)) + 150);
		scoreRect.w = 100;
		scoreRect.h = 30;
		scoreRect.x = ((scale.h * scale.w) / 2) - (scoreRect.w / 2);
		scoreRect.y = 0;
		SDL_RenderCopy(renderer, gameoverMessage, NULL, &gameoverRect);
		SDL_RenderCopy(renderer, retryMessage, NULL, &retryRect);
		SDL_RenderCopy(renderer, scoreMessage, NULL, &scoreRect);

		// Показує екран перемоги, поки не натиснуто пробіл
		while (true)
		{
			SDL_RenderPresent(renderer);

			if (SDL_PollEvent(&event))
			{
				if (event.type == SDL_QUIT)
				{
					exit(0);
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_RETURN)
				{
					return;
				}
			}
		}
	}
	void renderPlayer()
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		player.rect.w = scale.w;
		player.rect.h = scale.h;

		for (int i = 0; i < player.tailLength; i++)
		{
			if (player.tailX.size() == player.tailLength)
			{
				player.rect.x = player.tailX[i];
				player.rect.y = player.tailY[i];
				SDL_RenderFillRect(renderer, &player.rect);
			}
		}

		player.rect.x = player.x;
		player.rect.y = player.y;

		SDL_RenderFillRect(renderer, &player.rect);
	}
	void renderFood()
	{
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		food.rect.w = scale.w;
		food.rect.h = scale.h;
		SDL_RenderFillRect(renderer, &food.rect);
	}
	void renderScore()
	{
		SDL_Color Black = { 0, 0, 0 };

		TTF_Font* font = TTF_OpenFont((char*)"arial.ttf", 10);
		if (font == NULL) {
			cout << "Font loading error" << endl;
			return;
		}

		SDL_Surface* score = TTF_RenderText_Solid(font, (string("Score: ") + to_string(player.tailLength * 10)).c_str(), Black);
		SDL_Texture* scoreMessage = SDL_CreateTextureFromSurface(renderer, score);
		SDL_Rect scoreRect;
		scoreRect.w = 100;
		scoreRect.h = 30;
		scoreRect.x = ((scale.h * scale.w) / 2) - (scoreRect.w / 2);
		scoreRect.y = 0;
		SDL_RenderCopy(renderer, scoreMessage, NULL, &scoreRect);

		TTF_CloseFont(font);
	}
	bool checkCollision()
	{
		if (player.rect.x == food.x && player.rect.y == food.y) {
			return true;
		}
		return false;
	}
	void Render()
	{
		// Рендерить все
		renderFood();
		renderPlayer();
		renderScore();

		SDL_RenderDrawLine(renderer, 0, 0, 0, scale.h * scale.w);
		SDL_RenderDrawLine(renderer, 0, scale.h * scale.w, scale.h * scale.w, scale.h * scale.w);
		SDL_RenderDrawLine(renderer, scale.h * scale.w, scale.h * scale.w, scale.h * scale.w, 0);
		SDL_RenderDrawLine(renderer, scale.h * scale.w, 0, 0, 0);

		// Все на екран
		// Нічого не виводиться на екран, поки це не викликано
		SDL_RenderPresent(renderer);

		// Виберає колір і заповняє ним усе вікно, це скидає все до наступного кадру
		// Це також дає нам колір фону
		SDL_SetRenderDrawColor(renderer, 105, 105, 105, 255);
		SDL_RenderClear(renderer);
	}
	void Death()
	{
		if (player.rect.x < 0 || player.rect.y < 0 || player.rect.x > scale.h * scale.w - scale.h || player.rect.y > scale.h * scale.w - scale.h)
		{
			gameOver();
			player.Reset();
			up = false;
			left = false;
			right = false;
			down = false;
		}
		for (int i = 0; i < player.tailLength; i++)
		{
			if ((player.tailX.size() == player.tailLength) && (player.rect.x == player.tailX[i] && player.rect.y == player.tailY[i]))
			{
				gameOver();
				player.Reset();
				up = false;
				left = false;
				right = false;
				down = false;
			}
		}
	}
public:
	void Init()
	{
		// Починає все, щоб у нас було все
		SDL_Init(SDL_INIT_EVERYTHING);

		// Запускає TTF і перевіряє наявність помилок
		if (TTF_Init() < 0) {
			cout << "Error: " << TTF_GetError() << endl;
		}

		font = TTF_OpenFont((char*)"arial.ttf", 10);
		if (font == NULL) {
			cout << "Font loading error" << endl;
			return;
		}
		window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, scale.h * scale.w + 1, scale.h * scale.w + 1, SDL_WINDOW_RESIZABLE);
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	}
	void Deinit()
	{
		//deinit
		TTF_CloseFont(font);
		SDL_DestroyWindow(window);
		TTF_Quit();
		SDL_Quit();
	}
	void Update()
	{
		float newTime = SDL_GetTicks() / 150; //Значення (75) - це швидкість оновлення блоків
		float delta = newTime - time;
		time = newTime;
		inputThisFrame = false;

		// Елементи управління
		if (SDL_PollEvent(&event))
		{
			// Простий вихід із програми, коли вам це скажуть
			if (event.type == SDL_QUIT)
			{
				exit(0);
			}
			// Якщо натиснута клавіша
			if (event.type == SDL_KEYDOWN && inputThisFrame == false)
			{
				// Потім перевіряє натискання клавіші та відповідно змінює напрямок
				if (down == false && event.key.keysym.scancode == SDL_SCANCODE_UP)
				{
					up = true;
					left = false;
					right = false;
					down = false;
					inputThisFrame = true;
				}
				else if (right == false && event.key.keysym.scancode == SDL_SCANCODE_LEFT)
				{
					up = false;
					left = true;
					right = false;
					down = false;
					inputThisFrame = true;
				}
				else if (up == false && event.key.keysym.scancode == SDL_SCANCODE_DOWN)
				{
					up = false;
					left = false;
					right = false;
					down = true;
					inputThisFrame = true;
				}
				else if (left == false && event.key.keysym.scancode == SDL_SCANCODE_RIGHT)
				{
					up = false;
					left = false;
					right = true;
					down = false;
					inputThisFrame = true;
				}
			}
		}
		// Попередня позиція блоку гравця
		player.SetPrev();

		if (up)
		{
			player.y -= delta * scale.h;
		}
		else if (left)
		{
			player.x -= delta * scale.h;
		}
		else if (right)
		{
			player.x += delta * scale.h;
		}
		else if (down)
		{
			player.y += delta * scale.h;
		}
		// Виявляє зіткнення, чи зіткнувся з їжею?
		if (checkCollision())
		{
			// Спавнить нову їжу після її з’їдання
			food.FoodSpawn(player);
			player.tailLength++;
		}
		player.MovePlayer(delta, scale);
		Render();
		Death();
	}
	void Start()
	{
		food.FoodSpawn(player);
	}
};
int main(int argc, char* argv[])
{
	GameManager gameManager;

	//Ініцаалізація всього
	gameManager.Init();
	gameManager.Start();

	// Основний ігровий цикл,  постійно працює і постійно оновлює все
	while (true)
	{
		gameManager.Update();
	}

	//Очистка пам"яті
	gameManager.Deinit();

	return 0;
}