#pragma once
#include <vector>
#include "Player.h"
#include "snails.h"

enum State { STATE_MENU, STATE_INSTRUCTIONS, STATE_PLAYING, STATE_CREDITS };

typedef struct {
	unsigned int players;
	State state;
} GameState;

class Game {
public:
	Game();
	~Game();
	void Run();

private:
	GameState _gamestate;
	SDL_Window *_window;

	void SetState(State state);

	// Main Menu
	SDL_Texture *_main_menu;
	SDL_Texture *_menu_cursor;
	SDL_Rect _cursor_clip;

	inline void DrawMenu();
	inline void RedrawCursor();

	// Instructions
	SDL_Texture *_instructions;

	// Playing
	SDL_Texture *_background;
	std::vector<Player *> _players;

	inline void CreateGame();
	inline void DrawGame();
	inline void IncrPlayerScore(unsigned int id);

	// Credits
	SDL_Texture *_credits;
	SDL_Rect _credits_src_clip;
	SDL_Rect _credits_dest_clip;
	Mix_Music *_credits_music;
	inline void DrawCredits();
};
