#pragma once

class Player;

class PlayerInterface
{
public:
	PlayerInterface(float gameWidth, float gameHeigth);

	//Main Functions
	void init(int gameWidth, int gameHeigth);
	void OnUpdate(Uint32 t, Player& player);
	void OnDraw(CGraphics* g);	
private:
	
	CFont font;
	Player* localPlayer;
	float localH, localW;
	CSprite hpHeart, hearthBg, chargedShotBarBG;
	float localTime;
	float hitEffectTimer;

	CHealthBar  chargedShotBar;
	CSprite  sprintSkillLogoOn, sprintSkillLogoOff;
	CSprite InvulnerableBuffLogo, distanceBuffLogo;
	CSprite hitEffectBg;
};

 