#include "Game.h"
#include "headers/Enemy.h"
#include "headers/Player/Player.h"
#include "headers/Map.h"
#include <math.h> 

//*************** INIT ***************

Enemy::~Enemy()
{
	delete enemyModel;
}

void Enemy::init(int index, int enemyType, Map& map, CModelMd3& enemyMod, int curentGameLevel)
{
	//Shooting TO DO
	bullet.LoadModel("bullet/Bullet2.obj");
	bullet.SetScale(8.f);
	onStartOfTheLevel = true;

	localEnemyIndex = index;
	localGameLvl = curentGameLevel;
	AttackHideTimer = 0;
	changePosTimer = 0;
	enemyModel = enemyMod.Clone();
	localEnemyType = enemyType;
	IsInLineOfSight = false;
	curentPosNum = 0;

	//hp
	hearthHP.LoadImage("lifeHeart.png");
	hearthHP.SetSize(15, 15);

	//points 
 
	entityAllPos.clear();
	if (localEnemyType == 0 || localEnemyType == 2)
	{
		if (localGameLvl == 1)
		{
			if (localEnemyIndex == 0) // enemy 1 pos
			{
				entityAllPos.push_back(CVector(-1676, 100, 406));
				entityAllPos.push_back(CVector(-2040, 100, 1037));
				entityAllPos.push_back(CVector(-669, 100, 1662));
				entityAllPos.push_back(CVector(-2040, 100, 1037));
			}
			if (localEnemyIndex == 1)// enemy 2 pos
			{
				entityAllPos.push_back(CVector(-1536, 100, -477));
				entityAllPos.push_back(CVector(-1722, 100, -1009));
				entityAllPos.push_back(CVector(-1059, 100, -1265));
				entityAllPos.push_back(CVector(-1722, 100, -1009));
			}
			if (localEnemyIndex == 2) // enemy 3 pos
			{
				entityAllPos.push_back(CVector(-1051, 100, -1267));
				entityAllPos.push_back(CVector(-1055, 100, -805));
				entityAllPos.push_back(CVector(-206, 100, 16));
				entityAllPos.push_back(CVector(-1055, 100, -805));
			}
		}
		if (localGameLvl == 2)
		{
			if (localEnemyIndex == 0) // enemy 1 pos
			{
				entityAllPos.push_back(CVector(-2008, 100, 887));
				entityAllPos.push_back(CVector(-1867, 100, -289));
				entityAllPos.push_back(CVector(-258, 100, 1151));
				entityAllPos.push_back(CVector(-1867, 100, -289));
			}
			if (localEnemyIndex == 1)// enemy 2 pos
			{
				entityAllPos.push_back(CVector(-1902, 100, 345));
				entityAllPos.push_back(CVector(-1100, 100, -553));
				entityAllPos.push_back(CVector(-934, 100, -2057));
				entityAllPos.push_back(CVector(-1902, 100, -345));
			}
			if (localEnemyIndex == 2) // enemy 3 pos
			{
				entityAllPos.push_back(CVector(-1156, 100, -521));
				entityAllPos.push_back(CVector(-320, 100, 1182));
				entityAllPos.push_back(CVector(378, 100, 2453));
				entityAllPos.push_back(CVector(-1156, 100, -521));
			}
		}

		if (localGameLvl == 3)
		{
			if (localEnemyIndex == 0) // enemy 1 pos
			{
				entityAllPos.push_back(CVector(-1883, 100, 198));
				entityAllPos.push_back(CVector(-654, 100, 1080));
				entityAllPos.push_back(CVector(-177, 100, 351));
				entityAllPos.push_back(CVector(-654, 100, 1080));
			}
			if (localEnemyIndex == 1)// boss 2 pos
			{
				entityAllPos.push_back(CVector(-204, 100, 95));
				entityAllPos.push_back(CVector(-1777, 100, -1720));
				entityAllPos.push_back(CVector(-1673, 100, -11));
				entityAllPos.push_back(CVector(-1777, 100, -1720));
			}
		}

	
 
		
		enemyMaxHp = enemyCurrentHp = 3;
		enemyDamage = 1;
		enemySpeed = 550  ;
		//	bully running 1 - 22, throwing 24 - 109, death 111 - 220 boss
		if (localEnemyType == 2)
		{
		
		}
		else 
		{
		
			
		}

		//enemyModel->AddAnimation("run", 1, 36);
		//enemyModel->AddAnimation("attack", 36, 142);
		//enemyModel->AddAnimation("dead", 144, 211);
		
		enemyModel->AddAnimation("run", 1, 22);
		enemyModel->AddAnimation("attack", 24, 109);
		enemyModel->AddAnimation("dead", 111, 220);


		isFriend = false;
		enemyModel->SetPositionV(entityAllPos[0]);
	}

	else if (localEnemyType == 1 )
	{
		if (localGameLvl == 1)
		{
			if (localEnemyIndex == 0) // friend  
			{
				entityAllPos.push_back(CVector(2090, 100, 540));
				entityAllPos.push_back(CVector(2234, 100, 1017));
				entityAllPos.push_back(CVector(1194, 100, 1037));
				entityAllPos.push_back(CVector(2234, 100, 1017));
			}
		}
		if (localGameLvl == 2)
		{
			entityAllPos.push_back(CVector(1385, 100, -59));
			entityAllPos.push_back(CVector(1731, 100, 224));
			entityAllPos.push_back(CVector(304, 100, 1210));
			entityAllPos.push_back(CVector(1731, 100, 224));
		}

		enemyDamage = 1;
		enemyMaxHp = enemyCurrentHp = 3;
		enemySpeed = 550;
		enemyModel->AddAnimation("run", 1, 20);
		enemyModel->AddAnimation("attack", 36, 142);
		enemyModel->AddAnimation("dead", 144, 211);
		enemyModel->SetPositionV(entityAllPos[0]);
		isFriend = true;
	}
	

	enemyModel->PlayAnimation("attack",10, true);
	enemyModel->SetDirectionAndRotationToPoint(0, 0);
	isDead = preDeahAnimation = false;
	
	enemyHpbar.SetSize(35, 3);
	enemyHpbar.SetColor(CColor::Blue());
	enemyHpbar.SetHealth(100);

	deathAnimationTimer = 0;

	//wait at spawn point
	OnSpawnHold = true;

	enemyModel->SetToFloor(0);

	onHitEffect.delete_all();

	//yes this will eat memory
	onHitEffectModel.LoadModel("snowFlake/snowFlake.obj");
	onHitEffectModel.SetScale(0.6f);

}

//*************** UPDATE ***************
void Enemy::OnUpdate(Uint32 t, Player &player, Map& map, std::vector<Enemy*>& AllEnemies, CVector enemypos)
{

	if (isDead) return;

	if (preDeahAnimation)
	{
		
		if (deathAnimationTimer == 0)
		{
			enemyModel->PlayAnimation("dead", 16, false);
			enemyModel->SetSpeed(0);
			
			deathAnimationTimer = 1000 + t;

			if (localEnemyType == 2) deathAnimationTimer = t; // no death animation
		}
		else if(deathAnimationTimer < t) isDead = true;
		
		onHitEffect.Update(t);
		onHitEffect.delete_if(true);
		enemyModel->Update(t);
		return;
	}

	//local Variables
	localPlayer = &player;
	localMap = &map;
	localTime = t;
	AllAIPlayerList = AllEnemies;

	//for cutscenes
	if (isBoxShowen) UIDialogBox::OnUpdate(localTime, enemypos);
	enemyModel->Update(t);

	if (!OnSpawnHold)
	{	
		//start of the level Dialogues
		initDialogues();
		
		EnemyControl();
		//if pre-dead -> play animation of death , set dead status true by the end
		float remainingHpInPercentage = enemyCurrentHp / (enemyMaxHp / 100);
		enemyHpbar.SetHealth(remainingHpInPercentage);
		enemyModel->Update(t);

		onHitEffect.Update(t);
		onHitEffect.delete_if(true);

		ShotsHandler();
		//if dead -> return (do nothing)
	}
}

//*************** 2D RENDER ***************
void Enemy::OnDraw(CGraphics* g, CVector enemyPos)
{
	if (isBoxShowen) UIDialogBox::OnDraw(g);
	if (isDead|| OnSpawnHold) return;

	CVector displ = localPlayer->playerModel.GetPositionV() - enemyModel->GetPositionV();
	float distance = hypot(displ.GetX(), displ.GetZ());
	
	if (distance < 3000 && !preDeahAnimation && !isBoxShowen)
	{
		for (int i = 0; i < enemyCurrentHp; i++)
		{
			hearthHP.SetPosition(enemyPos.GetX() + i * 30, enemyPos.GetY() + enemyModel->GetTop());
			hearthHP.Draw(g);
		}
		
	}
}

//*************** 3D RENDER ***************
void Enemy::OnRender3D(CGraphics* g)
{
	if (isDead) return;
	enemyModel->Draw(g);
	onHitEffect.Draw(g);
	snowBallList.Draw(g);
}

void Enemy::EnemyGetDamage(float damage)
{
	if (preDeahAnimation) return;
	for (int i = 0; i < 15; i++)
	{
		CModel* p = onHitEffectModel.Clone();
		p->SetPositionV(enemyModel->GetPositionV() + CVector(0, 100, 0));
		p->SetDirection(rand() % 360);
		p->SetSpeed(rand() % 200 + 150);
		p->SetColor(CColor::White());
		p->Die(450);
		onHitEffect.push_back(p);
	}

	enemyCurrentHp -= damage;

	if (enemyCurrentHp <= 0)
	{
		deathSound.Play("monsterDeath.wav");
		preDeahAnimation = true;
 
	}
}

void Enemy::EnemyControl()
{
	if (changePosTimer == 0)
	{
		changePosTimer = localTime;
		AttackHideTimer = localTime;
	}
 
	if (changePosTimer <= localTime && !enemyModel->IsAutoMoving())
	{
		curentPosNum++;
		if (curentPosNum > 3) curentPosNum = 0;

		currentDestinationPoint = entityAllPos[curentPosNum];
 
		enemyModel->PlayAnimation("run", 22, true);
		enemyModel->SetDirectionAndRotationToPoint(currentDestinationPoint.GetX(), currentDestinationPoint.GetZ());
	 	enemyModel->MoveTo(currentDestinationPoint.GetX(), currentDestinationPoint.GetZ(), 200);
		if ((curentPosNum != 1 && curentPosNum != 3) || localGameLvl == 3) changePosTimer = localTime + 12000; //skipps middle pos in level 1 and 2, but not 3

	}
	else if (!enemyModel->IsAutoMoving())
	{
		Attack();
	}
}

void Enemy::Attack()
{
	if (attackDelay < localTime)
	{
		enemyModel->PlayAnimation("attack", 22, true);
		attackDelay = localTime + 3000;

		//finding Closest Distance
		float closestDistance = 0;
		float currentDist = 0;
		CVector targetPos = {0,0,0};

		//compare With All AI players
		for (auto AIPlayer : AllAIPlayerList)
		{
			//continue If target is a friend 
			if (isFriend == AIPlayer->isFriend) continue;

			currentDist = enemyModel->GetPositionV().Distance(AIPlayer->enemyModel->GetPositionV());

			if (currentDist < closestDistance || closestDistance == 0)
			{
				closestDistance = currentDist;
				targetPos = AIPlayer->enemyModel->GetPositionV();
			}
		}

		//Compare Distance to the Player If target is not friend 
		if (isFriend != localPlayer->isFriend)
		{
			currentDist = enemyModel->GetPositionV().Distance(localPlayer->playerModel.GetPositionV());
			if (currentDist < closestDistance || closestDistance == 0)
			{
				closestDistance = currentDist;
				targetPos = localPlayer->playerModel.GetPositionV();

				if (localPlayer->playerModel.GetSpeed() > 0)
				{
					// player Movement Prediction  
					CVector playerMovemntNormal = CVector(localPlayer->playerModel.GetXVelocity(), localPlayer->playerModel.GetYVelocity(), localPlayer->playerModel.GetZVelocity()).Normalized();
					CVector playerMovementPrediction = playerMovemntNormal  * localPlayer->playerModel.GetSpeed(); // need delta time.............
					targetPos += playerMovementPrediction;
				}
			
			}
		}


		CModel* pShot = bullet.Clone();
		pShot->SetPositionV(enemyModel->GetPositionV() + CVector(0, 150, 0));
		pShot->SetDirectionAndRotationToPoint(targetPos.GetX(), targetPos.GetZ());
		pShot->SetSpeed(1000);
		pShot->SetYVelocity(pShot->GetYVelocity() + closestDistance * 0.36);
		pShot->Die(3500);
		pShot->SetStatus(0);
		snowBallList.push_back(pShot);
	}
}

void Enemy::ShotsHandler()
{
	for (CModel* pShot : snowBallList)
	{
		for (auto mapObj : localMap->collidingObjects)
		{
			if (pShot->HitTest(mapObj))
			{
				pShot->Delete();
				continue;
			}
		}
		float gravity = 23;
		pShot->SetYVelocity(pShot->GetYVelocity() - gravity);

		if (pShot->HitTest(&localPlayer->playerModel) && !isFriend) //add distance
		{	
			localPlayer->playerGettingDamage(enemyDamage);
			pShot->Delete();
		}

		for (auto AIPlayer : AllAIPlayerList)
		{
			if (pShot->HitTest(AIPlayer->enemyModel) && isFriend != AIPlayer->isFriend)//add distance
			{
				AIPlayer->EnemyGetDamage(enemyDamage);
				pShot->Delete();
			}
		}
	}

	snowBallList.Update(localTime);
	snowBallList.delete_if(true);
}

void Enemy::initDialogues()
{
	//level 1 entry Dialog Box
	if (onStartOfTheLevel && localGameLvl == 1)
	{
		onStartOfTheLevel = false;
		if (!isFriend) UIDialogBox::showBox(localEnemyIndex, 0, 0, 9000);
		else UIDialogBox::showBox(5, 1, 1, 9000); // friend id 5
	}

	//level 1 entry Dialog Box
	if (onStartOfTheLevel && localGameLvl == 2)
	{
		onStartOfTheLevel = false;
		if (!isFriend) UIDialogBox::showBox(localEnemyIndex, 0, 0, 9000);
		else UIDialogBox::showBox(5, 1, 1, 9000); // friend id 5
	}

	//level 1 entry Dialog Box
	if (onStartOfTheLevel && localGameLvl == 3)
	{
		onStartOfTheLevel = false;
		if (!isFriend) UIDialogBox::showBox(localEnemyIndex, 0, 0, 9000);
	}
}
