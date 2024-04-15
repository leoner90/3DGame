#include "Game.h"
#include "headers/Radar.h"
#include "headers/Enemy.h"
#include "headers/Player/Player.h"
#include "headers/Map.h"

Radar::Radar(float w, float h)
{
	radarSize = 180;
	radarBg.LoadImage("radarBg2.png");
	radarBg.SetSize(radarSize + 30, radarSize);
	radarBg.SetPosition(w - ((radarSize + 30) / 2), radarSize/ 2 + 10);

	// char screen
	dot.LoadImage("dot.png");
	dot.SetSize(10, 10);
	
}

void Radar::init()
{
	  cooldown = 0;
}

void Radar::OnUpdate(Uint32 t, std::vector<Enemy*>& AllEnemies, Player& player, Map &map)
{
	localAllEnemies =  AllEnemies;
 

	if (cooldown == 0)
	{
		for (auto AiPlayer : AllEnemies)
		{
			//calculates can we see an emnemy
			AiPlayer->IsInLineOfSight = true;

			CVector displ = AiPlayer->enemyModel->GetPositionV() - player.playerModel.GetPositionV();
			displ.Normalize();

			//is in line of sigh of player
			if (displ.Dot(player.playerModel.GetDirectionV()) < 0)
			{
				AiPlayer->IsInLineOfSight = false;
				continue;
			}


			
			ray.SetPositionV(AiPlayer->enemyModel->GetPositionV(), player.playerModel.GetPositionV());

			for (auto obj : map.collidingObjects)
			{
				if (ray.HitTest(obj))
				{
					AiPlayer->IsInLineOfSight = false;
				}
			}

		}
	}

	if (cooldown++ > 30) cooldown = 0;

	


	localPlayer = &player;
}





void Radar::OnDraw(CGraphics* g)
{
	radarBg.Draw(g);
	for (auto enemy : localAllEnemies)
	{
		if (enemy->OnSpawnHold || !enemy->IsInLineOfSight) continue;


		CColor dotColor = CColor::Red();
		if (enemy->isFriend) dotColor = CColor::Green();

		DrawDot(enemy->enemyModel->GetX(), enemy->enemyModel->GetZ(), dotColor, g);
	}

	//player
	DrawDot(localPlayer->playerModel.GetX(), localPlayer->playerModel.GetZ(), CColor::Yellow(), g);
}


void Radar::DrawDot(float posX, float posZ, CColor color, CGraphics* g)
{
	float radarCenterPos = radarSize / 2 - 15;
	float PosX = posX / 3750 * (radarSize - 40);
	float resultX = radarCenterPos + (PosX / 2);

	float enemyPosZ = posZ / 3750 * (radarSize - 40);
	float resultZ = radarCenterPos - (enemyPosZ / 2);

	dot.SetPosition(1740 + resultX, resultZ);
	dot.SetColor(color);
	dot.Draw(g);

}