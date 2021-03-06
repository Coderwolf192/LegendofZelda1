#ifndef _ENDSCENE_
#define _ENDSCENE_

#include "MainScene.h"

class S_End : public MainScene {

public:
	S_End();
	~S_End();
	bool Awake(pugi::xml_node& conf);
	bool Start();
	bool Update();
	bool Clean();
	bool Save(pugi::xml_node&) const;
	bool Load(pugi::xml_node&);
public:
	char* test;
	SDL_Texture* finale_texture = nullptr;
	j1Timer loop_timer;
};

#endif // 

