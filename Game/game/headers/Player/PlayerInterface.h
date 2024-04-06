#pragma once

class Player;
class UIDialogBox;

class PlayerInterface
{
public:
	PlayerInterface(float gameWidth, float gameHeigth);

	//Main Functions
	void init(int gameWidth, int gameHeigth);
	void OnUpdate(Uint32 t, Player& player, UIDialogBox& dialogBox);
	void OnDraw(CGraphics* g);	
private:
	
	CFont font;
	Player* localPlayer;
	float localH, localW;
	CSprite weaponComponent, armorComponent, bossComponent;

	float skillBarY;
	float localTime;


	CHealthBar hbar, energyBar, armorBar;
	CSprite skillBar, sprintSkillLogoOn, sprintSkillLogoOff, callbackSkillIcon, mainEnergyUI;
};

 