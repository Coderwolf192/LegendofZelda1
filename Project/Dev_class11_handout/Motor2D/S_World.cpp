#include "S_World.h"
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
#include"O_FireParticles.h"
#include "S_InGameMenu.h"
#include"j1CutSceneManager.h"
#include "Video.h"
#include"ParticleManager.h"

bool S_World::Start()
{
	scene_str = "Castle Sewers";
	
	App->camera->Enable();	
	App->hud->Enable();
	App->collision->Enable();
	App->map->Enable();
	App->player->Enable();
	App->object->Enable();
	
	App->SaveGameModules("save_modules.xml");
	
	
	//PAUSE FALSE
	App->player->paused = false;
	App->enemy->paused = false;
	App->collision->paused = false;
	App->pathfinding->paused = false;

	//GuiImage* title = App->gui->CreateImage(iPoint(0, 0), &title_rec, false, AddGuiTo::none);
	//((Gui*)title)->SetListener(this);
	
	if (App->map->Load("tutorial map test.tmx") == true)
	{
		int w, h = 0;
		uchar* data = nullptr;
		if (App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h);

		for (uint i = 0; i < App->object->V_Objects.size(); i++) {
			if (App->object->V_Objects[i]->collider_tiles.size() > 0) {
				App->object->CreateColliders(*App->object->V_Objects[i]);
			}
		}

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

	
	App->audio->PlayMusic("audio/music/Sewers_Song.ogg", 0);
	App->audio->VolumeMusic(100);

	App->render->camera.x = 0;
	App->render->camera.y = 0;

	App->player->Link->pos = { 660,1400 };
	App->player->Zelda->pos = { 620,1400 };
	App->player->Link->logic_height = 1;
	App->player->Zelda->logic_height = 1;
	App->player->half_hearts_test_purpose = App->player->hearts_containers_test_purpose * 2;
	LOG("World Open");

	/*
	SDL_Rect r = { 0,0,300,300 };
	App->videoplayer->PlayVideo("video.ogv", r);
	*/

	return false;
}

bool S_World::Update()
{
	
	if (!App->player->paused)

	return false;
}

bool S_World::PostUpdate()
{


	return true;

}

bool S_World::Clean()
{
	
	LOG("World Test Deleted");

	App->player->Disable();
	App->enemy->Disable();
	App->map->Disable();
	App->collision->Disable();
	App->hud->Disable();
	App->object->Disable();

	//std::list <TileSet*>::iterator temp = App->map->data.tilesets.begin();
	
	/*p2List_item<TileSet*>* temp = App->map->data.tilesets.start;
	while (temp != NULL) {
		App->tex->UnLoad(temp->data->texture);
		temp = temp->next;
	}*/

	//This must be revised
	((S_InGameMenu*)App->scene->InGameMenuScene())->active = false;

	return false;
}

bool S_World::Save(pugi::xml_node& node) const
{
	return true;
}

bool S_World::Load(pugi::xml_node& node)
{
	return true;
}