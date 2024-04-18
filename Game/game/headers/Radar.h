#pragma once
//Forward Declaration
class AIPlayer;
class Player;
class Map;

class Radar
{
public:
	Radar(float w, float h);

	//Main Functions
	void init();
	void OnUpdate(Uint32 t, std::vector<AIPlayer*>& AllAIPlayerList, Player& player, Map& map);
	void OnDraw(CGraphics* g);

private:
	void DrawDot(float posX, float posZ, CColor color, CGraphics* g);

	//Radar ItSelf
	CSprite radarBg, radarCover;
	float radarSize;

	//Player & AIPlayers Dots
	CSprite dot;
	CSpriteList dotList;

	//Local Variables
	std::vector<AIPlayer*> localAllAIPlayerList;
	Player* localPlayer;

	//ray Casting Show enemies only in Front Of Player
	CLine ray;
	int cooldown;
};