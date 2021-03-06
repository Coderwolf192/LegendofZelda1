#include "S_CastleCutScene.h"
#include "j1Input.h"
#include "SDL\include\SDL.h"
#include "j1Scene.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Map.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Pathfinding.h"
#include "j1App.h"
#include "j1Player.h"
#include "j1Camera.h"
#include "j1HUD.h"
#include "j1Enemy.h"
#include"j1Audio.h"
#include"j1GameStartMenuBack.h"
#include "S_InGameMenu.h"

bool S_CastleCutScene::Start()
{
	scene_str = "Castle Sewers";

	App->camera->Enable();
	App->hud->Enable();
	App->collision->Enable();
	App->map->Enable();
	App->player->Enable();
	App->object->Enable();

	App->scene->blocks_out = false;
	App->scene->heart_1 = true;
	App->scene->heart_2 = true;
	App->scene->heart_3 = true;
	App->scene->heart_4 = true;
	App->scene->heart_5 = true;


	//PAUSE FALSE
	App->player->paused = false;
	App->enemy->paused = false;
	App->collision->paused = false;
	App->pathfinding->paused = false;


	if (App->map->Load("sala del rey.tmx") == true)
	{
		int w, h = 0;
		uchar* data = nullptr;
		if (App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h);

		int w_two, h_two = 0;
		uchar* data_two = nullptr;
		if (App->map->CreateEnemyMap(w_two, h_two, &data_two)) {
			int w_three = 0;
			int h_three = 0;
			uchar* data_three = nullptr;
			if (App->map->CreateEnemyPathMap(w_three, h_three, &data_three)) {
				App->enemy->Enable();
			}
		}
		RELEASE_ARRAY(data);
		//App->map->CreateLogicMap();
	}
	
	//Commit number 1000 :)

	App->audio->PlayMusic("audio/music/Sewers_Song.ogg", 0);
	App->audio->VolumeMusic(100);

	App->render->camera.x = 0;
	App->render->camera.y = 0;
	if (App->cutscenemanager->bool_done_cutscenes[0]==false) {
		App->player->Link->pos = { 433,832 };
		App->player->Link->character_direction = direction::right;
		App->player->Zelda->pos = { 775,814 };
	}
	else {
		App->player->Link->pos = { 1400,450 };
		App->player->Zelda->pos = { 1500,450 };
	}
	App->player->Link->logic_height = 0;
	App->player->Zelda->logic_height = 0;
	App->player->half_hearts_test_purpose = App->player->hearts_containers_test_purpose * 2;
	App->cutscenemanager->StartCutscene(0);
	
	LOG("World Open");
	return true;
}

bool S_CastleCutScene::Update()
{
	return false;
}

bool S_CastleCutScene::PostUpdate()
{

	return false;
}

bool S_CastleCutScene::Clean()
{

	App->player->Disable();
	App->enemy->Disable();
	App->map->Disable();
	App->collision->Disable();
	App->hud->Disable();
	App->object->Disable();

	((S_InGameMenu*)App->scene->InGameMenuScene())->active = false;
	return false;
}
