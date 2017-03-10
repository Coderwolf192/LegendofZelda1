#ifndef _J1PLAYER_
#define _J1PLAYER_

#include "j1Module.h"
#include "p2Point.h"
#include "MainScene.h"
#include "P_Link.h"
#include "P_Zelda.h"

#include "j1Map.h"


enum key_state {
	up,
	down,
	left,
	right,
	left_up,
	left_down,
	right_up,
	right_down,
	idle
};

enum movement_animation {
	animation_up,
	animation_down,
	animation_left,
	animation_right,
	animation_idle_up,
	animation_idle_down,
	animation_idle_left,
	animation_idle_right,
};

class P_Link;
class P_Zelda;
class Character;




class j1Player :public j1Module {
public:
	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	void Draw();

	void ActivatePathfinding();
	bool Move_Camera();
	

public:
	
	bool change = false;
	MainScene* actual_scene;
	bool chase = false;
	bool cooperative = false;
	Character* selected_character;
	Character* other_character;
	P_Link* Link;
	P_Zelda* Zelda;

	//Colisions
	MapLayer* Colision;
	
	int live;

};



#endif
