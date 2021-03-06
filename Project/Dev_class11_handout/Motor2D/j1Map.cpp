#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1FileSystem.h"
#include "j1Textures.h"
#include "j1Map.h"
#include <math.h>
#include "j1Gui.h"
#include "j1Player.h"
#include "j1Window.h"

#include "Brofiler/Brofiler.h"

j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name = "map";
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder = config.child("folder").child_value();

	return ret;
}

void j1Map::Draw(int height)
{
	BROFILER_CATEGORY("Map Draw", Profiler::Color::LightYellow);
	float scale = App->win->GetScale();

	if (map_loaded == false)
		return;

	//TEMP FOR VERTICAL SLICE

	int i = 0;
	Character* Link = App->player->Link;
	Character* Zelda = App->player->Zelda;
	bool blit_link = true;
	bool blit_zelda = true;


	std::list<MapLayer*>::iterator item = data.layers.begin();
	for (; item != data.layers.cend(); ++item) {
		MapLayer* layer = (*item);

		if (!App->collision->debug) {
			if (layer->properties.Get("Navigation") != 0 || layer->properties.Get("Enemies") != 0 || layer->properties.Get("Path") != 0)
				continue;
		}
		if (layer->print_height == height) {
			/*
			//Quadtree culling
			std::vector< iPoint*> tiles_in_view;
			SDL_Rect viewport = { -App->render->camera.x / scale,-App->render->camera.y / scale,App->render->camera.w / scale,App->render->camera.h / scale };
			BROFILER_CATEGORY("Map Draw CollectCandidates", Profiler::Color::LightYellow);
			layer->layer_quadtree->CollectCandidates(tiles_in_view, viewport);
			BROFILER_CATEGORY("Map Draw CollectCandidates for", Profiler::Color::LightYellow);
			for (std::vector<iPoint*>::iterator item = tiles_in_view.begin(); item != tiles_in_view.cend(); ++item)
			{
				iPoint tile_pos = { (*item)->x / data.tile_width, (*item)->y / data.tile_height };
				int tile_id = layer->Get(tile_pos.x, tile_pos.y);
				if (tile_id > 0)
				{
					TileSet* tileset = GetTilesetFromTileId(tile_id);
					SDL_Rect r = tileset->GetTileRect(tile_id);
					iPoint pos = MapToWorld(tile_pos.x, tile_pos.y);
					App->render->Blit(tileset->texture, pos.x, pos.y, &r);
				}
			}
			*/
			/**/
			//Camera culling
			BROFILER_CATEGORY("Map Draw camera culling", Profiler::Color::LightYellow);
			for (int y = 0; y < data.height; ++y)
			{
				if (y * data.tile_height + I_CAMERAMARGINTILE >= -App->render->camera.y / scale && y *data.tile_height < -App->render->camera.y / scale + App->render->camera.h / scale) {
					for (int x = 0; x < data.width; ++x)
					{
						if (x*data.tile_width + I_CAMERAMARGINTILE >= -App->render->camera.x / scale && x*data.tile_width < -App->render->camera.x / scale + App->render->camera.w / scale) {
							int tile_id = layer->Get(x, y);
							if (tile_id > 0)
							{
								TileSet* tileset = GetTilesetFromTileId(tile_id);

								SDL_Rect r = tileset->GetTileRect(tile_id);
								iPoint pos = MapToWorld(x, y);

								//if (layer->properties.Get("Navigation") == false && layer->properties.Get("Enemies") == false && layer->properties.Get("Path") == false)
								App->render->Blit(tileset->texture, pos.x, pos.y, &r);
							}
						}
					}
				}
			}
			/**/
		}

		/*
		auto next_item = item;
		next_item++;
		if (layer->print_height == Link->logic_height && blit_link && (*next_item)->print_height !=Link->logic_height)
		{
		blit_link = false;
		App->render->Blit(Link->entity_texture, Link->pos.x - Link->actual_animation.GetCurrentFrame().pivot.x, Link->pos.y - Link->actual_animation.GetCurrentFrame().pivot.y, &Link->actual_animation.GetCurrentFrame().rect);
		}
		if (layer->print_height == Zelda->logic_height  && blit_zelda && (*next_item++)->print_height != Zelda->logic_height) {
		blit_zelda = false;
		App->render->Blit(Zelda->entity_texture, Zelda->pos.x - Zelda->actual_animation.GetCurrentFrame().pivot.x, Zelda->pos.y - Zelda->actual_animation.GetCurrentFrame().pivot.y, &Zelda->actual_animation.GetCurrentFrame().rect);
		}
		*/
		//App->player->Draw();

	}
}

int Properties::Get(const char* value, int default_value) const
{

	for (std::list<Property*>::const_iterator item = list.begin(); item != list.cend(); ++item) {
		if ((*item)->name == value)
			return (*item)->value;
	}

	return default_value;
}

TileSet* j1Map::GetTilesetFromTileId(int id) const
{
	std::list<TileSet*>::const_iterator item = data.tilesets.begin();
	TileSet* set = (*item);
	for (; item != data.tilesets.cend(); ++item) {
		if (id < (*item)->firstgid)
		{
			set = (*--item);
			break;
		}
		set = (*item);
	}
	return set;
}

iPoint j1Map::MapToWorld(int x, int y) const
{
	iPoint ret(0,0);

	if(data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
	}
	else if(data.type == MAPTYPE_ISOMETRIC)
	{
		ret.x =(int)( (x - y) * (data.tile_width * 0.5f));
		ret.y = (int)((x + y) * (data.tile_height * 0.5f));
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

iPoint j1Map::WorldToMap(int x, int y) const
{
	iPoint ret(0,0);

	if(data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}
	else if(data.type == MAPTYPE_ISOMETRIC)
	{
		
		float half_width = data.tile_width * 0.5f;
		float half_height = data.tile_height * 0.5f;
		ret.x = int( (x / half_width + y / half_height) / 2) - 1;
		ret.y = int( (y / half_height - (x / half_width)) / 2);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}
/*
iPoint j1Map::MapToWorldCenter(int x, int y) const
{
	iPoint ret = MapToWorld(x, y);

	ret.x += data.tile_width * 0.5f;
	ret.y += data.tile_height * 0.5f - 1;

	return ret;
}
*/
SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect = {0,0,0,0};
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));
	return rect;
}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	


	for (std::list <TileSet*>::iterator temp = App->map->data.tilesets.begin(); temp != App->map->data.tilesets.cend(); ++temp) {
		if (*temp != nullptr) {
			App->tex->UnLoad((*temp)->texture);
		}
	}

	for (std::list<TileSet*>::iterator item = data.tilesets.begin(); item != data.tilesets.cend(); ++item) {
		RELEASE((*item));
	}

	data.tilesets.clear();

	std::list<ObjectLayer*>::iterator item = data.objects.begin();
	for (; item != data.objects.cend(); ++item) {
		RELEASE((*item));
	}
	data.objects.clear();

	// Remove all layers

	std::list<MapLayer*>::iterator item2 = data.layers.begin();
	for (; item2 != data.layers.cend(); ++item2) {
		RELEASE((*item2));
	}

	data.layers.clear();


	V_Colision.clear();
	V_Logic.clear();
	V_Enemies.clear();
	V_PathEnemies.clear();
	// Clean up the pugui tree
	map_file.reset();

	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	static char tmp_string[4096];
	sprintf_s(tmp_string, 4096, "%s%s",folder.c_str(),file_name);
	bool ret = true;
	
	char* buf;
	int size = App->fs->Load(tmp_string, &buf);
	pugi::xml_parse_result result = map_file.load_buffer(buf, size);

	RELEASE(buf);

	if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if(ret == true)
	{
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for(tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if(ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if(ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.push_back(set);
	}

	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for(layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* lay = new MapLayer();

		ret = LoadLayer(layer, lay);
		
		if(ret == true)
			data.layers.push_back(lay);
	}

	pugi::xml_node object_layer;

	//Iterates all the object layers
	int height = 0;
	for (object_layer = map_file.child("map").child("objectgroup"); object_layer && ret; object_layer = object_layer.next_sibling("objectgroup"))
	{
		ObjectLayer* obj = new ObjectLayer();
		ret = LoadObjectLayer(object_layer, obj,height );
		if (ret == true)
			data.objects.push_back(obj);

		height++;
	}
	count = 0;
	for (object_layer = map_file.child("map").child("objectgroup"); object_layer && ret; object_layer = object_layer.next_sibling("objectgroup"))
	{
		LoadConnectedObjects(object_layer);
	}

	if(ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		for (std::list<TileSet*>::iterator item = data.tilesets.begin(); item != data.tilesets.cend(); ++item) {
			TileSet* s = (*item);
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.c_str(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
		}

		std::list<MapLayer*>::iterator item_layer = data.layers.begin();
		for (; item_layer != data.layers.cend(); ++item_layer) {
			MapLayer* l = (*item_layer);
			LOG("Layer ----");
			LOG("name: %s", l->name.c_str());
			LOG("tile width: %d tile height: %d", l->width, l->height);
		}

	}
	map_loaded = ret;
	
	return ret;
}

// Load map general properties
bool j1Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if(map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		std::string bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 255;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 255;

		if(bg_color.length() > 0)
		{
			std::string red, green, blue;
			bg_color.replace(1, 2, red);
			bg_color.replace(3, 4, green);
			bg_color.replace(5, 6, blue);

			int v = 0;

			sscanf_s(red.c_str(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.c_str(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.c_str(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.b = v;
		}

		std::string orientation(map.attribute("orientation").as_string());

		if(orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if(orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if(orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}
	}

	return ret;
}

bool j1Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name = tileset_node.attribute("name").as_string();

	set->firstgid = tileset_node.attribute("firstgid").as_int();
	if (strcmp(set->name.c_str(),"collision tileset") ==0) {
		TILE_COL_ID = set->firstgid;
		CANT_PASS_COL_ID = set->firstgid + 3;
		App->enemy->green_enemy_id = set->firstgid+2;
		App->enemy->enemy_path_id = set->firstgid + 1;
		App->enemy->championsoldier_enemy_id = set->firstgid + 4;
		App->enemy->hyrulebombsoldier_enemy_id = set->firstgid + 6;
		App->enemy->statue_enemy_id = set->firstgid + 8;
		App->enemy->rope_enemy_id= set->firstgid + 5;
		App->enemy->skullrope_enemy_id = set->firstgid + 10;
		App->enemy->rat_enemy_id = set->firstgid + 7;
		App->enemy->bat_enemy_id = set->firstgid + 9;

	}
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if(offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if(image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
	
	set->texture = App->tex->Load(PATH(folder.c_str(), image.attribute("source").as_string()));

	int w, h;
	SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
	set->tex_width = image.attribute("width").as_int();

	if (set->tex_width <= 0)
	{
		set->tex_width = w;
	}

	set->tex_height = image.attribute("height").as_int();

	if (set->tex_height <= 0)
	{
		set->tex_height = h;
	}

	set->num_tiles_width = set->tex_width / set->tile_width;
	set->num_tiles_height = set->tex_height / set->tile_height;
	}

	return ret;
}

bool j1Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;
	float scale = App->win->GetScale();

	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_int();
	layer->height = node.attribute("height").as_int();
	layer->print_height = node.first_child().first_child().attribute("value").as_int();
	LoadProperties(node, layer->properties);
	pugi::xml_node layer_data = node.child("data");

	if (layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{
		layer->data = new uint[layer->width*layer->height];
		memset(layer->data, 0, layer->width*layer->height);

		pugi::xml_node tile = layer_data.first_child();
		const char* chain = tile.value();
		char* temp = strtok((char*)chain, " ,.-");
		int i = 0;
		/*
		int count = 0;
		SDL_Rect boundaries = { 0,0,layer->width * data.tile_width,layer->height * data.tile_height };
		layer->layer_quadtree = new Quadtree(boundaries);
		*/
		while (temp != nullptr)
		{
			int num = atoi(temp);

			//Here implement quadtree
			/*
			iPoint tilepos = layer->Get(i);
			tilepos.x *= data.tile_width;
			tilepos.y *= data.tile_height;
			bool inserted = layer->layer_quadtree->Insert(&tilepos);
			if (inserted) count++;
			*/
			layer->data[i++] = num;
			temp = strtok(NULL, ",");
		}
		//LOG("Accepted points: %i/%i", count, layer->width * layer->height);
	}

	return ret;
}

bool j1Map::LoadObjectLayer(pugi::xml_node & node, ObjectLayer * layer, int height)
{

	//Loads all the objects in a layer created in Tiled
	bool ret = true;

	layer->name = node.attribute("name").as_string();
	
	LoadProperties(node, layer->properties);
	pugi::xml_node layer_data = node.child("object");

	if (layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{		
	//Iterates all the objects
		int count = App->object->V_Objects.size();
		for (pugi::xml_node node_object = layer_data; node_object; node_object = node_object.next_sibling("object"))
		{
			pugi::xml_node iterator = node_object.child("properties").child("property");
			while (iterator.attribute("name").as_string() != "type" && iterator){
				iterator = iterator.next_sibling();
			}
			pugi::xml_node attribute = node_object.child("properties").child("property");
			while (strcmp(attribute.attribute("name").value(), "type") )
				attribute = attribute.next_sibling();
			char* type_name = (char*)attribute.attribute("value").as_string();
			Object* temp = App->object->CreateObject(type_name, node_object, height);

			//Change the logic height of the new object
			
		}
		
		

	}

	return ret;
}

bool j1Map::LoadConnectedObjects(pugi::xml_node & node)
{
	bool ret = true;

	
	pugi::xml_node layer_data = node.child("object");

	if (layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		
	}
	else
	{
		
		for (pugi::xml_node node_object = layer_data; node_object; node_object = node_object.next_sibling("object"))
		{

			pugi::xml_node attribute = node_object.child("properties").child("property");
			while (strcmp(attribute.attribute("name").value(), "entity") && attribute) {
				attribute = attribute.next_sibling();
			}
			std::vector<Object*> vec_connected_objects = App->object->FindObject(attribute.attribute("value").as_string());
			for (int i = 0; i < vec_connected_objects.size(); i++) {
				App->object->V_Objects[count]->connected_object.push_back(vec_connected_objects[i]);
			}
			
			count++;
		}
	}
	return false;
}

// Load a group of properties from a node and fill a list with it
bool j1Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
	bool ret = false;

	pugi::xml_node data = node.child("properties");

	if(data != NULL)
	{
		pugi::xml_node prop;

		for(prop = data.child("property"); prop; prop = prop.next_sibling("property"))
		{
			Properties::Property* p = new Properties::Property();

			p->name = prop.attribute("name").as_string();
			p->value = prop.attribute("value").as_int();

			properties.list.push_back(p);
		}
	}

	return ret;
}


//need to make a common function that recives int& width, int& height, uchar** buffer and string and vector


bool j1Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer) const
{
	bool ret = false;

	std::list<MapLayer*>::const_iterator item = data.layers.begin();
	for (int i =0;i<data.layers.size(); item++,i++) {
		MapLayer* layer = (*item);

		//App->map->Colision = layer;
		if (layer->properties.Get("Navigation", 0) != 0) {

			App->map->V_Colision.push_back(layer);

			uchar* map = new uchar[layer->width*layer->height];
			memset(map, 1, layer->width*layer->height);

			for (int y = 0; y < data.height; ++y)
			{
				for (int x = 0; x < data.width; ++x)
				{
					int i = (y*layer->width) + x;

					int tile_id = layer->Get(x, y);
					TileSet* tileset = (tile_id > 0) ? GetTilesetFromTileId(tile_id) : NULL;

					if (tileset != NULL)
					{
						map[i] = (tile_id - tileset->firstgid) > 0 ? 0 : 1;
						/*TileType* ts = tileset->GetTileType(tile_id);
						if(ts != NULL)
						{
						map[i] = ts->properties.Get("walkable", 1);
						}*/
					}
				}
			}

			*buffer = map;
			width = data.width;
			height = data.height;
			ret = true;
		}
	}
	return ret;
}

bool j1Map::CreateEnemyMap(int& width, int& height, uchar** buffer) const
{
	bool ret = false;

	std::list<MapLayer*>::const_iterator item = data.layers.begin();
	for (; item != data.layers.cend(); ++item) {
		MapLayer* layer = (*item);

		//App->map->Colision = layer;
		if (layer->properties.Get("Enemies", 0) != 0) {

			App->map->V_Enemies.push_back(layer);

			uchar* map = new uchar[layer->width*layer->height];
			memset(map, 1, layer->width*layer->height);

			for (int y = 0; y < data.height; ++y)
			{
				for (int x = 0; x < data.width; ++x)
				{
					int i = (y*layer->width) + x;

					int tile_id = layer->Get(x, y);
					TileSet* tileset = (tile_id > 0) ? GetTilesetFromTileId(tile_id) : NULL;

					if (tileset != NULL)
					{
						map[i] = (tile_id - tileset->firstgid) > 0 ? 0 : 1;
						/*TileType* ts = tileset->GetTileType(tile_id);
						if(ts != NULL)
						{
						map[i] = ts->properties.Get("walkable", 1);
						}*/
					}
				}
			}

			*buffer = map;
			width = data.width;
			height = data.height;
			ret = true;


		}
	}
	return ret;
}

bool j1Map::CreateEnemyPathMap(int & width, int & height, uchar ** buffer) const
{
	bool ret = false;


	std::list<MapLayer*>::const_iterator item = data.layers.begin();
	for (; item != data.layers.cend(); ++item) {
		MapLayer* layer = (*item);

		//App->map->Colision = layer;
		if (layer->properties.Get("Path", 0) != 0) {

			App->map->V_PathEnemies.push_back(layer);

			uchar* map = new uchar[layer->width*layer->height];
			memset(map, 1, layer->width*layer->height);

			for (int y = 0; y < data.height; ++y)
			{
				for (int x = 0; x < data.width; ++x)
				{
					int i = (y*layer->width) + x;

					int tile_id = layer->Get(x, y);
					TileSet* tileset = (tile_id > 0) ? GetTilesetFromTileId(tile_id) : NULL;

					if (tileset != NULL)
					{
						map[i] = (tile_id - tileset->firstgid) > 0 ? 0 : 1;
						/*TileType* ts = tileset->GetTileType(tile_id);
						if(ts != NULL)
						{
						map[i] = ts->properties.Get("walkable", 1);
						}*/
					}
				}
			}

			*buffer = map;
			width = data.width;
			height = data.height;
			ret = true;


		}
	}
	return ret;
}
