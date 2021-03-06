#include "S_TopOfTheMountain.h"
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
#include "O_ColourBlock.h"
#include "j1CutSceneManager.h"
#include "Video.h"

bool S_TopOfTheMountain::Start()
{
	scene_str = "Top of The Mountain";

	App->camera->Enable();
	App->hud->Enable();
	App->collision->Enable();
	App->map->Enable();
	App->player->Enable();
	App->object->Enable();




	//PAUSE FALSE
	App->player->paused = false;
	App->enemy->paused = false;
	App->collision->paused = false;
	App->pathfinding->paused = false;
	

	if (App->map->Load("top_of_the_mountain.tmx") == true)

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

	temp = true;
	cutscene = false;
	App->audio->PlayMusic("audio/music/Top-of-the-Mountain.ogg", 0);
	App->audio->VolumeMusic(100);

	App->render->camera.x = 0;
	App->render->camera.y = 0;

	//App->player->Link->pos = { 660,1200 };
	//App->player->Zelda->pos = { 620,1200 };
	App->cutscenemanager->StartCutscene(2);
	
	LOG("World Open");
	return false;
}

bool S_TopOfTheMountain::Update()
{
	
	if (!App->player->paused)

		return false;
}

bool S_TopOfTheMountain::PostUpdate()
{

	
	
	if (App->cutscenemanager->FinishCutscene() && temp) {
		SDL_Rect r = { 0,0,1280,720 };
		App->videoplayer->video_finished = false;
		App->videoplayer->PlayVideo("Top-of-the-mountain-video.ogv", r);
		App->videoplayer->video_finished = false;
		cutscene = true;
		temp = false;

	}
	if (App->videoplayer->video_finished && cutscene) {
		App->audio->RestartAudio();
		App->cutscenemanager->StartCutscene(4);		
		App->audio->PlayMusic("audio/music/Dungeon.ogg", 0);
		cutscene = false;
	}



	return true;


}

bool S_TopOfTheMountain::Clean()
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
