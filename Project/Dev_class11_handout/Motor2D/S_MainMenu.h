#ifndef _S_MAIN_MENU_
#define _S_MAIN_MENU_
#include"j1App.h"
#include"j1Scene.h"
#include "MainScene.h"
#include"Gui.h"
class S_MainMenu : public MainScene {

public:
	S_MainMenu();
	~S_MainMenu();
	bool Start();
	bool Update();
	bool Clean();
	void OnGui(Gui* ui, GuiEvent event);
public:
	GuiButton* idle_hoover_pressed_button = nullptr;

};

#endif // #pragma once