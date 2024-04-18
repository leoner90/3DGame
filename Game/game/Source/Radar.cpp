#include "Game.h"
#include "../headers/Radar.h"
#include "../headers/AIPlayer.h"
#include "../headers/Player/Player.h"
#include "../headers/Map.h"

Radar::Radar(float w, float h)
{
	//Radar ItSelf
	radarSize = 180;
	radarBg.LoadImage("radarBg2.png");
	radarBg.SetSize(radarSize + 30, radarSize);
	radarBg.SetPosition(w - ((radarSize + 30) / 2), radarSize/ 2 + 10);

	//Player & AIPlayers Dots
	dot.LoadImage("dot.png");
	dot.SetSize(10, 10);
}

void Radar::init()
{
	  cooldown = 0; //For RayCasting
}

void Radar::OnUpdate(Uint32 t, std::vector<AIPlayer*>& AllAIPlayerList, Player& player, Map &map)
{
	//local Var
	localAllAIPlayerList = AllAIPlayerList;
	localPlayer = &player;

	//Ray Casting Show enemies only in Front Of Player
	if (cooldown == 0)
	{
		for (auto AiPlayer : AllAIPlayerList)
		{
			//calculates can we see an enemy
			AiPlayer->IsInLineOfSight = true;
			
			//displacement
			CVector displ = AiPlayer->AIPlayerModel->GetPositionV() - player.playerModel.GetPositionV();
			displ.Normalize();

			//is in line of sigh of player
			if (displ.Dot(player.playerModel.GetDirectionV()) < 0)
			{
				AiPlayer->IsInLineOfSight = false;
				continue;
			}
			ray.SetPositionV(AiPlayer->AIPlayerModel->GetPositionV(), player.playerModel.GetPositionV());

			for (auto obj : map.collidingObjects)
			{
				if (ray.HitTest(obj))
				{
					AiPlayer->IsInLineOfSight = false;
				}
			}
		}
	}
	//if cooldown = true , new rayCast
	if (cooldown++ > 30) cooldown = 0;
}

void Radar::OnDraw(CGraphics* g)
{
	radarBg.Draw(g);

	//if IsInLineOfSight true -> Draw Enemy / TeamMate DOT
	for (auto enemy : localAllAIPlayerList)
	{
		if (enemy->OnSpawnHold || !enemy->IsInLineOfSight) continue;

		CColor dotColor = CColor::Red();
		if (enemy->isFriend) dotColor = CColor::Green();

		DrawDot(enemy->AIPlayerModel->GetX(), enemy->AIPlayerModel->GetZ(), dotColor, g);
	}

	//player dot
	DrawDot(localPlayer->playerModel.GetX(), localPlayer->playerModel.GetZ(), CColor::Yellow(), g);
}

void Radar::DrawDot(float posX, float posZ, CColor color, CGraphics* g)
{
	//finding pos of Enemy by comparing to the half GAME Map size (7500 x 7500)
	float haldMapSize = 3750;
	float radarCenterPos = radarSize / 2 - 15;
	float PosX = posX / haldMapSize * (radarSize - 40);
	float resultX = radarCenterPos + (PosX / 2);

	float enemyPosZ = posZ / haldMapSize * (radarSize - 40);
	float resultZ = radarCenterPos - (enemyPosZ / 2);

	dot.SetPosition(1740 + resultX, resultZ); // 1740 - x axis offset
	dot.SetColor(color);
	dot.Draw(g);
}