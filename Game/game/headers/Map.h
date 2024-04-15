#pragma once
class Player;

class Map
{
public:
	Map();
	//Main Functions
	void init(int level);
	void OnUpdate(Uint32 t, Player& player);
	void OnDraw(CGraphics* g);
	void OnRender3D(CGraphics* g);

	CModelList collidingObjects, mapCollision;
private:
	void weather();
	void LoadData();

	Uint32 localTime;
	CFloor floor;
 
	CModel barrels, tree, flowers, grass, hidingWall, house, iglo, mushroom, mountain, rock2, snowman, wall, rainDrop;
	CModelList modelList, rain;
	CFont font;
	Player* localPlayer;
	int gameLevel;
};