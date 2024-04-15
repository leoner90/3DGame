#pragma once
class Enemy;
class Player;
class Map;

class Radar
{
public:
	Radar(float w, float h);

	//Main Functions
	void init();
	void OnUpdate(Uint32 t, std::vector<Enemy*>& AllEnemies, Player& player, Map& map);
	void OnDraw(CGraphics* g);

private:
	void DrawDot(float posX, float posZ, CColor color, CGraphics* g);

	CSprite radarBg, radarCover;
	CSprite dot;
	CSpriteList dotList;

	std::vector<Enemy*> localAllEnemies;
	Player* localPlayer;
	float radarSize;
	CLine ray;
	int cooldown;
};

 