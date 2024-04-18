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
	//fonts
	CFont font;

	//local Var
	Player* localPlayer;
	float localH, localW;
	float localTime;

	//Health
	CSprite hpHeart, hearthBg;

	//Charged Shot UI
	CSprite chargedShotBarBG;
	CHealthBar  chargedShotBar;

	//Hit Effect
	float hitEffectTimer;
	CSprite hitEffectBg;

	//Roll Skill Logo
	CSprite  sprintSkillLogoOn, sprintSkillLogoOff;

	//Buffs Logo
	CSprite InvulnerableBuffLogo, distanceBuffLogo;

};