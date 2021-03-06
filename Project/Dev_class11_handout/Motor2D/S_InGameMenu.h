#ifndef _S_INGAME_MENU_
#define _S_INGAME_MENU_

#include "j1App.h"
#include "j1Scene.h"
#include "MainScene.h"
#include "Gui.h"
#include "j1Timer.h"

class S_InGameMenu : public MainScene
{
public:
	S_InGameMenu();
	~S_InGameMenu();
	bool Awake(pugi::xml_node& conf);
	bool Start();
	bool Update();
	bool Clean();
	bool Save(pugi::xml_node&) const;
	bool Load(pugi::xml_node&);
	void OnGui(Gui* ui, GuiEvent event);

	bool Active() const
	{
		return active;
	}
	//This must be revised, must be private
	bool active = false;

private:
	GuiButton* IngamemenuLabel = nullptr;
	GuiButton* resume =			nullptr;
	GuiButton* loadcheckpoint =	nullptr;
	GuiButton* options =		nullptr;
	GuiButton* mainmenu =		nullptr;
	GuiButton* quit =	   		nullptr;
	
};

#endif // #pragma once
