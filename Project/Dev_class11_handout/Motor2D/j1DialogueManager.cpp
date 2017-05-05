#include "j1DialogueManager.h"
#include "j1App.h"
#include "j1FileSystem.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Input.h"
#include "j1HUD.h"
#include "j1Fonts.h"

#include "GuiImage.h"
#include "GuiLabel.h"
#include "GuiButton.h"

j1DialogueManager::j1DialogueManager() : j1Module()
{
	name = ("dialogue");
}

j1DialogueManager::~j1DialogueManager()
{
	
}

bool j1DialogueManager::Awake(pugi::xml_node& config)
{
	bool ret = false;
	LOG("Loading DialogManager");

	WindowRect.w = App->win->GetWindowW();
	WindowRect.h = App->win->GetWindowH();

	pugi::xml_document	config_file;
	pugi::xml_node		dialogue_config;

	char* buf = nullptr;
	int size = App->fs->Load("dialogues/Dialogues.xml", &buf);
	pugi::xml_parse_result result = config_file.load_buffer(buf, size);
	RELEASE(buf);

	if (result == NULL)
		LOG("Could not load map xml file Dialogues.xml. pugi error: %s", result.description());
	else
		dialogue_config = config_file.child("DialogueManager");

	//Set config
	if (dialogue_config.empty() == false)
		ret = true;

	if (ret == true)
	{
		//Set drawing areas and dialogue hud gui
		SDL_Rect TextBackgroundRect = { 0,94,1233,231 };
		TextBackground = App->gui->CreateImage({ 23,WindowRect.h - TextBackgroundRect.h - 11 }, &TextBackgroundRect, false, AddGuiTo::none);

		SDL_Rect LeftCharacterLabelRect = { 0,0,336,94 };
		LeftCharacterLabel = App->gui->CreateButton(iPoint(5, WindowRect.h - TextBackgroundRect.h - 11 - (int)(LeftCharacterLabelRect.h * 0.5f)), &std::string("Left Guy"), ButtonType::idle_only, &LeftCharacterLabelRect, false, AddGuiTo::none);
		LeftCharacterLabel->SetFont(App->font->ReturnofGanon36);

		SDL_Rect RightCharacterLabelRect = { 336,0,336,94 };
		RightCharacterLabel = App->gui->CreateButton(iPoint(WindowRect.w - RightCharacterLabelRect.w - 5, WindowRect.h - TextBackgroundRect.h - 11 - (int)(LeftCharacterLabelRect.h * 0.5f)), &std::string("Right Guy"), ButtonType::idle_only, &RightCharacterLabelRect, false, AddGuiTo::none);
		RightCharacterLabel->SetFont(App->font->ReturnofGanon36);

		DialogueInterlucutorStrRelationVec.push_back(new DialogueInterlucutorStrRelation(&std::string(""), DialogueInterlucutor::item_nullinterlucutor));
		DialogueInterlucutorStrRelationVec.push_back(new DialogueInterlucutorStrRelation(&std::string("king"), DialogueInterlucutor::King));
		DialogueInterlucutorStrRelationVec.push_back(new DialogueInterlucutorStrRelation(&std::string("link"), DialogueInterlucutor::Link));
		DialogueInterlucutorStrRelationVec.push_back(new DialogueInterlucutorStrRelation(&std::string("zelda"), DialogueInterlucutor::Zelda));
		DialogueInterlucutorStrRelationVec.push_back(new DialogueInterlucutorStrRelation(&std::string("messenger"), DialogueInterlucutor::Messenger));
		DialogueInterlucutorStrRelationVec.push_back(new DialogueInterlucutorStrRelation(&std::string("ric"), DialogueInterlucutor::Ric));
		DialogueInterlucutorStrRelationVec.push_back(new DialogueInterlucutorStrRelation(&std::string("guard"), DialogueInterlucutor::Guard));

		///Get characters atlas
		link_tex = nullptr;
		zelda_tex = nullptr;
		king_tex = nullptr;
		guard_tex = nullptr;
		ric_tex = nullptr;

		//Allocate dialogues from XML
		AllocateDialogues(dialogue_config, &TextBackground->GetLocalPos());
	}

	ActiveDialog = new ActiveDialogue();

	return ret;
}

void j1DialogueManager::AllocateDialogues(pugi::xml_node& dialoguenode, iPoint* TextBackgroundPos)
{
	//Allocate dialogues from XML
	//itereate cutscenes
	for (pugi::xml_node newcutscene = dialoguenode.child("Cutscenes").child("cutscene"); newcutscene; newcutscene = newcutscene.next_sibling("cutscene"))
		CreateDialogue(newcutscene, TextBackgroundPos, DialogueType::Cutscene_dialoguetype);
	//itereate NPCs
	for (pugi::xml_node newnpc = dialoguenode.child("NPCs").child("npc"); newnpc; newnpc = newnpc.next_sibling("npc"))
		CreateDialogue(newnpc, TextBackgroundPos, DialogueType::NPC_dialoguetype);
	//itereate items
	for (pugi::xml_node newirtem = dialoguenode.child("Items").child("item"); newirtem; newirtem = newirtem.next_sibling("item"))
		CreateDialogue(newirtem, TextBackgroundPos, DialogueType::item_dialoguetype);
}

void j1DialogueManager::CreateDialogue(pugi::xml_node& dialoguenode, iPoint* TextBackgroundPos, DialogueType type)
{
	dialogues.push_back(new Dialogue());
	Dialogue* newdialogue = dialogues.back();
	newdialogue->id = (DialogueID)dialoguenode.attribute("enum_value").as_int();
	newdialogue->type = type;
	for (pugi::xml_node newstep = dialoguenode.child("step"); newstep; newstep = newstep.next_sibling("step"))
	{
		newdialogue->DialogueSteps.push_back(new DialogueStep());
		DialogueStep* newdialoguestep = newdialogue->DialogueSteps.back();
		newdialoguestep->SpeakerDialogueCharacter = new DialogueCharacter();
		newdialoguestep->SpeakerDialogueCharacter->DialogueCharacter_id = CheckInterlocutor(&std::string(newstep.attribute("speaker").as_string()));
		if(newdialoguestep->SpeakerDialogueCharacter->DialogueCharacter_id == DialogueInterlucutor::item_nullinterlucutor)
			newdialoguestep->SpeakerDialogueCharacter->DialogueCharacter_str = std::string("");
		else
			newdialoguestep->SpeakerDialogueCharacter->DialogueCharacter_str = std::string(newstep.attribute("speaker").as_string());
		newdialoguestep->SpeakerDialogueCharacter->DialogueCharacter_pos = CheckInterlocutorPosition(&std::string(newstep.attribute("speaker_pos").as_string()));

		///Set character atlas and expression
		newdialoguestep->SpeakerDialogueCharacter->Character_Atlas = nullptr;
		newdialoguestep->SpeakerDialogueCharacter->Character_Expression_Rect = { 0,0,0,0 };

		newdialoguestep->ListenerDialogueCharacter = new DialogueCharacter();
		newdialoguestep->ListenerDialogueCharacter->DialogueCharacter_id = CheckInterlocutor(&std::string(newstep.attribute("listener").as_string()));
		if (newdialoguestep->ListenerDialogueCharacter->DialogueCharacter_id == DialogueInterlucutor::item_nullinterlucutor)
			newdialoguestep->ListenerDialogueCharacter->DialogueCharacter_str = std::string("");
		else
			newdialoguestep->ListenerDialogueCharacter->DialogueCharacter_str = std::string(newstep.attribute("listener").as_string());
		newdialoguestep->ListenerDialogueCharacter->DialogueCharacter_pos = CheckInterlocutorPosition(&std::string(newstep.attribute("listener_pos").as_string()));
		
		///Set character atlas and expression
		newdialoguestep->ListenerDialogueCharacter->Character_Atlas = nullptr;
		newdialoguestep->ListenerDialogueCharacter->Character_Expression_Rect = { 0,0,0,0 };

		int y = 0;
		for (pugi::xml_node newsteplines = newstep.child("line"); newsteplines; newsteplines = newsteplines.next_sibling("line"), y += 30)
		{
			newdialoguestep->lines.push_back(App->gui->CreateLabel({ TextBackgroundPos->x + 60,TextBackgroundPos->y + 60 + y }, &std::string(newsteplines.child_value()), false, AddGuiTo::none));
			newdialoguestep->lines.back()->SetFont(App->font->ReturnofGanon36);
		}
	}
}

DialogueInterlucutor j1DialogueManager::CheckInterlocutor(std::string* interlocutor_str)
{
	for (std::vector<DialogueInterlucutorStrRelation*>::iterator item = DialogueInterlucutorStrRelationVec.begin(); item != DialogueInterlucutorStrRelationVec.cend(); ++item)
		if ((*item)->DialogueInterlucutorStr == *interlocutor_str)
			return (*item)->DialogueInterlucutorEnum;
	return DialogueInterlucutor::item_nullinterlucutor;
}

DialogueInterlucutorPosition j1DialogueManager::CheckInterlocutorPosition(std::string* interlocutor_position_str)
{
	if(*interlocutor_position_str == right_str)
		return DialogueInterlucutorPosition::Right;
	return DialogueInterlucutorPosition::Left;
}

bool j1DialogueManager::Start()
{
	return true;
}

bool j1DialogueManager::PreUpdate()
{
	return true;
}

bool j1DialogueManager::Update(float dt)
{
	//Test code
	//Different dialogues
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		App->dialoguemanager->ActivateDialogue(DialogueID::castle_intro);
	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
		App->dialoguemanager->ActivateDialogue(DialogueID::castle_sewers_entrance);
	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
		App->dialoguemanager->ActivateDialogue(DialogueID::castle_sewers_exit);
	//Pause/resume dialogue
	if (App->input->GetKey(SDL_SCANCODE_9) == KEY_DOWN)
		PauseActiveDialogue();
	if (App->input->GetKey(SDL_SCANCODE_0) == KEY_DOWN)
		ResumeActiveDialogue();
	/*
	if (App->input->GetKey(SDL_SCANCODE_0) == KEY_DOWN)
	{
		ActiveDialog->DialogueActive = false;
		ActiveDialog->DialoguePaused = false;
	}
	*/
	//Test code end

	if (!ActiveDialog->DialogueActive || ActiveDialog->DialoguePaused)
		return true;

	//Blit Dark background
	App->render->DrawQuad(WindowRect, Black(0), Black(1), Black(2), 80, true, false, false);

	//Blit text background and Name labels
	TextBackground->DrawWithAlternativeAtlas(App->hud->GetAtlas());
	if(*LeftCharacterLabel->GetLabelStr() != empty_char)
		LeftCharacterLabel->DrawWithAlternativeAtlas(App->hud->GetAtlas());
	if (*RightCharacterLabel->GetLabelStr() != empty_char)
		RightCharacterLabel->DrawWithAlternativeAtlas(App->hud->GetAtlas());

	//Blit text
	for (std::vector<GuiLabel*>::iterator item = ActiveDialog->ActiveDialogueStepPtr->lines.begin(); item != ActiveDialog->ActiveDialogueStepPtr->lines.cend(); ++item)
		(*item)->Draw();

	if ((App->input->GetKey(SDL_SCANCODE_RETURN) == j1KeyState::KEY_DOWN) || (App->input->GetControllerButton(0, 0) == j1KeyState::KEY_DOWN))
		DialogueNextStep();

	return true;
}

bool j1DialogueManager::PostUpdate()
{
	return true;
}

bool j1DialogueManager::CleanUp()
{
	for (std::vector<Dialogue*>::iterator item = dialogues.begin(); item != dialogues.cend(); ++item)
		RELEASE(*item);
	dialogues.clear();
	for (std::vector<DialogueInterlucutorStrRelation*>::iterator item = DialogueInterlucutorStrRelationVec.begin(); item != DialogueInterlucutorStrRelationVec.cend(); ++item)
		RELEASE(*item);
	DialogueInterlucutorStrRelationVec.clear();
	RELEASE(ActiveDialog);
	RELEASE(TextBackground);
	RELEASE(LeftCharacterLabel);
	RELEASE(RightCharacterLabel);
	return true;
}

void j1DialogueManager::ActivateDialogue(DialogueID id)
{
	ActiveDialog->DialogueActive = true;
	ActiveDialog->DialoguePaused = false;
	for (std::vector<Dialogue*>::iterator item = dialogues.begin(); item != dialogues.cend(); ++item)
		if ((*item)->id == id)
		{
			ActiveDialog->ActiveDialoguePtr = *item;
			ActiveDialog->ActiveDialogueStepPtr = (*item)->DialogueSteps.front();
			SetCharacterBlit();
		}
}

void j1DialogueManager::PauseActiveDialogue()
{
	if(ActiveDialog->DialogueActive == true)
		ActiveDialog->DialoguePaused = true;
}

void j1DialogueManager::ResumeActiveDialogue()
{
	if (ActiveDialog->DialogueActive == true)
		ActiveDialog->DialoguePaused = false;
}

void j1DialogueManager::DialogueNextStep()
{
	for (std::vector<DialogueStep*>::iterator item = ActiveDialog->ActiveDialoguePtr->DialogueSteps.begin(); item != ActiveDialog->ActiveDialoguePtr->DialogueSteps.cend(); ++item)
		if ((*item) == ActiveDialog->ActiveDialogueStepPtr)
		{
			if ((*item) != ActiveDialog->ActiveDialoguePtr->DialogueSteps.back())
			{
				ActiveDialog->ActiveDialogueStepPtr = *(++item);
				SetCharacterBlit();
			}
			else //if it the last step, quit and clean
			{
				ActiveDialog->DialogueActive = false;
				ActiveDialog->DialoguePaused = false;
				ActiveDialog->ActiveDialoguePtr = nullptr;
				ActiveDialog->ActiveDialogueStepPtr = nullptr;
			}
			return;
		}
}

void j1DialogueManager::SetCharacterBlit()
{
	if (ActiveDialog->ActiveDialogueStepPtr->ListenerDialogueCharacter->DialogueCharacter_pos == DialogueInterlucutorPosition::Left)
		LeftCharacterLabel->EditButtonStr(&ActiveDialog->ActiveDialogueStepPtr->ListenerDialogueCharacter->DialogueCharacter_str);
	else
		RightCharacterLabel->EditButtonStr(&ActiveDialog->ActiveDialogueStepPtr->ListenerDialogueCharacter->DialogueCharacter_str);
	if (ActiveDialog->ActiveDialogueStepPtr->SpeakerDialogueCharacter->DialogueCharacter_pos == DialogueInterlucutorPosition::Left)
		LeftCharacterLabel->EditButtonStr(&ActiveDialog->ActiveDialogueStepPtr->SpeakerDialogueCharacter->DialogueCharacter_str);
	else
		RightCharacterLabel->EditButtonStr(&ActiveDialog->ActiveDialogueStepPtr->SpeakerDialogueCharacter->DialogueCharacter_str);
}

void j1DialogueManager::OnGui(Gui* ui, GuiEvent event)
{

}

void j1DialogueManager::OnConsoleCommand(const Command* command, const std::vector<std::string>& commandarguments)
{

}

void j1DialogueManager::OnConsoleCVar(const CVar* cvar)
{

}

DialogueStep::DialogueStep()
{

}

DialogueStep::~DialogueStep()
{
	delete SpeakerDialogueCharacter;
	delete ListenerDialogueCharacter;
	for (std::vector<GuiLabel*>::iterator item = lines.begin(); item != lines.cend(); ++item)
		RELEASE(*item);
	lines.clear();
}

Dialogue::Dialogue()
{

}

Dialogue::~Dialogue()
{
	for (std::vector<DialogueStep*>::iterator item = DialogueSteps.begin(); item != DialogueSteps.cend(); ++item)
		RELEASE(*item);
	DialogueSteps.clear();
}

ActiveDialogue::ActiveDialogue()
{

}

ActiveDialogue::~ActiveDialogue()
{

}

DialogueInterlucutorStrRelation::DialogueInterlucutorStrRelation(std::string* str, DialogueInterlucutor enu) : DialogueInterlucutorStr(*str), DialogueInterlucutorEnum(enu)
{

}

DialogueInterlucutorStrRelation::~DialogueInterlucutorStrRelation()
{

}

DialogueCharacter::DialogueCharacter()
{

}

DialogueCharacter::~DialogueCharacter()
{

}