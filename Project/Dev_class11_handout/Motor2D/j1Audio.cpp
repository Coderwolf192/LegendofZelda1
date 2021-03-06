#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1FileSystem.h"
#include "j1Audio.h"

#include "j1Player.h"
#include "SDL/include/SDL.h"
#include "SDL_mixer\include\SDL_mixer.h"
#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

j1Audio::j1Audio() : j1Module()
{
	music = NULL;
	name  = "audio";
}

// Destructor
j1Audio::~j1Audio()
{}

// Called before render is available
bool j1Audio::Awake(pugi::xml_node& config)
{
	LOG("Loading Audio Mixer");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		LOG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		active = false;
		ret = true;
	}

	// load support for the JPG and PNG image formats
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if((init & flags) != flags)
	{
		LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		active = false;
		ret = true;
	}

	//Initialize SDL_mixer
	if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		LOG("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		active = false;
		ret = true;
	}



	


	return ret;
}

void j1Audio::RestartAudio()
{
	Mix_CloseAudio();
	Mix_Quit();
	SDL_CloseAudio();

	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	LOG("Loading Audio Mixer");

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		LOG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		active = false;
	}

	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if ((init & flags) != flags)
	{
		LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		active = false;
	}

	//Initialize SDL_mixer
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		LOG("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		active = false;
	}
}


bool j1Audio::Start()
{
	button_sound = App->audio->LoadFx("audio/fx/button.wav");
	secret_sound = App->audio->LoadFx("audio/fx/secret.wav");
	App->player->Link->Link_Hurt_Audio = App->audio->LoadFx("audio/fx/link hurt.wav");
	App->player->Link->Link_Sword_Audio = App->audio->LoadFx("audio/fx/fighter sword 1.wav");
	App->player->Link->Link_Sword_Collides_Sword_Audio = App->audio->LoadFx("audio/fx/sword shine 1.wav");
	App->player->Zelda->Throw_Audio = App->audio->LoadFx("audio/fx/throw.wav");
	App->player->Fall_Players_Audio = App->audio->LoadFx("audio/fx/fall.wav");
	App->player->Zelda->Arrow_Audio = App->audio->LoadFx("audio/fx/arrow 2.wav");
	App->player->Zelda->Arrow_Hit_Wall_Audio = App->audio->LoadFx("audio/fx/arrow hit wall.wav");
	App->enemy->enemy_dies_audio = App->audio->LoadFx("audio/fx/enemy dies.wav");
	App->enemy->enemy_pathfinding_audio = App->audio->LoadFx("audio/fx/soldier.wav");

	boss_defeated_sound = App->audio->LoadFx("audio/fx/boss defeated.WAV");
	boss_hit_sound = App->audio->LoadFx("audio/fx/boss-hit.wav");
	boss_invencible_sound = App->audio->LoadFx("audio/fx/boss-invencible.wav");
	boss_laser_sound = App->audio->LoadFx("audio/fx/boss_laser_countdown.wav");
	boss_dies_sound = App->audio->LoadFx("audio/fx/boss_dies.wav");

	return true;
}

// Called before quitting
bool j1Audio::CleanUp()
{
	if(!active)
		return true;

	LOG("Freeing sound FX, closing Mixer and Audio subsystem");

	if(music != NULL)
	{
		//Mix_FreeMusic(music);
	}

	for (std::list<Mix_Chunk*>::iterator item = fx.begin(); item != fx.cend(); ++item)
		Mix_FreeChunk((*item));

	fx.clear();

	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	return true;
}

// Play a music file
bool j1Audio::PlayMusic(const char* path, float fade_time)
{
	bool ret = true;

	if(!active)
		return false;

	if(music != nullptr)
	{
		if(fade_time > 0.0f)
		{
			Mix_FadeOutMusic(int(fade_time * 1000.0f));
		}
		else
		{
			Mix_HaltMusic();
		}

		// this call blocks until fade out is done
		Mix_FreeMusic(music);
	}

	music = Mix_LoadMUS_RW(App->fs->Load(path), 1);

	if(music == nullptr)
	{
		LOG("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());
		ret = false;
	}
	else
	{
		if(fade_time > 0.0f)
		{
			if(Mix_FadeInMusic(music, -1, (int) (fade_time * 1000.0f)) < 0)
			{
				LOG("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
		else
		{
			if(Mix_PlayMusic(music, -1) < 0)
			{
				LOG("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
	}

	LOG("Successfully playing %s", path);
	return ret;
}

// Load WAV
unsigned int j1Audio::LoadFx(const char* path)
{
	unsigned int ret = 0;

	if(!active)
		return 0;

	Mix_Chunk* chunk = Mix_LoadWAV_RW(App->fs->Load(path), 1);

	if(chunk == nullptr)
	{
		LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
	}
	else
	{
		fx.push_back(chunk);
		ret = fx.size();
	}

	return ret;
}

// Play WAV
bool j1Audio::PlayFx(unsigned int id, int repeat)
{
	bool ret = false;

	if (!active)
		return false;

	if (id > 0 && id <= fx.size())
	{
		int n_mus = 0;
		std::list<Mix_Chunk*>::iterator item = fx.begin();
		for (; item != fx.cend() && n_mus<id - 1; ++item, ++n_mus) {

		}

		Mix_PlayChannel(-1, (*item), repeat);
	}

	return ret;
}

bool j1Audio::StopMusic(float fade_time)
{
	bool ret = true;
	if (music != nullptr)
	{
		if (fade_time > 0.0f)
		{
			Mix_FadeOutMusic(int(fade_time * 1000.0f));
		}
		else
		{
			Mix_HaltMusic();
		}
		// this call blocks until fade out is done
		Mix_FreeMusic(music);
	}
	return ret;
}

bool j1Audio::ResumeMusic()
{
	bool ret = true;
	if (music != nullptr)
	{
		Mix_ResumeMusic();
		if (Mix_Playing(-1) == 0)
		{
			LOG("Music is now playing.");
			ret = false;
		}
		else
		{
			LOG("Music dosent play. Mix_GetError(): %s", Mix_GetError());
		}
	}
	return ret;
}

void j1Audio::VolumeMusic(int volume)
{
	if (music != nullptr)
	{
		LOG("volume was    : %d\n", Mix_VolumeMusic(MIX_MAX_VOLUME / 2));
		Mix_VolumeMusic(volume);
		LOG("volume is now : %d\n", Mix_VolumeMusic(-1));
		this->volume = volume;
	}
}
