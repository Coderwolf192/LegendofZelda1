#ifndef _STATUE_ENEMY_
#define _STATUE_ENEMY_
#include "j1Enemy.h"
#include"j1Render.h"

enum state_statue {
	state1,
	state2,
	state3
};

class Statue_Enemy : public Enemy {
public:
	Statue_Enemy();
	Statue_Enemy(const Statue_Enemy&);
	~Statue_Enemy();
	void Action();
	//void Rang_Player();
public:
	iPoint temp_point = { 0,0 };
	state_statue state_statue = state1;
	float t = 0;
	iPoint first_point = {0,0};
	iPoint last_point = { 0,0 };
	SDL_Rect rect_en = { 0,0,0,0 };
	int new_y = 0;
};


#endif
