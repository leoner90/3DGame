#include "Game.h"
#include "headers/AIPlayer.h"
#include "headers/Player/Player.h"
#include "headers/Map.h"
#include <math.h> 


//*************** DELETE ***************
AIPlayer::~AIPlayer()
{
	delete AIPlayerModel;
}

//*************** INIT ***************
void AIPlayer::init(int index, int enemyType, Map& map, CModelMd3& AIPlayerMod, int curentGameLevel)
{
	//resets
	onStartOfTheLevel = true;
	AttackHideTimer = 0;
	changePosTimer = 0;
	IsInLineOfSight = false;
	curentPosNum = 0;

	//local Variables
	localEnemyIndex = index;
	localGameLvl = curentGameLevel;
	localEnemyType = enemyType;
	AIPlayerModel = AIPlayerMod.Clone();

	//Variables
	snowBallSize = 8.f;
	enemyDamage = 1;
	enemyMaxHp = enemyCurrentHp = 3;
	enemySpeed = 350;

	//INIT Pos
	AIPlayersInitPositions();

	//Death state Reset
	isDead = preDeahAnimation = false;
	deathAnimationTimer = 0;

	//wait at spawn point
	OnSpawnHold = true;
	AIPlayerModel->SetToFloor(0);

	//ON HIT VFX Handler - yes this will eat memory
	onHitEffect.delete_all();
	onHitEffectModel.LoadModel("snowFlake/snowFlake.obj");
	onHitEffectModel.SetScale(0.6f);

	// ** Animation
	AIPlayersInitAnimations();

	//INIT Animation
	AIPlayerModel->PlayAnimation("idle", 3, true);

	//Shooting  
	bullet.LoadModel("bullet/Bullet2.obj");
	bullet.SetScale(snowBallSize);

	//HP
	if (isFriend) hearthHP.LoadImage("lifeHeartFriend.png");
	else hearthHP.LoadImage("lifeHeart.png");
	hearthHP.SetSize(15, 15);

	//Delta Time
	oldTime = deltaTime = 0;
}

//*************** UPDATE ***************
void AIPlayer::OnUpdate(Uint32 t, Player &player, Map& map, std::vector<AIPlayer*>& AllAIPlayers, CVector enemypos)
{
	if (isDead) return;

	//DeltaTime
	if (oldTime != 0) deltaTime = oldTime - t;
	oldTime = t;

	//In Pre Death Condition
	if (preDeahAnimation)
	{
		if (deathAnimationTimer == 0)
		{
			if(localEnemyType == 2) AIPlayerModel->PlayAnimation("dead", 50, false);
			else AIPlayerModel->PlayAnimation("dead", 33, false);
			AIPlayerModel->SetSpeed(0);
			deathAnimationTimer = 2000 + t;
		}
		else if(deathAnimationTimer < t) isDead = true;
		
		onHitEffect.Update(t);
		onHitEffect.delete_if(true);
		AIPlayerModel->Update(t);
		return;
	}

	//local Variables
	localPlayer = &player;
	localMap = &map;
	localTime = t;
	AllAIPlayerList = AllAIPlayers;

	//set world pos for dialog and hp
	UIDialogBox::OnUpdate(localTime, enemypos);
	hearthHP.SetPosition(enemypos.GetX() - 30, enemypos.GetY() + 30);

	AIPlayerModel->Update(t);

	if (!OnSpawnHold) // false on actual game start
	{	
		initDialogues(); //start of the level Dialogues
		AIPlayerControl();
		ShotsHandler();
	}
	//Hit effect updater
	onHitEffect.Update(t);
	onHitEffect.delete_if(true);
}

//*************** 2D RENDER ***************
void AIPlayer::OnDraw(CGraphics* g)
{
	UIDialogBox::OnDraw(g);
	if (isDead || OnSpawnHold) return;

	// if player is far away or dialog box showen Do not Draw HP
	CVector displ = localPlayer->playerModel.GetPositionV() - AIPlayerModel->GetPositionV();
	float distance = hypot(displ.GetX(), displ.GetZ());
	
	if (distance < 3000 && !preDeahAnimation && !isBoxShowen)
	{
		for (int i = 0; i < enemyCurrentHp; i++)
		{
			hearthHP.SetX(hearthHP.GetX() + 20);
			hearthHP.Draw(g);
		}
	}
}

//*************** 3D RENDER ***************
void AIPlayer::OnRender3D(CGraphics* g)
{
	if (isDead) return;
	AIPlayerModel->Draw(g);
	onHitEffect.Draw(g);
	snowBallList.Draw(g);
}

void AIPlayer::EnemyGetDamage(float damage)
{
	if (preDeahAnimation) return;

	//hit Effect
	for (int i = 0; i < 15; i++)
	{
		CModel* p = onHitEffectModel.Clone();
		p->SetPositionV(AIPlayerModel->GetPositionV() + CVector(0, 100, 0));
		p->SetDirection(rand() % 360);
		p->SetSpeed(rand() % 200 + 150);
		p->SetColor(CColor::White());
		p->Die(450);
		onHitEffect.push_back(p);
	}

	enemyCurrentHp -= damage; //Damage

	//If dead
	if (enemyCurrentHp <= 0)
	{
		snowBallList.clear();
		deathSound.Play("Death.wav");
		deathSound.SetVolume(50);
		preDeahAnimation = true;
	}
}

void AIPlayer::AIPlayerControl()
{
	//init
	if (changePosTimer == 0)
	{
		changePosTimer = localTime;
		AttackHideTimer = localTime;
	}
	// if time to change Pos , select next pos and move there
	if (changePosTimer <= localTime && !AIPlayerModel->IsAutoMoving())
	{
		curentPosNum++;
		if (curentPosNum > 3) curentPosNum = 0;
		currentDestinationPoint = entityAllPos[curentPosNum];

		AIPlayerModel->PlayAnimation("run", 35, true);
		AIPlayerModel->SetDirectionAndRotationToPoint(currentDestinationPoint.GetX(), currentDestinationPoint.GetZ());
	 	AIPlayerModel->MoveTo(currentDestinationPoint.GetX(), currentDestinationPoint.GetZ(), enemySpeed);
		if ((curentPosNum != 1 && curentPosNum != 3) || localGameLvl == 3) changePosTimer = localTime + 20000; //skipps middle pos in level 1 and 2, but not 3

	}
	//if not moving -> Attack
	else if (!AIPlayerModel->IsAutoMoving())
		Attack();

}

void AIPlayer::Attack()
{
	if (attackDelay < localTime)
	{
		AIPlayerModel->PlayAnimation("attack", 22, true);

		//!!!!! NOT EFFECTIVE AS IT CHECK POSSITION ALL THE TIME INSTEAD OF DOING IT ONCE (when actual attack happens), 
		// BUT IT ROTATES ATTACK ANIMATION TOWARDS TARGET EVERY FRAME WHICH LOOK NICE!!!! 
		
		//finding Closest Distance
		float closestDistance = 0;
		float currentDist = 0;
		CVector targetPos = { 0,0,0 };

		//compare With All AI players
		for (auto AIPlayer : AllAIPlayerList)
		{
			//continue If target is a friend 
			if (isFriend == AIPlayer->isFriend) continue;

			currentDist = AIPlayerModel->GetPositionV().Distance(AIPlayer->AIPlayerModel->GetPositionV());

			if (currentDist < closestDistance || closestDistance == 0)
			{
				closestDistance = currentDist;
				targetPos = AIPlayer->AIPlayerModel->GetPositionV();
			}
		}

		//Compare Distance to the Player If target is not friend 
		if (isFriend != localPlayer->isFriend)
		{
			float distanceToPlayer = 0;
			distanceToPlayer = localPlayer->playerModel.GetPositionV().Distance(AIPlayerModel->GetPositionV());
			if (distanceToPlayer < closestDistance || closestDistance == 0)
			{
				closestDistance = distanceToPlayer;
				targetPos = localPlayer->playerModel.GetPositionV();


				if (localPlayer->playerModel.GetSpeed() != 0)
				{
					flyTime = closestDistance / 1000.f;
					// player Movement Prediction  
					CVector playerMovemntNormal = CVector(localPlayer->playerModel.GetXVelocity(), localPlayer->playerModel.GetYVelocity(), localPlayer->playerModel.GetZVelocity()).Normalized();
					CVector playerMovementPrediction = playerMovemntNormal * flyTime; // need delta time.............
					targetPos += playerMovementPrediction;
				}
			}
		}

		//Rotate AIPlayer towards target
		AIPlayerModel->SetDirectionAndRotationToPoint(targetPos.GetX(), targetPos.GetZ());

		//if end Of animation, throw snowball
		if (AIPlayerModel->GetFrame() < 90) return; // attack by the end of the attack animations
		
		attackDelay = localTime + 3000;
		flyTime = closestDistance / 1000.f;

		//Shot
		CModel* pShot = bullet.Clone();
		pShot->SetPositionV(AIPlayerModel->GetPositionV() + CVector(0, 150, 0));
		pShot->SetDirectionAndRotationToPoint(targetPos.GetX(), targetPos.GetZ());
		pShot->SetSpeed(1000);
		pShot->SetYVelocity(closestDistance * 0.36);
		pShot->Die(5500);
		pShot->SetStatus(0);
		snowBallList.push_back(pShot);
	}
}

void AIPlayer::ShotsHandler()
{
	for (CModel* pShot : snowBallList)
	{
		//delete if hit the map
		for (auto mapObj : localMap->collidingObjects)
		{
			if (pShot->HitTest(mapObj))
			{
				pShot->Delete();
				continue;
			}
		}

		//Gravity
		float gravity = 23;
		pShot->SetYVelocity(pShot->GetYVelocity() - (float)gravity);

		//Hit Test Against Player
		if (pShot->HitTest(&localPlayer->playerModel) && !isFriend) //add distance
		{	
			localPlayer->playerGettingDamage(enemyDamage);
			pShot->Delete();
		}
		//Hit Test Against Other AIPlayer if the Friend status is diferent
		for (auto AIPlayer : AllAIPlayerList)
		{
			if (pShot->HitTest(AIPlayer->AIPlayerModel) && isFriend != AIPlayer->isFriend)
			{
				AIPlayer->EnemyGetDamage(enemyDamage);
				pShot->Delete();
			}
		}
	}

	snowBallList.Update(localTime);
	snowBallList.delete_if(true);
}

void AIPlayer::initDialogues()
{
	//level 1 entry Dialog Box
	if (onStartOfTheLevel && localGameLvl == 1)
	{
		onStartOfTheLevel = false;
		if (!isFriend) UIDialogBox::showBox(localEnemyIndex, localEnemyIndex, localEnemyIndex, 10000);
		else UIDialogBox::showBox(5, 3, 3, 10000); // friend id 5
	}

	//level 2 entry Dialog Box
	if (onStartOfTheLevel && localGameLvl == 2)
	{
		if (!isFriend) UIDialogBox::showBox(localEnemyIndex, 7+ localEnemyIndex, 7+ localEnemyIndex, 10000);
		else UIDialogBox::showBox(5, 10, 10, 10000); // friend id 5
		onStartOfTheLevel = false;
	}

	//level 3 entry Dialog Box
	if (onStartOfTheLevel && localGameLvl == 3)
	{
		//the best code ever!!!!
		int speakerId = 5;
		if (localEnemyIndex == 1) speakerId = 3;
	    UIDialogBox::showBox(speakerId, 14 + localEnemyIndex, 14 + localEnemyIndex, 10000);
		onStartOfTheLevel = false;

	}
}

void AIPlayer::AIPlayersInitPositions()
{
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
			if (localEnemyIndex == 2) // enemy 3  BOSS
			{
				entityAllPos.push_back(CVector(-1156, 100, -521));
				entityAllPos.push_back(CVector(-320, 100, 1182));
				entityAllPos.push_back(CVector(378, 100, 2453));
				entityAllPos.push_back(CVector(-1156, 100, -521));

				enemyDamage = 2;
				snowBallSize = 25.f;
			}
		}

		if (localGameLvl == 3)
		{
			if (localEnemyIndex == 0) // enemy 1 pos ( friend)
			{
				enemyMaxHp = enemyCurrentHp = 6;
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

				enemyDamage = 2;
				snowBallSize = 25.f;
			}
		}

		isFriend = false;
		AIPlayerModel->SetPositionV(entityAllPos[0]);
	}

	else if (localEnemyType == 1)
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

		AIPlayerModel->SetPositionV(entityAllPos[0]);
		isFriend = true;
	}
}

void AIPlayer::AIPlayersInitAnimations()
{
	if (localEnemyType == 0)
	{
		AIPlayerModel->AddAnimation("run", 1, 36);
		AIPlayerModel->AddAnimation("attack", 36, 142);
		AIPlayerModel->AddAnimation("dead", 144, 211);
		AIPlayerModel->AddAnimation("idle", 125, 138);
	}
	//friend
	if (localEnemyType == 1)
	{
		enemyMaxHp = enemyCurrentHp = 6;
		AIPlayerModel->AddAnimation("run", 1, 36);
		AIPlayerModel->AddAnimation("attack", 36, 142);
		AIPlayerModel->AddAnimation("dead", 144, 211);
		AIPlayerModel->AddAnimation("idle", 125, 138);
	}
	//boss
	else if (localEnemyType == 2)
	{
		enemyMaxHp = enemyCurrentHp = 8;
		AIPlayerModel->AddAnimation("run", 1, 22);
		AIPlayerModel->AddAnimation("attack", 138, 240);
		AIPlayerModel->AddAnimation("dead", 24, 124);
		AIPlayerModel->AddAnimation("idle", 138, 155);
	}
}