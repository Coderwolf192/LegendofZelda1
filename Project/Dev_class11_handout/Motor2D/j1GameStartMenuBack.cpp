#include <math.h>
#include "j1Player.h"
#include "j1Enemy.h"
#include "j1Collision.h"
#include "j1Pathfinding.h"
#include "p2Log.h"
#include "j1GameStartMenuBack.h"
#include "S_MainMenu.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Gui.h"
#include "GuiImage.h"
#include "GuiButton.h"
#include "j1InputManager.h"
#include "j1CutSceneManager.h"
#include "j1Audio.h"

j1GameStartMenuBackground::j1GameStartMenuBackground()
{
	name = "gamestartmenubackground";
}

j1GameStartMenuBackground::~j1GameStartMenuBackground()
{

}

// Called before render is available
bool j1GameStartMenuBackground::Awake(pugi::xml_node& conf)
{
	LOG("Loading Game Start Menu Background atlas");
	bool ret = true;

	atlas_file_name = conf.child("atlas").attribute("file").as_string("");

	return ret;
}

// Called before the first frame
bool j1GameStartMenuBackground::Start()
{
	atlas = App->tex->Load(atlas_file_name.c_str());

	Background_Initial_pos = App->win->GetWindowH() - background.h;

	BackgroundCharacterList.push_back(new BackgroundCharacter(&LinkRect, { (int)App->win->GetWindowW(),0 }));
	BackgroundCharacterList.push_back(new BackgroundCharacter(&ZeldaRect, { BackgroundCharacterList.back()->position.x + BackgroundCharacterList.back()->rect.w + spacebetweenbackgroundcharacters,0 }));
	BackgroundCharacterList.push_back(new BackgroundCharacter(&MessengerRect, { BackgroundCharacterList.back()->position.x + BackgroundCharacterList.back()->rect.w + spacebetweenbackgroundcharacters,26 }));

	Background_timer.Start();

	return true;
}

// Called each loop iteration
bool j1GameStartMenuBackground::PreUpdate()
{
	if (App->scene->AnyWorldActive() && !App->cutscenemanager->CutsceneReproducing())
	{
		if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || (App->input->GetControllerButton(0, 6) == KEY_DOWN) || (App->input->GetControllerButton(1, 6) == KEY_DOWN)) {
			ingamenushow = !ingamenushow;
		}
		if (((App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) || (App->input->GetControllerButton(0, 6) == KEY_DOWN) || (App->input->GetControllerButton(1, 6) == KEY_DOWN)) && (ingamenushow))
		{
			App->scene->Show(Scene_ID::ingamemenu);
			Freeze(false);
			App->player->paused = true;
			App->enemy->paused = true;
			App->collision->paused = true;
			App->pathfinding->paused = true;
		}
		if (((App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) || (App->input->GetControllerButton(0, 6) == KEY_DOWN) || (App->input->GetControllerButton(1, 6) == KEY_DOWN)) && (!ingamenushow))
		{
			App->scene->Hide();
			Freeze(true);
			App->player->paused = false;
			App->enemy->paused = false;
			App->collision->paused = false;
			App->pathfinding->paused = false;
		}
	}
	return true;
}

// Called each loop iteration
bool j1GameStartMenuBackground::Update(float dt)
{
	if (!freeze)
	{
		//Blit background
		int Background_Y_Pos = -App->render->camera.y + Background_Initial_pos + Background_pos;
		App->render->Blit(App->gui->GetAtlas(), -App->render->camera.x + 1, Background_Y_Pos, &background, 1.0f, 0, INT_MAX, INT_MAX, false);
		//Blit background
		if (Background_Y_Pos < -1)
		{
			if (Background_timer.Read() > Background_speed)
			{
				Background_pos += 2;
				Background_timer.Start();
			}
		}
		else
			if (!firstloop)
			{
				if (!((S_MainMenu*)App->scene->GetActiveScene())->visibility)
				{
					((GuiImage*)((S_MainMenu*)App->scene->GetActiveScene())->title)->SetVisible(true);
					((GuiImage*)((S_MainMenu*)App->scene->GetActiveScene())->title)->SetOpacity(startmenuopacity);
					for (std::vector<GuiButton*>::iterator item = ((S_MainMenu*)App->scene->GetActiveScene())->buttons.begin(); item != ((S_MainMenu*)App->scene->GetActiveScene())->buttons.cend(); ++item)
					{
						(*item)->SetVisible(true);
						(*item)->SetOpacity(startmenuopacity);
					}
					((S_MainMenu*)App->scene->GetActiveScene())->visibility = true;
					firstloop = true;
					MainMenuOpacity_timer.Start();
				}
			}
			else
			{
				if ((MainMenuOpacity_timer.Read() > 10) && (startmenuopacity < 255))
				{
					startmenuopacity += 5;
					if (startmenuopacity > 255)
						startmenuopacity = 255;
					for (std::vector<GuiButton*>::iterator item = ((S_MainMenu*)App->scene->GetActiveScene())->buttons.begin(); item != ((S_MainMenu*)App->scene->GetActiveScene())->buttons.cend(); ++item)
						(*item)->SetOpacity(startmenuopacity);
					((GuiImage*)((S_MainMenu*)App->scene->GetActiveScene())->title)->SetOpacity(startmenuopacity);
					if (!activate_background_movement)
					{
						background_movement = true;
						Background_Characters_timer.Start();
					}
					MainMenuOpacity_timer.Start();
				}
			}
	}
	//Blit characters
	if (background_movement)
	{
		if (!freeze)
			for (std::list<BackgroundCharacter*>::iterator item = BackgroundCharacterList.begin(); item != BackgroundCharacterList.cend(); ++item)
				App->render->Blit(App->gui->GetAtlas(), -App->render->camera.x + (*item)->position.x, -App->render->camera.y + (*item)->position.y, &(*item)->rect, 1.0f, 0, INT_MAX, INT_MAX, false);

		if (Background_Characters_timer.Read() > backgroundcharactersspeed)
			for (std::list<BackgroundCharacter*>::iterator item = BackgroundCharacterList.begin(); item != BackgroundCharacterList.cend(); ++item)
				(*item)->position.x -= 1;
		if (BackgroundCharacterList.front()->position.x < -820)
		{
			BackgroundCharacter* toback = BackgroundCharacterList.front();
			BackgroundCharacterList.pop_front();
			toback->position.x = BackgroundCharacterList.back()->position.x + BackgroundCharacterList.back()->rect.w + spacebetweenbackgroundcharacters;
			BackgroundCharacterList.push_back(toback);
		}
	}

	return true;
}

// Called each loop iteration
bool j1GameStartMenuBackground::PostUpdate()
{
	return true;
}

// Called before quitting
bool j1GameStartMenuBackground::CleanUp()
{
	App->tex->UnLoad(atlas);
	for (std::list<BackgroundCharacter*>::iterator item = BackgroundCharacterList.begin(); item != BackgroundCharacterList.cend(); ++item)
		RELEASE(*item);
	BackgroundCharacterList.clear();
	return true;
}

void j1GameStartMenuBackground::Freeze(bool freeze)
{
	ingamenushow = !freeze;
	this->freeze = freeze;
}