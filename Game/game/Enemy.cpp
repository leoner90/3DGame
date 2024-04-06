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

void Enemy::init(int index, int enemyType, Map& map, CModelMd3& enemyMod)
{
	//Shooting TO DO
	bullet.LoadModel("bullet/Bullet2.obj");
	bullet.SetScale(8.f);


	localEnemyIndex = index;
	AttackHideTimer = 0;
	changePosTimer = 0;
	enemyModel = enemyMod.Clone();
	localEnemyType = enemyType;
	IsInLineOfSight = false;
	curentPosNum = 0;

	//points 
 
	if (localEnemyType == 0) 
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
		
		enemyMaxHp = enemyCurrentHp = 200;
		enemyDamage = 50;
		enemySpeed = 350 + rand() % 250;
		enemyModel->AddAnimation("run", 1, 29);
		enemyModel->AddAnimation("attack", 35, 59);
		enemyModel->AddAnimation("dead", 65, 82);
		enemyModel->SetPositionV(entityAllPos[0]);
		isFriend = false;
	}

	else if (localEnemyType == 1)
	{
		if (localEnemyIndex == 0) // fiend 1
		{
			entityAllPos.push_back(CVector(2063, 100, -1074));
			entityAllPos.push_back(CVector(1881, 100, -1716));
			entityAllPos.push_back(CVector(1270, 100, -1773));
			entityAllPos.push_back(CVector(1881, 100, -1716));
		}
		if (localEnemyIndex == 1) // fiend 2
		{
			entityAllPos.push_back(CVector(2090, 100, 540));
			entityAllPos.push_back(CVector(2234, 100, 1017));
			entityAllPos.push_back(CVector(1194, 100, 1037));
			entityAllPos.push_back(CVector(2234, 100, 1017));
		}

		enemyDamage = 50;
		enemyMaxHp = enemyCurrentHp = 400;
		enemySpeed = 350 + rand() % 250;
		enemyModel->AddAnimation("attack", 0, 10);
		enemyModel->AddAnimation("run", 10, 40);
		enemyModel->AddAnimation("dead", 42, 70);
		enemyModel->SetPositionV(entityAllPos[0]);
		isFriend = true;
	}
	

	
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

	showBox(0, 3,3,16000);


}

//*************** UPDATE ***************
void Enemy::OnUpdate(Uint32 t, Player &player, Map& map, std::vector<Enemy*>& AllEnemies, CVector enemypos)
{
	//if dead -> return (do nothing)
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

	if (!OnSpawnHold)
	{	
		//if dialog box showen -> update position
		if (isBoxShowen) UIDialogBox::OnUpdate(localTime, enemypos);
		EnemyControl();


		//if pre-dead -> play animation of death , set dead status true by the end
		float remainingHpInPercentage = enemyCurrentHp / (enemyMaxHp / 100);
		enemyHpbar.SetHealth(remainingHpInPercentage);
		enemyModel->Update(t);

		onHitEffect.Update(t);
		onHitEffect.delete_if(true);

	}

	ShotsHandler();


}

//*************** 2D RENDER ***************
void Enemy::OnDraw(CGraphics* g, CVector enemyPos)
{
	if (isDead|| OnSpawnHold) return;
 
	enemyHpbar.SetPosition(enemyPos.x, enemyPos.y + 30);

	CVector displ = localPlayer->playerModel.GetPositionV() - enemyModel->GetPositionV();
	float distance = hypot(displ.GetX(), displ.GetZ());
	
	if(distance < 1200 && !preDeahAnimation) enemyHpbar.Draw(g);

	if (isBoxShowen) UIDialogBox::OnDraw(g);
	 
}



//*************** 3D RENDER ***************
void Enemy::OnRender3D(CGraphics* g)
{
	if (isDead) return;
	enemyModel->Draw(g);
	onHitEffect.Draw(g);
	snowBallList.Draw(g);
}

void Enemy::EnemyGetDamage(float damage , CModel Vfx)
{
	
	if (preDeahAnimation) return;
	for (int i = 0; i < 15; i++)
	{
		CModel* p = Vfx.Clone();
		p->SetPositionV(enemyModel->GetPositionV() + CVector(0, 100, 0));
		p->SetDirection(rand() % 360);
		p->SetSpeed(rand() % 200);
		p->SetColor(CColor::Green());
		p->Die(250);
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
		if (curentPosNum != 1 && curentPosNum != 3) changePosTimer = localTime + 10000;

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
			if (currentDist < closestDistance)
			{
				closestDistance = currentDist;
				targetPos = localPlayer->playerModel.GetPositionV();
			}
		}
 

		CModel* pShot = bullet.Clone();
		pShot->SetPositionV(enemyModel->GetPositionV() + CVector(0, 150, 0));
		pShot->SetDirectionAndRotationToPoint(targetPos.GetX(), targetPos.GetZ());
	 
		pShot->SetSpeed(1000);
		pShot->SetYVelocity(pShot->GetYVelocity() + closestDistance * 0.33);
		pShot->Die(3500);
		pShot->SetStatus(0);
		snowBallList.push_back(pShot);
	}
}

void Enemy::ShotsHandler()
{
	for (CModel* pShot : snowBallList)
	{
		float gravity = 23;
		pShot->SetYVelocity(pShot->GetYVelocity() - gravity);

 
		if (pShot->HitTest(&localPlayer->playerModel)) //add distance
		{
			localPlayer->playerGettingDamage(20);
			pShot->Delete();
		}
	}

	snowBallList.Update(localTime);
	snowBallList.delete_if(true);
}