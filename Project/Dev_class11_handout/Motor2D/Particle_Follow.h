#pragma once
#ifndef _P_FOLLOW_H_
#define _P_FOLLOW_H_

#include "ParticleManager.h"

class Particle;

class P_Follow
{
public:
	P_Follow(Arrow* arrow_to_follow, iPoint* object_follow, SDL_Rect initial_rect, iPoint area, iPoint timelife, int num_textures, int num_particles, bool active_, bool isMouse);
	~P_Follow();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	void render(fPoint pos);

	void Update_position(iPoint* pos);


public:
	//TODO 2: ---------------------------------------------------------------------
	//Create a Particle Vector, use std::vector
	//Also, create a pointer to an entity (SceneElement*)
	//And finally a pointer to an iPoint (iPoint*)
	std::vector<Particle*> particle;
	Arrow* arrow_to_follow = nullptr;
	iPoint* object_follow = nullptr;
	//-----------------------------------------------------------------------------
	//Num of particles
	int number_particles = 0;

	//iPoints
	iPoint area = { 0,0 };
	iPoint timelife = { 0,0 };

	//position
	fPoint pos = { 0.0f,0.0f };
	int size_rect = 0;
	int n_textures = 0;

	//velocity
	//fPoint speed;
	bool active = false;
	bool isMouse = false;

	bool godelete = false;

};

#endif /*_P_FOLLOW_H_*/