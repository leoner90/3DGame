#include "Game.h"
#include "../headers/Player/Player.h"
#include "../headers/Player/PlayerInterface.h"


PlayerInterface::PlayerInterface(float gameWidth, float gameHeigth)
{
	font.LoadDefault();

	//local Var
	localH = gameHeigth;
	localW = gameWidth;

	//hp
	hpHeart.LoadImage("lifeHeart.png");
	hpHeart.SetSize(45, 34);

	hearthBg.LoadImage("hearthBg.png");
	hearthBg.SetSize(230, 90);
	hearthBg.SetPosition(140, localH - 70);

	//Buffs Icons
	InvulnerableBuffLogo.LoadImage("imortalityLogo.png");
	InvulnerableBuffLogo.SetSize(30, 30);
	InvulnerableBuffLogo.SetPosition(localW - 90, localH - 30);

	distanceBuffLogo.LoadImage("longShotBuffLogo.png");
	distanceBuffLogo.SetSize(30, 30);
	distanceBuffLogo.SetPosition(localW - 90, localH - 75);

	//chargedShotBarBG
	chargedShotBarBG.LoadImage("chargedShotBg.png");
	chargedShotBarBG.SetSize(200, 100);
	chargedShotBarBG.SetPosition(localW /2 ,  250);

	chargedShotBar.SetSize(180, 40);
	chargedShotBar.SetPosition(localW / 2, 260);
	chargedShotBar.SetColors(CColor::Blue(), CColor::Red(), CColor::Blue());
	chargedShotBar.SetHealth(0);

	//skillBar
	sprintSkillLogoOn.LoadImage("sprintSkillLogo.png");
	sprintSkillLogoOn.SetSize(68, 80);
	sprintSkillLogoOn.SetPosition(localW - 65, 235 );

	sprintSkillLogoOff.LoadImage("sprintSkillLogoOnReload.png");
	sprintSkillLogoOff.SetSize(68, 80);
	sprintSkillLogoOff.SetPosition(localW - 65, 235 );

	//player In Damage Effect
	hitEffectBg.LoadImage("hitEffect.png");
	hitEffectBg.SetSize(localW, localH);
	hitEffectBg.SetPosition(localW / 2, localH / 2);
}

//*** INIT
void PlayerInterface::init(int gameWidth, int gameHeigth)
{
	hitEffectTimer = 0;
}

//*** UPDATE
void PlayerInterface::OnUpdate(Uint32 t, Player& player)
{
	//local Var
	localPlayer = &player;
	localTime = t;

	//Charged Shot Handler
	chargedShotBar.SetHealth(localPlayer->chargePercent);
	chargedShotBar.Update(localTime);

	//Hit Effect 
	if (localPlayer->isPlayerInDamage && hitEffectTimer == 0) hitEffectTimer = t + 1500;
	if (hitEffectTimer < t) hitEffectTimer = 0;
}

void PlayerInterface::OnDraw(CGraphics* g)
{
	//Health Bar
	hearthBg.Draw(g);
	for (int i = 0; i < localPlayer->playerCurrentHp; i++)
	{
		hpHeart.SetPosition(85 + i * 57, localH - 70);
		hpHeart.Draw(g);
	}
	
	//SKILLS Logo Draw
	if(localPlayer->dashCoolDown < localTime)
		sprintSkillLogoOn.Draw(g);
	else
		sprintSkillLogoOff.Draw(g);
 
	//Charged Shot DRAW
	if (localPlayer->chargePercent > 0)
	{
		chargedShotBar.Draw(g);
		chargedShotBarBG.Draw(g);
	}

	//Buffs
	if (localPlayer->isPlayerInvulnerable )
	{
		InvulnerableBuffLogo.Draw(g);
		font.DrawNumber(localW - 65, localH - 45, round(localPlayer->InvulnerableBuffTimer - localTime) / 100.0 , CColor::White(), 30);
	}

	if (localPlayer->isPlayerUnderDistanceBuff)
	{
		distanceBuffLogo.Draw(g);
		font.DrawNumber(localW - 65, localH - 90, round(localPlayer->distanceBuffTimer - localTime) / 100.0  , CColor::White(), 30);
	}

	//Hit EffectS
	if(hitEffectTimer != 0 ) hitEffectBg.Draw(g);
}