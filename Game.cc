#include "Game.h"
#include <cmath>
#include <ctime>
#include "Texture.h"

SDL_Renderer *_renderer = NULL;

Game::Game() : _window(NULL), _players(4) {
	_gamestate.state = STATE_MENU;
	_gamestate.players = 1;

	srand(time(0));
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_INFO);
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	IMG_Init(IMG_INIT_PNG | IMG_INIT_WEBP);
	Mix_Init(MIX_INIT_MP3);

	_window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED,
							   SDL_WINDOWPOS_CENTERED, 640, 480, 0);
	_renderer = SDL_CreateRenderer(
		_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 1024);

	// Main Menu
	_main_menu = Texture::Load("assets/menu.webp");

	_menu_cursor = Texture::Load("assets/cursor.webp");
	_cursor_clip.x = 160;
	_cursor_clip.y = 146;
	_cursor_clip.w = 32;
	_cursor_clip.h = 32;

	// Instructions
	_instructions = Texture::Load("assets/instructions.webp");

	// Playing
	SDL_Surface *bg =
		SDL_CreateRGBSurface(0, WINDOW_WIDTH, WINDOW_HEIGHT, 24, 0, 0, 0, 0);
	SDL_FillRect(bg, NULL, 0x880088);
	SDL_Rect rect = {512, 0, 1, 480};
	SDL_FillRect(bg, &rect, 0xffffff);
	_background = SDL_CreateTextureFromSurface(_renderer, bg);
	SDL_FreeSurface(bg);

	// Credits
	_credits = Texture::Load("assets/credits.webp");
	_credits_src_clip.x = 0;
	_credits_src_clip.y = 0;
	_credits_src_clip.w = WINDOW_WIDTH;
	_credits_src_clip.h = WINDOW_HEIGHT;
	_credits_dest_clip = _credits_src_clip;
	_credits_music = Mix_LoadMUS("assets/credits.ogg");
	if (!_credits_music) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR,
					 "Error while loading music assets/credits.ogg: %s",
					 Mix_GetError());
		exit(1);
	}
}

Game::~Game() {
	SDL_DestroyTexture(_main_menu);
	SDL_DestroyTexture(_menu_cursor);
	SDL_DestroyTexture(_instructions);
	SDL_DestroyTexture(_background);
	SDL_DestroyTexture(_credits);
	Mix_FreeMusic(_credits_music);
	SDL_DestroyRenderer(_renderer);
	SDL_DestroyWindow(_window);
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}

void Game::Run() {
	DrawMenu();

	bool running = true;
	SDL_Event event;
	Uint32 current_time, last_time = 0;
	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT ||
				(event.type == SDL_KEYDOWN &&
				 event.key.keysym.sym == SDLK_ESCAPE)) {
				running = false;
			}
			if (_gamestate.state == STATE_MENU && event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
					case SDLK_RETURN:
						SetState(STATE_INSTRUCTIONS);
						break;
					case SDLK_DOWN:
						if (_gamestate.players < 4) {
							_cursor_clip.y += 48;
							RedrawCursor();
							_gamestate.players++;
						}
						break;
					case SDLK_UP:
						if (_gamestate.players > 1) {
							_cursor_clip.y -= 48;
							RedrawCursor();
							_gamestate.players--;
						}
						break;
					case SDLK_i:
						SetState(STATE_CREDITS);
						Mix_PlayMusic(_credits_music, 1);
					default:
						break;
				}
			} else if (_gamestate.state == STATE_INSTRUCTIONS) {
				if (event.type == SDL_KEYDOWN) {
					SetState(STATE_PLAYING);
					current_time = SDL_GetTicks();
					last_time = current_time;
				}
			} else if (_gamestate.state == STATE_PLAYING) {
				if (event.type == SDL_KEYDOWN &&
					current_time - last_time < 5000) {
					switch (event.key.keysym.sym) {
						case SDLK_q:
							IncrPlayerScore(0);
							break;
						case SDLK_f:
							IncrPlayerScore(1);
							break;
						case SDLK_j:
							IncrPlayerScore(2);
							break;
						case SDLK_m:
							IncrPlayerScore(3);
							break;
						default:
							break;
					}
				}
			}
		}

		if (_gamestate.state == STATE_PLAYING) {
			current_time = SDL_GetTicks();
			if (current_time - last_time > 5000 &&
				current_time - last_time < 5100) {
				unsigned int score, max_score = 0;
				unsigned int i;
				const unsigned int l = _gamestate.players;
				for (i = 0; i < l; i++) {
					score = _players[i]->getScore();
					if (score > max_score) {
						max_score = score;
					}
				}
				double d_max_score = (double)max_score;
				for (i = 0; i < l; i++) {
					double travelled_distance =
						(((double)_players[i]->getScore()) / d_max_score) *
						128.0;
					if (!isnan(travelled_distance)) {
						_players[i]->Move(travelled_distance);
					}
				}
			} else if (current_time - last_time > 5100 &&
					   current_time - last_time < 6000) {
				unsigned int i;
				const unsigned int l = _gamestate.players;
				for (i = 0; i < l; i++) {
					_players[i]->Update();
				}
				DrawGame();
				for (i = 0; i < l; i++) {
					if (_players[i]->getClip()->x > 448) {
						char text[17];
						sprintf(text, "Player %d has won!", i + 1);
						SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
												 "You won!", text, _window);
						SetState(STATE_MENU);
						i = l;
					}
				}
			} else if (current_time - last_time > 6000) {
				last_time = current_time;
			}
		} else if (_gamestate.state == STATE_CREDITS) {
			if (_credits_src_clip.y < 520) {
				_credits_src_clip.y++;
				if (_credits_src_clip.y > 40) {
					_credits_src_clip.h--;
					_credits_dest_clip.h--;
				}
				DrawCredits();
			} else {
				SetState(STATE_MENU);
			}
		}
		SDL_Delay(16);
	}
}

inline void Game::DrawMenu() {
	SDL_RenderCopy(_renderer, _main_menu, NULL, NULL);
	SDL_RenderCopy(_renderer, _menu_cursor, NULL, &_cursor_clip);
	SDL_RenderPresent(_renderer);
}

inline void Game::RedrawCursor() {
	SDL_RenderCopy(_renderer, _main_menu, NULL, NULL);
	SDL_RenderCopy(_renderer, _menu_cursor, NULL, &_cursor_clip);
	SDL_RenderPresent(_renderer);
}

inline void Game::DrawGame() {
	SDL_RenderCopy(_renderer, _background, NULL, NULL);
	unsigned int i;
	const unsigned int l = _gamestate.players;
	for (i = 0; i < l; i++) {
		SDL_RenderCopy(_renderer, _players[i]->getTexture(), NULL,
					   _players[i]->getClip());
	}
	SDL_RenderPresent(_renderer);
}

inline void Game::IncrPlayerScore(unsigned int id) {
	if (_gamestate.players >= id + 1) {
		_players[id]->IncrScore();
	}
}

inline void Game::DrawCredits() {
	SDL_RenderCopy(_renderer, _credits, &_credits_src_clip,
				   &_credits_dest_clip);
	SDL_RenderPresent(_renderer);
}

void Game::SetState(State state) {
	_gamestate.state = state;
	if (state == STATE_MENU) {
		DrawMenu();
	} else if (state == STATE_INSTRUCTIONS) {
		SDL_RenderCopy(_renderer, _instructions, NULL, NULL);
		SDL_RenderPresent(_renderer);
	} else if (state == STATE_PLAYING) {
		unsigned int i;
		const unsigned int l = _gamestate.players;
		for (i = 0; i < l; i++) {
			if (!_players[i]) {
				_players[i] = new Player(i);
			} else {
				_players[i]->Reset();
			}
		}
		DrawGame();
	}
}
