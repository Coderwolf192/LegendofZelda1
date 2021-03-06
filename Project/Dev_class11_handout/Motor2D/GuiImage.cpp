#include "j1Gui.h"
#include "GuiImage.h"
#include "j1App.h"
#include "j1Render.h"

GuiImage::GuiImage(iPoint position, SDL_Rect* rect, bool movable, AddGuiTo addto) : Gui(position, GuiType::gui_image, movable, addto), 
texture_rect(*rect)
{
	Gui_Collider = { position.x, position.y, rect->w, rect->h };
}

GuiImage::~GuiImage()
{

}

void GuiImage::Draw()
{
	SubDrawFunction(App->gui->GetAtlas());
}

void GuiImage::DrawWithAlternativeAtlas(SDL_Texture* alternativeatlas, bool usecam, bool usescale)
{
	SubDrawFunction(alternativeatlas, usecam, usescale);
}

void GuiImage::SubDrawFunction(SDL_Texture* atlas, bool usecam, bool usescale)
{
	if (this->visible == true)
	{
		if (usecam)
		{
			App->render->Blit(atlas, position.x - App->render->camera.x, position.y - App->render->camera.y, &texture_rect, 1.0f, 0, INT_MAX, INT_MAX, usescale, opacity);
			if (App->gui->Gui_DebugDraw_isactive())
				this->DebugDraw();
		}
		else
			App->render->Blit(atlas, position.x, position.y, &texture_rect, 1.0f, 0, INT_MAX, INT_MAX, usescale, opacity);
	}
}

void GuiImage::DebugDraw() const
{
	App->render->DrawQuad(Gui_Collider, Lime(0), Lime(1), Lime(2), DEBUG_DRAW_ALPHA, true, false, false);
}

void GuiImage::SetOpacity(uint newopacity)
{
	if (newopacity < 0)
		opacity = 0;
	else if (newopacity > 255)
		opacity = 255;
	else
		opacity = newopacity;
}