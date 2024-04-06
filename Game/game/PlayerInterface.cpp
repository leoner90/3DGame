#include "Game.h"
#include "headers/Player/Player.h"
#include "headers/Player/PlayerInterface.h"
#include "headers/UIDialogBox.h"


PlayerInterface::PlayerInterface(float gameWidth, float gameHeigth)
{
	localH = gameHeigth;
	localW = gameWidth;
	font.LoadDefault();



	//energyBar

	energyBar.SetSize(205, 15);
	energyBar.SetPosition(218, 93); 
	energyBar.SetColors(CColor::Yellow(), CColor::Black(), CColor::Black());

	// setup healthbar
	hbar.SetSize(95, 25);
	hbar.SetPosition(165, 53);
	hbar.SetColors(CColor::Red(), CColor::Black(), CColor::Black());

	//Armor Bar
	armorBar.SetSize(95, 25);
	armorBar.SetPosition(272, 53);
	armorBar.SetColors(CColor::Gray(), CColor::Black(), CColor::Black());


	mainEnergyUI.LoadImage("mainEnergyUI.png");
	mainEnergyUI.SetSize(300, 150);
	mainEnergyUI.SetPosition(180, 100);






	//Resources Icons
	armorComponent.LoadImage("techoCore.png");
	armorComponent.SetSize(25, 30);
	armorComponent.SetPosition(localW - 90, localH - 30);

	weaponComponent.LoadImage("bio.png");
	weaponComponent.SetSize(30, 30);
	weaponComponent.SetPosition(localW - 90, localH - 75);

	bossComponent.LoadImage("bioCore.png");
	bossComponent.SetSize(30, 30);
	bossComponent.SetPosition(localW - 90, localH - 120);

	skillBarY = 240;

	//skillBar
	sprintSkillLogoOn.LoadImage("sprintSkillLogo.png");
	sprintSkillLogoOn.SetSize(80, 80);
	sprintSkillLogoOn.SetPosition(localW - 65, skillBarY - 5);

	sprintSkillLogoOff.LoadImage("sprintSkillLogoOnReload.png");
	sprintSkillLogoOff.SetSize(80, 80);
	sprintSkillLogoOff.SetPosition(localW - 65, skillBarY - 5);

	callbackSkillIcon.LoadImage("callbackSkillIcon.png");
	callbackSkillIcon.SetSize(80, 80);
	callbackSkillIcon.SetPosition(localW - 65, skillBarY - 5);


}

void PlayerInterface::init(int gameWidth, int gameHeigth)
{
	hbar.SetHealth(100);
	armorBar.SetHealth(100);
	energyBar.SetHealth(100);
}

void PlayerInterface::OnUpdate(Uint32 t, Player& player, UIDialogBox& dialogBox)
{
	localPlayer = &player;
	localTime = t;
	float remainingHpInPercentage = localPlayer->playerCurrentHp / (localPlayer->playerMaxHp / 100);
 

	hbar.SetHealth(remainingHpInPercentage);
 
}

void PlayerInterface::OnDraw(CGraphics* g)
{
	hbar.Draw(g);
	energyBar.Draw(g);
	armorBar.Draw(g);
	mainEnergyUI.Draw(g);

 

 
	if(localPlayer->dashCoolDown < localTime)
		sprintSkillLogoOn.Draw(g);
	else
		sprintSkillLogoOff.Draw(g);
 
	 

	//resources
	if (localPlayer->armorComponents > 0)
	{
		armorComponent.Draw(g);
		font.DrawNumber(localW - 50, localH - 45, localPlayer->armorComponents, CColor::White(), 30);
	}

	if (localPlayer->weaponComponents > 0)
	{
		weaponComponent.Draw(g);
		font.DrawNumber(localW - 50, localH - 90, localPlayer->weaponComponents, CColor::White(), 30);
	}

	if (localPlayer->bossLoot > 0)
	{
		bossComponent.Draw(g);
		font.DrawNumber(localW - 50, localH - 135, localPlayer->bossLoot, CColor::White(), 30);
	}
	

	
}

 