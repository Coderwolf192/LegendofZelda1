#include "j1Enemy.h"
#include"Green_Enemy.h"
#include"j1Collision.h"
#include"j1Map.h"
#include"j1Textures.h"
bool j1Enemy::Awake(pugi::xml_node &)
{

	

	return true;
}
bool j1Enemy::Start()
{
	green_soldier_tex = App->tex->Load("sprites/green_soldier.png.png");

	for (int i = 0; i < App->map->V_Enemies.size(); i++) {
		for (int y = 0; y < App->map->data.height; ++y) {
			for (int x = 0; x < App->map->data.width; ++x) {
				if (App->map->V_Enemies[i]->Get(x, y) != 0) {
					Create_Enemy(App->map->V_Enemies[i]->Get(x, y), iPoint(x, y));

				}
			}
		}
	}


	

	return true;
}

bool j1Enemy::PreUpdate()
{
	return true;
}

bool j1Enemy::Update(float dt)
{
	for (int i = 0; i < V_MyEnemies.size(); i++) {
		V_MyEnemies[i]->collider->rect.x = V_MyEnemies[i]->pix_world_pos.x+17;
		V_MyEnemies[i]->collider->rect.y = V_MyEnemies[i]->pix_world_pos.y +10;
		//V_MyEnemies[i]->shield_test->SetPos(V_MyEnemies[i]->collider->rect.x-23, V_MyEnemies[i]->collider->rect.y + 30, V_MyEnemies[i]->logic_height);
		Update_Sword_Collision(V_MyEnemies[i]);
		App->render->Blit(green_soldier_tex, V_MyEnemies[i]->pix_world_pos.x, V_MyEnemies[i]->pix_world_pos.y, &V_MyEnemies[i]->rect);
		V_MyEnemies[i]->Action();
		
	}


	return true;
}

bool j1Enemy::PostUpdate()
{

	return true;
}

bool j1Enemy::CleanUp()
{
	return true;
}

Enemy* j1Enemy::Create_Enemy(uint id_enemy, iPoint pos_array_enemy)
{
	Enemy* ret = nullptr;
	
	switch (id_enemy) {
	case enemyType::green_enemy:
		ret = new Green_Enemy();
		ret->rect = { 0,0,44,60 };
		break;

	}

		//This will not be usefull when the enemies will be readed from xml

	//Position in array
	ret->array_pos = pos_array_enemy;

	SDL_Rect rect_test = { ret->array_pos.x,ret->array_pos.y,20,10 };
	ret->shield_test = App->collision->AddCollider(rect_test, COLLIDER_TYPE::collider_enemy_sword, ret, this);


	//Position in world pixel 
		ret->pix_world_pos.x = pos_array_enemy.x*App->map->data.tile_width;
		ret->pix_world_pos.y = pos_array_enemy.y*App->map->data.tile_height;

		SDL_Rect rect = { ret->pix_world_pos.x+16, ret->pix_world_pos.y+32,16,42 };
		ret->collider = App->collision->AddCollider(rect, COLLIDER_TYPE::collider_enemy, (Entity*)ret, App->enemy);
		ret->logic_height = 1;
		
		V_MyEnemies.push_back(ret);


	return ret;
}

iPoint j1Enemy::CalculatePath(Enemy* enemy)
{
	if (FindInPath(enemy->array_pos, enemy) == false) {
		enemy->Path_Enemy.push_back(enemy->array_pos);
	}
	iPoint cell;
	for (int i = 0; i < App->map->V_PathEnemies.size(); i++) {
		if (enemy->movable == true) {
			cell.create(enemy->array_pos.x, enemy->array_pos.y + 1);
			if (App->map->V_PathEnemies[i]->Get(cell.x, cell.y) != 0 && FindInPath(cell, enemy) == false) {
				enemy->Path_Enemy.push_back(cell);
				return cell;
			}

			cell.create(enemy->array_pos.x, enemy->array_pos.y - 1);
			if (App->map->V_PathEnemies[i]->Get(cell.x, cell.y) != 0 && FindInPath(cell, enemy) == false) {
				enemy->Path_Enemy.push_back(cell);
				return cell;
			}

			cell.create(enemy->array_pos.x + 1, enemy->array_pos.y);
			if (App->map->V_PathEnemies[i]->Get(cell.x, cell.y) != 0 && FindInPath(cell, enemy) == false) {
				enemy->Path_Enemy.push_back(cell);
				return cell;
			}

			cell.create(enemy->array_pos.x - 1, enemy->array_pos.y);
			if (App->map->V_PathEnemies[i]->Get(cell.x, cell.y) != 0 && FindInPath(cell, enemy) == false) {
				enemy->Path_Enemy.push_back(cell);
				return cell;
			}

		}
	}
	if (enemy->Path_Enemy.size()>1) {
		enemy->Path_Enemy.clear();
	}
	else {
		enemy->movable = false;
	}
	return enemy->array_pos;
}

bool j1Enemy::FindInPath(iPoint pos, Enemy* enemy) {

	for (std::list<iPoint>::iterator item = enemy->Path_Enemy.begin(); item != enemy->Path_Enemy.cend(); ++item) {
		if ((*item) == pos) {
			return true;
		}
	}
	return false;
}

void j1Enemy::Update_Sword_Collision(Enemy* enemy)
{

	switch (enemy->Enemy_Orientation) {
		//	V_MyEnemies[i]->shield_test->SetPos(V_MyEnemies[i]->collider->rect.x - 23, V_MyEnemies[i]->collider->rect.y + 30, V_MyEnemies[i]->logic_height);

	case OrientationEnemy::up_enemy:
		//enemy->shield_test->SetPos(enemy->collider->rect.x + 30, enemy->collider->rect.y - 23, enemy->logic_height);
		enemy->shield_test->rect = { enemy->collider->rect.x, enemy->collider->rect.y - enemy->collider->rect.h+10 , 10,20 };
		break;

	case OrientationEnemy::down_enemy:
		
		enemy->shield_test->rect = { enemy->collider->rect.x, enemy->collider->rect.y + enemy->collider->rect.h , 10,20};
		//enemy->shield_test->SetPos(enemy->collider->rect.x+30, enemy->collider->rect.y - 23, enemy->logic_height);
		break;

	case OrientationEnemy::right_enemy:
		enemy->shield_test->rect = { enemy->collider->rect.x + 30, enemy->collider->rect.y + 23 , 20,10 };
		//enemy->shield_test->SetPos(enemy->collider->rect.x + 23, enemy->collider->rect.y + 30, enemy->logic_height);
		break;

	case OrientationEnemy::left_enemy:
		
		//20,10
		enemy->shield_test->rect = { enemy->collider->rect.x - 23, enemy->collider->rect.y + 30 , 20,10 };
		//enemy->shield_test->SetPos(enemy->collider->rect.x - 23, enemy->collider->rect.y + 30, enemy->logic_height);
		break;



	}
}

