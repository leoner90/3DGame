#include "Game.h"
#include "../headers/Player/Player.h"
#include "../headers/map.h"
#include "../headers/AIPlayer.h"



//*************** CONSTRUCTOR ***************

Player::Player()
{
	font.LoadDefault();

	//On Damage VFX
	onDamageVfx.LoadModel("snowFlake/snowFlake.obj");
	onDamageVfx.SetScale(0.6f);

	isPlayerDead = playerPreDeahAnimation = false;
	

	//model
	playerModel.LoadModel("Player/main.md3");
	playerModel.LoadTexture("Player/main.png");
	playerModel.SetScale(2.f);

	//animation
	playerModel.AddAnimation("run", 1, 20);
	playerModel.AddAnimation("precharge", 22, 62); //62 chargingFrame
	playerModel.AddAnimation("postcharge", 63, 137); //62 chargingFrame
	playerModel.AddAnimation("chargehold", 62, 62);
	playerModel.AddAnimation("death", 139, 216);
	playerModel.AddAnimation("idle", 218, 302);
	playerModel.AddAnimation("dash", 304, 339);
	playerModel.AddAnimation("hit", 341, 378);
	playerModel.AddAnimation("victory", 380, 417);

	//Shooting
	bullet.LoadModel("bullet/Bullet2.obj");
	bullet.SetScale(8.f);

	//sounds
	footsteps.Play("footsteps.wav", -1);
	footsteps.SetVolume(55);
	footsteps.Pause();

	//Loot
	lootItemOne.LoadModel("buffs/armor.obj"); // weapon 
	lootItemOne.SetScale(111);
	lootItemTwo.LoadModel("buffs/heart.obj"); // armor
	lootItemTwo.SetScale(1.f);
	lootItemThree.LoadModel("buffs/muffin.obj");
	lootItemThree.SetScale(52);
}

//*************** INIT ***************
void Player::init(int curentGameLevel)
{
	localGameLvl = curentGameLevel;
	//possition
	if (curentGameLevel == 1)
	{
		playerMaxHp = playerCurrentHp = 3; 	//stats 
		playerModel.SetPosition(2063, 0, -1074);
		//playerModel.SetRotation(180);
	}
	else if (curentGameLevel == 2)
	{
		playerModel.SetPosition(1007, 0, -2274);
		playerModel.SetRotation(270);
	}
	else if (curentGameLevel == 3)
	{
		playerModel.SetPosition(738, 0, -2068);
		playerModel.SetRotation(270);
	}

	//init Animation
	playerModel.PlayAnimation("idle", 20, true);

	//buffs
	isPlayerInvulnerable = isPlayerUnderDistanceBuff = false;
	InvulnerableBuffTimer = distanceBuffTimer = 0;
	rangeBuffModifer = 300;
	fullyChargargedShotRange = 700;

	//death 
	isPlayerDead = playerPreDeahAnimation =  false;
	playerdeathAnimationTimer = 0;

	onHitEffect.delete_all();
	dashCoolDown = 0;

	//shooting
	shootingDelay = 900;
	playerDamage = 1; // -1 live
	chargedDamage = 7; // x

	//Reset
	isPlayerInDamage = false;
	chargedShot = false;
	isShotCharged = false;
	isPlayerInDash = false;
	chargingAnimation = false;
	isPlayerMoving = false;
	InDamageStunDelayTimer = 0;
	startChargedShotTimer = 0;
	repeatStunDelayTimer = 0;
	dashTimer = 0;
	footsteps.Pause();
	playerShots.delete_all();
	playerCurrentState = UNOCCUPIED;
	playerModel.SetSpeed(0);
	//shooting reset
	totalTimeToCharge = 1500;

	//loot => buffs
	randomLootTimer = 0;
	lootTimerOffset = 15000; // evry 30 sec loot apears
	rangeOfAttack = 100;

	isFriend = true;
	lootList.delete_all();
}

//*************** UPDATE ***************
void Player::OnUpdate(Uint32 t, bool Dkey, bool Akey, bool Wkey, bool Skey, Map& map, std::vector<AIPlayer*> AllAIPlayers, CVector& mousePos, CVector playerWorldPos,float cameraYrot)
{
	//shots
	playerShotsHandler();

	//if dead -> return (do nothing)
	if (isPlayerDead) return;

	//mirroring variables
	localMouse = &mousePos; 
	localAllAIPlayers = AllAIPlayers;
	localMap = &map;
	localTime = t;
	localcameraYrot = cameraYrot;

	//delta time
	deltatime = (t - prevFrameTime) / 1000  ; //in sec
	prevFrameTime = t;
 
	//DEATH HANDLER
	float remainingHpInPercentage = playerCurrentHp / (playerMaxHp / 100);
	playerModel.SetHealth(remainingHpInPercentage);

	if (playerPreDeahAnimation)
	{
		if (playerdeathAnimationTimer == 0)
		{
			playerModel.PlayAnimation("death", 43, false);
			playerdeathAnimationTimer = 3000 + t;
		}
		else if (playerdeathAnimationTimer < t) isPlayerDead = true;
		playerModel.SetSpeed(0);
		playerModel.Update(t);
		return;
	}


	//loot timer starter
	if (randomLootTimer == 0) randomLootTimer = t + rand() % lootTimerOffset + 10000;
	else if (randomLootTimer < t) addLoot();

	//if moving play start footsteps sound
	if (isPlayerMoving) footsteps.Resume();

	//recovery from hit
	if (isPlayerInDamage && InDamageStunDelayTimer < t)
	{
		isPlayerInDamage = false;
		playerCurrentState = savedPrevPlayerState;
	} 

	//Delete -Update Shots And VFX if Deleted
	playerShots.delete_if(true);
	playerShots.Update(t);

	onHitEffect.Update(t);
	onHitEffect.delete_if(true);

	if (playerCurrentState == INATTACK && playerModel.AnimationFinished() && !chargingAnimation)
	{
		playerModel.PlayAnimation("chargehold", 1, true); // attackChargingHold name dosen't work
		chargingAnimation == false;
	}
	
	//Attack Delay 
	if (playerCurrentState == INATTACK && startChargedShotTimer == 0 && playerModel.AnimationFinished())   // startChargedShotTimer != 0  indicates that right button relised
	{
		chargingAnimation = false;
		playerCurrentState = UNOCCUPIED;
	}

	//charged Shot
	chargedShotHandler();

	//Update and save Player Pos
	lastFramePos = playerModel.GetPositionV();
	playerModel.Update(t);

	//return if in damage or in Attack
	if (isPlayerInDamage || playerCurrentState == INATTACK) return;
 
	
	//Movement controller
	PlayerControl(Dkey, Akey, Wkey, Skey);

	//collisions
	playerCollision(AllAIPlayers);

	//loot
	lootHandler();

	lootList.delete_if(true);
	lootList.Update(t);

	//remove buff effects when time is finished
	buffHandler();
}

//*************** 2D RENDER ***************
void Player::OnDraw(CGraphics* g)
{
	UIDialogBox::OnDraw(g);
}

//*************** 3D RENDER ***************
void Player::OnRender3D(CGraphics* g)
{
	playerShots.Draw(g);
	playerModel.Draw(g);
	lootList.Draw(g);
	onHitEffect.Draw(g);
}

void Player::UpdateForCutscene(UINT32 t, CVector playerWorldPos)
{
	//Final Cutscene Update player animation And Dialog Box
	UIDialogBox::OnUpdate(t, playerWorldPos, localGameLvl);
	playerModel.Update(t);
}

//*************** PLAYER CONTROLER ***************
void Player::PlayerControl(bool Dkey, bool Akey, bool Wkey , bool Skey )
{
	//cameraYrot
	if (playerCurrentState == INDASH) //ROLL
		playerModel.SetPositionV(playerModel.GetPositionV() + playerModel.GetDirectionV() * 30);
	
	if (playerModel.AnimationFinished() && playerCurrentState == INDASH)
	{
		isPlayerInDash = false;
		playerCurrentState = UNOCCUPIED;
	}
	if (playerCurrentState == INDASH) return;
	
	//moving direction
	if (Wkey)
	{
		playerModel.SetSpeed(650);
		playerModel.PlayAnimation("run", 22, true);
		isPlayerMoving = true;
		playerModel.SetRotation(90 - localcameraYrot);
		//Diagonal
		if(Akey) playerModel.SetRotation(135 - localcameraYrot);
		else if(Dkey) playerModel.SetRotation(45 - localcameraYrot);

	}
	else if (Skey)
	{
		isPlayerMoving = true;
		playerModel.SetSpeed(450);
		playerModel.PlayAnimation("run", 22, true);
		playerModel.SetRotation(270 - localcameraYrot );
		//Diagonal
		if (Akey) playerModel.SetRotation(225 - localcameraYrot);
		else if (Dkey) playerModel.SetRotation(315 - localcameraYrot);
	}
	else if (Akey)
	{
		playerModel.SetSpeed(650);
		playerModel.PlayAnimation("run", 22, true);
		playerModel.SetRotation(180 - localcameraYrot);
	}
	else if (Dkey)
	{
		playerModel.SetSpeed(650);
		playerModel.PlayAnimation("run", 22, true);
		playerModel.SetDirection(1, 0);
		playerModel.SetRotation(0 - localcameraYrot);
	}
	else
	{
		footsteps.Pause();
		playerModel.SetSpeed(0);
		isPlayerMoving = false;
		playerModel.PlayAnimation("idle", 20, true);
	}

	playerModel.SetDirection(playerModel.GetRotation());
}

//*** KEYBOARD EVENTS
void Player::OnKeyDown(SDLKey sym, CVector currentMousePos)
{
	if (sym == SDLK_SPACE && playerCurrentState == UNOCCUPIED) //ROLL SKILL
	{
		if (dashCoolDown < localTime && playerCurrentState != INDASH)
		{
			isPlayerInDash = true;
			dashCoolDown = localTime + 3000;
			playerCurrentState = INDASH;
			dashSound.Play("dash.wav", 1);
			dashSound.SetVolume(75);
			playerModel.PlayAnimation("dash", 60, false);			
			playerModel.SetSpeed(0);
		}
	}
}

CVector Player::GetLastPos()
{
	return lastFramePos;
}

//*** CHARGED SHOT
void Player::chargedShotHandler()
{
	if (startChargedShotTimer != 0 && !isShotCharged)
	{
		float currentChargeinSec = totalTimeToCharge - (startChargedShotTimer - localTime);
		chargePercent = currentChargeinSec / totalTimeToCharge * 100;

		if (startChargedShotTimer < localTime)
		{
			isShotCharged = true;
			rangeOfAttack = 100;
		}
	}
}

//*** GETTING DAMAGE
void Player::playerGettingDamage(float damage)
{
	//return;  //immortality
	if (playerPreDeahAnimation || isPlayerInvulnerable) return;

	playerCurrentHp -= damage;

	//Hit Effect
	for (int i = 0; i < 55; i++)
	{
		CModel* p = onDamageVfx.Clone();
		p->SetPositionV(playerModel.GetPositionV() + CVector(0, 100, 0));
		p->SetDirection(rand() % 360);
		p->SetSpeed(rand() % 200 + 150);
		p->SetColor(CColor::White());
		p->Die(470);
		onHitEffect.push_back(p);
	}

	//Pre-Death handler
	if (playerCurrentHp <= 0)
	{
		playerShots.clear();
		playerCurrentHp = 0;
		playerPreDeahAnimation = true;
		deathSound.Play("Death.wav", 1);
	}
	else //Hit
	{
		if (repeatStunDelayTimer < localTime)
		{
			hitSound.Play("PlayerHurt.wav", 1);
			hitSound.SetVolume(75);
			repeatStunDelayTimer = 1500 + localTime;
			playerModel.PlayAnimation("hit", 20, false);
			InDamageStunDelayTimer = 500 + localTime;
			savedPrevPlayerState = playerCurrentState;
			playerCurrentState = INDAMAGE;
			isPlayerInDamage = true;
			playerModel.SetSpeed(0);
		}
	}
}

//*** COLLISIONS
void Player::playerCollision(std::vector<AIPlayer*> AllAIPlayers)
{
	//End of the map Collisions
	for (auto wallSeg : localMap->mapCollision)
	{
		if (playerModel.HitTest(wallSeg))
		{
			playerModel.SetPositionV(lastFramePos);
		}
	}

	//Map objects Collisions
	for (auto collidingObj : localMap->collidingObjects)
	{
		float distance = 200;
		if (collidingObj->GetStatus() == 6) distance = 400; // if house
		if (collidingObj->GetStatus() == 1 ) distance = 200; // if iglo or barrel
		if ( collidingObj->GetStatus() == 7) distance = 0; // if iglo 

		//Iglo have very strange behavior if apply distance......
		if (distance != 0) if (playerModel.HitTest(collidingObj->GetPositionV(), distance)) playerModel.SetPositionV(lastFramePos);
		if (distance == 0) if (playerModel.HitTest(collidingObj)) playerModel.SetPositionV(lastFramePos);
	}
	 
	//Enemies Collisions
	for (auto AIPlayer : AllAIPlayers) {
		if (AIPlayer->preDeahAnimation) continue;
		if (playerModel.HitTest(AIPlayer->AIPlayerModel))
		{
			playerModel.SetPositionV(lastFramePos);
		}
	}
}

//*** BUFF => LOOT 
void Player::addLoot()
{
	//Create Random Loot at Random Position
	randomLootTimer = localTime + rand() % lootTimerOffset + 10000;
	CVector possitionsAllowed[9] =
	{
		CVector(2441 + rand() % 60 ,0, 1751 + rand() % 60),
		CVector(606 + rand() % 60,0, -2368 + rand() % 60),
		CVector(-834 + rand() % 60, 0, -2434 + rand() % 60),
		CVector(-1358 + rand() % 60,0, 2394 + rand() % 60),
		CVector(826 + rand() % 60,0, 2265 + rand() % 60),
		CVector(2436 + rand() % 60,0, -36 + rand() % 60),
		CVector(34 + rand() % 60,0, -1637 + rand() % 60),
		CVector(-1365 + rand() % 60,0, 1475 + rand() % 60),
		CVector(1234 + rand() % 60,0, 1813 + rand() % 60)
	};

	float typeOfloot = rand() % 3;
 
	if(typeOfloot ==  0) lootList.push_back(lootItemOne.Clone());
	else if (typeOfloot == 1) lootList.push_back(lootItemTwo.Clone());
	else if (typeOfloot == 2) lootList.push_back(lootItemThree.Clone());
	lootList.back()->SetOmega(75);
	lootList.back()->SetPositionV(possitionsAllowed[rand() % 9]);
	lootList.back()->SetStatus(typeOfloot);
}

//*** CREATE SHOT
void Player::performShot()
{
	//playerCurrentState = INATTACK;
	attackDelay = localTime + shootingDelay;
	//chargedShot
	playerModel.PlayAnimation("attack", 20, true);

	CModel* pShot = bullet.Clone();
	pShot->SetPositionV(playerModel.GetPositionV() + CVector(0, 150, 0));
	pShot->SetDirectionAndRotationToPoint(localMouse->GetX(), localMouse->GetZ());
	pShot->SetSpeed(900);

	//RANGE Calculation dependin on Buff and How long shot was prepared
	rangeOfAttack = fullyChargargedShotRange * chargePercent / 100;
	if (isPlayerUnderDistanceBuff) rangeOfAttack + rangeBuffModifer;

	pShot->SetYVelocity(pShot->GetYVelocity() + rangeOfAttack);
	rangeOfAttack = 600;

	pShot->Die(3500);
	pShot->SetStatus(0);
	playerShots.push_back(pShot);

	//sound
	shotSound.Play("shot.wav", 1);
	shotSound.SetVolume(30);

}

//*** SHOT HANDLER
void Player::playerShotsHandler()
{
	//Shot Collision With The Map
	for (CModel* pShot : playerShots)
	{
		for (auto mapObj : localMap->collidingObjects)
		{
			if (pShot->HitTest(mapObj))
			{
				pShot->Delete();
				continue;
			}
		}

		//Gravity
		float gravity = 20;
		pShot->SetYVelocity(pShot->GetYVelocity() - gravity);

		//Shot Collision With AIPlayers
		for (auto enemy : localAllAIPlayers)
		{
			if (enemy->isFriend) continue;
			if (pShot->HitTest(enemy->AIPlayerModel->GetPositionV(), 150)) //add distance
			{
				if (enemy->preDeahAnimation) continue;
				pShot->Delete();
				float damage;

				if (chargedShot)  
				{
					damage = playerDamage * chargedDamage;
					chargedShot = false;
					rangeOfAttack = 1000;
				}
				else damage = playerDamage;

				enemy->EnemyGetDamage(damage); // to redo vfx ......
			}
		}
	}
}

//*** BUFF ON PICK UP
void Player::lootHandler()
{
	//LOOT - DROP
	for (auto item : lootList)
	{
		if (playerModel.HitTest(item->GetPositionV(), 150))
		{
			int itemType = item->GetStatus();
			switch (itemType)
			{
			case 0:
				isPlayerInvulnerable = true;
				InvulnerableBuffTimer = localTime + 9000;
				break;
			case 1:
				if (playerCurrentHp < playerMaxHp) playerCurrentHp++;
				break;
			case 2:
				isPlayerUnderDistanceBuff = true;
				distanceBuffTimer = localTime + 12000;
				break;
			default:
				break;
			}
			item->Delete();
			PickUpLootSound.Play("pickUp.wav", 1);
		}
	}
}

//*** BUFF Reset
void Player::buffHandler()
{
	if (InvulnerableBuffTimer < localTime) isPlayerInvulnerable = false;
	if (distanceBuffTimer < localTime) isPlayerUnderDistanceBuff = false;
}


//*** Mouse Evenets
void Player::OnRButtonDown(long t)
{
	if (playerCurrentState == UNOCCUPIED && !playerPreDeahAnimation)
	{
		//chargedShot
		playerCurrentState = INATTACK;
		playerModel.SetSpeed(0);
		playerModel.PlayAnimation("precharge", 40,false);
		if (startChargedShotTimer == 0) startChargedShotTimer = localTime + totalTimeToCharge;
		charginShotSound.Play("chargeAttack.wav");

		playerModel.SetDirectionAndRotationToPoint(localMouse->GetX(), localMouse->GetZ());
	}
}

void Player::OnRButtonUp()
{
	//Shoot
	if (startChargedShotTimer != 0) // attack delay (if (playerCurrentState == INATTACK) - less relayeble
	{	
		playerModel.PlayAnimation("postcharge", 80, false); // just throwing animation
		isShotCharged = false;
		charginShotSound.Clear();
		startChargedShotTimer = 0;
		performShot();
		chargePercent = 0;
	}
}