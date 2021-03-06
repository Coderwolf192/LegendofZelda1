#ifndef _J1PLAYER_
#define _J1PLAYER_

#include "j1Module.h"
#include "p2Point.h"
#include "MainScene.h"
#include "P_Link.h"
#include "P_Zelda.h"

#include "j1Map.h"

class P_Link;
class P_Zelda;
class Character;

class j1Player :public j1Module {
public:
	
	j1Player() {
		name = "player";
	}
	~j1Player() {};
	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	bool Sleep();

	void Draw(int height, int y_pos);

	void ActivatePathfinding();
	bool Move_Camera();
	void OnCollision(Collider* collider1, Collider* collider2);
	void EndCollision(Collider* collider1, Collider* collider2);
	void StartCollision(Collider* collider1, Collider* collider2);

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

public:
	
	bool change = false;
	MainScene* actual_scene = nullptr;
	bool chase = false;
	bool cooperative = true;
	Character* selected_character = nullptr;
	Character* other_character = nullptr;

	j1Timer Audio_Fx_Timer;
	uint Fall_Players_Audio = 0;
	P_Link* Link = nullptr;
	P_Zelda* Zelda = nullptr;

	bool loop_game_menu = false;

	//Colisions
	MapLayer* Colision = nullptr;
	

	bool god_mode = false;
	//--------------------------------------------------------------------------------------//
	//--------------------------------This must be in player--------------------------------//
	//--------------------------------------------------------------------------------------//
	int hearts_containers_test_purpose = 4;	//Total half hearts that the player have, how many FULL hearts containers have
	int half_hearts_test_purpose = 8; //Current half hearts life
									   //Example
									   // half_hearts_test_purpose = 6, means that you have 3 heart containers
									   // half_hearts_life_test_purpose = 3, means that you have 1,5 hearts out of 3 full ones

};



#endif
