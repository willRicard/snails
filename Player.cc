#include "Player.h"
#include "Texture.h"

Player::Player(int id) : _texture(NULL), _score(0) {
	char path[19];
	sprintf(path, "assets/snail_%d.webp", id + 1);
	_texture = Texture::Load(path);
	_clip.x = 8;
	_clip.y = id * 128 + 8;
	_clip.w = 64;
	_clip.h = 64;
}

static float easeInOut(float t, float a, float b, float d) {
	t /= d / 2;
	if (t < 1) return b / 2 * t * t * t * t + a;
	t -= 2;
	return -b / 2 * (t * t * t * t - 2) + a;
}

void Player::Update() {
	if (_animating) {
		Uint32 current_time = SDL_GetTicks();
		float current_x = easeInOut((float)(current_time - _anim_start_time),
									(float)_initial_x,
									(float)(_target_x - _initial_x), 900.0);
		// float current_x = easeOut((float) (current_time - _anim_start_time),
		// (float) _initial_x, (float) (_target_x - _initial_x), 900.f);
		_clip.x = (int)current_x;
		if (_clip.x >= _target_x) {
			_animating = false;
			_clip.x = _target_x;
		}
	}
}
