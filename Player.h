#pragma once
#include "snails.h"

class Player {
public:
	Player(int id);
	inline ~Player() { SDL_DestroyTexture(_texture); };
	inline void Move(double distance) {
		_score = 0;
		_animating = true;
		_initial_x = (double)_clip.x;
		_target_x = (double)(_clip.x + (int)distance);
		_anim_start_time = SDL_GetTicks();
	};
	void Update();
	inline void Reset() { _clip.x = 8; };
	inline void IncrScore() { _score++; };
	inline int getScore() const { return _score; };
	inline SDL_Texture *getTexture() { return _texture; };
	inline SDL_Rect *getClip() { return &_clip; };

private:
	SDL_Texture *_texture;
	SDL_Rect _clip;
	int _score;

	bool _animating;
	double _initial_x;
	double _target_x;
	Uint32 _anim_start_time;
};
