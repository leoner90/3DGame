#pragma once

//Forward Declaration
class Player;

class Map
{
public:
	Map();
	//Main Functions
	void init(int level);
	void OnUpdate(Uint32 t, Player& player);
	void OnRender3D(CGraphics* g);

	//Map Objects (TO DO GETTERS SETTERS)
	CModelList collidingObjects, mapCollision;
private:
	//Models
	CModel barrels, tree, flowers, grass, hidingWall, house, iglo, mushroom, mountain, rock2, snowman, wall, snowFlake;
	CModelList modelList, snowFlakeList;

	//Westher
	void weather();

	//Map Loading
	void LoadData();

	//local Variables
	Uint32 localTime;
	Player* localPlayer;
	int localGameLevel;

	//Game Floor
	CFloor floor;

	//fonts
	CFont font;
};