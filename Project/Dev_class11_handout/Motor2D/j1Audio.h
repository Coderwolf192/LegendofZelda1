#ifndef __j1AUDIO_H__
#define __j1AUDIO_H__

#include "j1Module.h"
#include <list>
#define DEFAULT_MUSIC_FADE_TIME 2.0f

struct _Mix_Music;
struct Mix_Chunk;

class j1Audio : public j1Module
{
public:

	j1Audio();

	// Destructor
	virtual ~j1Audio();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	bool Start();

	// Called before quitting
	bool CleanUp();

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	void RestartAudio();

	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0);
	uint button_sound = 0;
	uint secret_sound = 0;
	uint boss_defeated_sound = 0;
	uint boss_hit_sound = 0;
	uint boss_invencible_sound = 0;
	uint boss_laser_sound = 0;
	uint boss_dies_sound = 0;


	bool StopMusic(float fade_time);
	bool ResumeMusic();
	void VolumeMusic(int volume);

	bool PlayingMusic()
	{
		if (music == nullptr || volume == 0)
			return false;
		else
			return true;
	}
	
private:

	_Mix_Music*			music = nullptr;
	std::list<Mix_Chunk*> fx;
	uint volume = 0;
	
};

#endif // __j1AUDIO_H__