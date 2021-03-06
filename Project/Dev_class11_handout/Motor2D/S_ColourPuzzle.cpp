#include "S_ColourPuzzle.h"
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
#include "O_DoubleButton.h"
bool S_ColourPuzzle::Start()
{
	scene_str = "Van Ruhda Quest";

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


	if (App->map->Load("Colours Puzzle.tmx") == true)

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

	App->audio->PlayMusic("audio/music/Dungeon.ogg", 0);
	App->audio->VolumeMusic(100);

	App->render->camera.x = 0;
	App->render->camera.y = 0;

	for (uint i = 0; i < App->object->V_Objects.size(); i++) {
		if (App->object->V_Objects[i]->type == objectType::double_button) {
			DoubleButton* temp = (DoubleButton*)App->object->V_Objects[i];
			temp->idle_button = rect_double_red_button_idle;
			temp->pressed_button = rect_double_red_button_one;
			temp->double_pressed_button = rect_double_red_button_pressed;
		}
	}
	//App->player->Link->pos = { 660,1200 };
	//App->player->Zelda->pos = { 620,1200 };
	App->player->Link->logic_height = 0;
	App->player->Zelda->logic_height = 0;
	

	for (int i = 0; i < App->object->V_Objects.size(); i++) {
		if (App->object->V_Objects[i]->type == objectType::door) {
			App->object->V_Objects[i]->texture_rect = { 948,0,40,64 };
			Frame temp_frame;
			temp_frame.rect = { 0, 0, 0, 0 };
			temp_frame.pivot = { 0,0 };
			for (int n = 0; n < 25; n++) {
				App->object->V_Objects[i]->object_animation.frames[n] = temp_frame;
			}
			App->object->V_Objects[i]->object_animation.last_frame = 0;
			for (int n = 0; n < 6; n++) {
				temp_frame.rect = { 948, 0 + n*64, 40, 64 };
				App->object->V_Objects[i]->object_animation.PushBack(temp_frame);
			}
		}
	}

	LOG("World Open");
	return false;
}

bool S_ColourPuzzle::Update()
{
	
	if (!App->player->paused)
		
	return false;
}

bool S_ColourPuzzle::PostUpdate()
{
	for (int i = 0; i < App->object->V_Objects.size(); i++) {
		if (App->object->V_Objects[i]->type == objectType::colour_blocks) {
			ColourBlock* temp_block = (ColourBlock*)App->object->V_Objects[i];
			if (temp_block->block_delay.Read() > 2000 && temp_block->can_dissapear)
				App->object->V_Objects[i]->active = false;
		}
	}

	


	return true;


}

bool S_ColourPuzzle::Clean()
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
