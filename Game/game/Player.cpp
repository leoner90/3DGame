#include "Game.h"
#include "headers/Player/Player.h"
#include "headers/map.h"
#include "headers/Enemy.h"
#include "headers/UIDialogBox.h"


//*************** CONSTRUCTOR ***************

Player::Player()
{
	dashVfx.LoadModel("rainDrop/rainDrop.obj");
	dashVfx.SetScale(0.2f);

	onDamageVfx.LoadModel("rainDrop/rainDrop.obj");
	onDamageVfx.SetScale(0.2f);

	isPlayerDead = playerPreDeahAnimation = false;
	font.LoadDefault();

	//model
	playerModel.LoadModel("Player/myne2.md3");
	playerModel.LoadTexture("Player/skin1.png");

	playerModel.SetScale(25.5f);

	//animation
	playerModel.AddAnimation("runF", 42, 63);
	playerModel.AddAnimation("runB", 64, 80);
	playerModel.AddAnimation("runR", 82, 99);
	playerModel.AddAnimation("runL", 101, 117);
	playerModel.AddAnimation("hit", 119, 200);
	playerModel.AddAnimation("idle", 1, 40);
	playerModel.AddAnimation("death", 202, 312);
	playerModel.AddAnimation("dash", 314, 330);

	//Shooting
	bullet.LoadModel("bullet/Bullet2.obj");
	bullet.SetScale(8.f);

	//sounds
	footsteps.Play("footsteps.wav", -1);
	footsteps.SetVolume(20);
	footsteps.Pause();

	//Loot
	lootItemOne.LoadModel("buffs/armor.obj"); // weapon 
	lootItemOne.SetScale(111);
	lootItemTwo.LoadModel("buffs/heart.obj"); // armor
	lootItemTwo.SetScale(0.5f);
	lootItemThree.LoadModel("buffs/muffin.obj");
	lootItemThree.SetScale(42);
}


//*************** INIT ***************
void Player::init()
{
	//reset
	playerModel.SetPosition(1200, 0, 1200);
	isPlayerMoving = false;
	playerShots.delete_all();
	playerCurrentState = UNOCCUPIED;

	//stats
	playerMaxHp = playerCurrentHp = 500;


	//baffs
	armorComponents = weaponComponents = bossLoot = 0;

	isPlayerDead = playerPreDeahAnimation =  false;
	playerdeathAnimationTimer = 0;

	onHitEffect.delete_all();
	dashCoolDown = 0;

	//shooting
	shootingDelay = 400;
	playerDamage = 150;
	chargedDamage = 7; // x

	//charged Shot
	ShotChargeBar.SetSize(35, 1);
	ShotChargeBar.SetColors(CColor::Yellow(), CColor::Black(), CColor::Black());
	ShotChargeBar.SetHealth(0);

	//Reset
	isPlayerInDamage = false;
	chargedShot = false;
	isShotCharged = false;
	isPlayerInDash = false;
	InDamageStunDelayTimer = 0;
	startChargedShotTimer = 0;
	repeatStunDelayTimer = 0;
	dashTimer = 0;
	footsteps.Pause();

	//shooting reset
	totalTimeToCharge = 2000;
	ShotChargeBarOffset = 150;

	//loot
	randomLootTimer = 0;
	lootTimerOffset = 30000;
	rangeOfAttack = 600;

	isFriend = true;
}

//*************** UPDATE ***************
void Player::OnUpdate(Uint32 t, bool Dkey, bool Akey, bool Wkey, bool Skey, Map& map, std::vector<Enemy*> AllEnemies, CVector& mousePos, CVector playerWorldPos)
{
	//if dead -> return (do nothing)
	if (isPlayerDead) return;

	//mirroring
	localMouse = &mousePos; 
	localEnemies = AllEnemies;
	localMap = &map;
	localTime = t;

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
	if (randomLootTimer == 0) randomLootTimer = t + rand() % 15000;
	else if (randomLootTimer < t) addLoot();

	//if moving play start footsteps sound
	if (isPlayerMoving) footsteps.Resume();

	//recovery from hit
	if (isPlayerInDamage && InDamageStunDelayTimer < t)
	{
		isPlayerInDamage = false;
		playerCurrentState = savedPrevPlayerState;
	} 

	playerShots.delete_if(true);
	playerShots.Update(t);

	if (isPlayerInDamage) 
	{
		playerModel.Update(t);
		return;
	}
 
	//Attack Delay 
	if (attackDelay - t <= 0 && playerCurrentState == INATTACK) playerCurrentState = UNOCCUPIED;
	
	//control & collisions
	playerCollision(AllEnemies);
	PlayerControl(Dkey, Akey, Wkey, Skey);

	//shots
	playerShotsHandler();

	//loot
	lootHandler();

	//charged Shot
	chargedShotHandler();

	//Update and deletes
	ShotChargeBar.SetPosition(playerWorldPos.x, playerWorldPos.y + ShotChargeBarOffset);
	ShotChargeBar.Update(t);

	lootList.delete_if(true);
	lootList.Update(t);



	lastFramePos = playerModel.GetPositionV();
	playerModel.Update(t);

	dashEffect.Update(t);
	dashEffect.delete_if(true);

	onHitEffect.Update(t);
	onHitEffect.delete_if(true);	
}

//*************** 2D RENDER ***************
void Player::OnDraw(CGraphics* g, UIDialogBox& dialogBox)
{
 

	if(ShotChargeBar.GetHealth() > 0 ) ShotChargeBar.Draw(g);
}

//*************** 3D RENDER ***************
void Player::OnRender3D(CGraphics* g)
{
	playerShots.Draw(g);
	playerModel.Draw(g);
	lootList.Draw(g);
	dashEffect.Draw(g);
	onHitEffect.Draw(g);
}

 

//*************** PLAYER CONTROLER ***************
void Player::PlayerControl(bool Dkey, bool Akey, bool Wkey , bool Skey )
{
	if (playerModel.AnimationFinished() && playerCurrentState == INDASH)
	{
		isPlayerInDash = false;
		playerCurrentState = UNOCCUPIED;
	}
	if (playerCurrentState == INDASH) return;

	//moving animation
	if (isPlayerMoving)
	{
		CVector charDirectionV = playerModel.GetDirectionV();

		CVector mousePos = CVector(localMouse->GetX(), 0, localMouse->GetZ());
		CVector displ = mousePos - playerModel.GetPositionV();
		CVector normDisp = displ.Normalized();

		float dotProduct = charDirectionV.Dot(normDisp);
		float angle = acos(dotProduct) * 180.0 / M_PI;
	}
 
	//moving direction
	if (Wkey)
	{
		playerModel.SetSpeed(650);
		playerModel.PlayAnimation("runF", 22, true);

		isPlayerMoving = true;
		//if (Dkey) playerModel.SetDirection(1, -1);
		//else if (Akey) playerModel.SetDirection(-1, -1);
		//else playerModel.SetDirection(0, -1);	
	}
	else if (Skey)
	{
		playerModel.SetSpeed(-450);
		playerModel.PlayAnimation("runB", 22, true);
	}
	else
	{
		footsteps.Pause();
		playerModel.SetSpeed(0);
		isPlayerMoving = false;
		playerModel.PlayAnimation("idle", 20, true);
	}

	if (Akey ) { playerModel.Rotate(6); playerModel.SetDirection(playerModel.GetRotation()); }

	if (Dkey ) { playerModel.Rotate(-6); playerModel.SetDirection(playerModel.GetRotation()); }

	/*
	else if (Skey)
	{
		playerModel.SetSpeed(500);
		playerModel.PlayAnimation("runB", 22, true);

		isPlayerMoving = true;
		if (Dkey) playerModel.SetDirection(1, 1);
		else if (Akey) playerModel.SetDirection(-1, 1);
		else playerModel.SetDirection(0, 1);
	}

	else if (Dkey) 
	{
		playerModel.SetSpeed(500);
		playerModel.PlayAnimation("runL", 22, true);
		isPlayerMoving = true;
		playerModel.SetDirection(1, 0);
	}
	else if (Akey)
	{
		playerModel.SetSpeed(500);
		playerModel.PlayAnimation("runL", 22, true);
		isPlayerMoving = true;
		playerModel.SetDirection(-1, 0);
	}
	 */

	
	playerModel.SetDirectionV(playerModel.GetRotationV());
}


void Player::OnKeyDown(SDLKey sym, CVector currentMousePos)
{
	if (sym == SDLK_SPACE)
	{
		if (dashCoolDown < localTime && playerCurrentState != INDASH)
		{
			isPlayerInDash = true;
			dashCoolDown = localTime + 3000;
			playerCurrentState = INDASH;
			dashSound.Play("dash.wav", 1);
			dashSound.SetVolume(75);
			playerModel.PlayAnimation("dash", 85, false);

			//CVector displ = CVector(localMouse->GetX() - playerModel.GetX(), 0, localMouse->GetZ() - playerModel.GetZ());
			//CVector dash = displ.Normalized();
			
			playerModel.SetPositionV(playerModel.GetPositionV() + playerModel.GetDirectionV() * 1000);
			playerModel.SetSpeed(0);
		}
	}
}

void Player::chargedShotHandler()
{
	if (startChargedShotTimer != 0 && !isShotCharged)
	{
		float currentChargeinSec = totalTimeToCharge - (startChargedShotTimer - localTime);
		int chargePercent = currentChargeinSec / totalTimeToCharge * 100;

		if (chargePercent < 50) ShotChargeBar.SetColor(CColor::Red());
		else if (chargePercent < 100) ShotChargeBar.SetColor(CColor::Yellow());
		else if (chargePercent >= 100) ShotChargeBar.SetColor(CColor::Green());
		ShotChargeBar.SetHealth(chargePercent);

		if (startChargedShotTimer < localTime)
		{
			isShotCharged = true;
			rangeOfAttack = 1300;
		}
	}
}

void Player::playerGettingDamage(float damage)
{
	hitSound.Play("PlayerHurt.wav", 1);
	if (playerPreDeahAnimation) return;

	for (int i = 0; i < 25; i++)
	{
		CModel* p = onDamageVfx.Clone();
		p->SetPositionV(playerModel.GetPositionV() + CVector(0, 100, 0));
		p->SetDirection(rand() % 360);
		p->SetSpeed(rand() % 200);
		p->SetColor(CColor::Red());
		p->Die(270);
		onHitEffect.push_back(p);
	}


	if (playerCurrentHp <= 0)
	{
		playerCurrentHp = 0;
		playerPreDeahAnimation = true;
		deathSound.Play("PlayerDeath3.wav", 1);
	}
	else
	{
		if (repeatStunDelayTimer < localTime)
		{
			hitSound.Play("PlayerHurt.wav", 1);
			hitSound.SetVolume(75);
			repeatStunDelayTimer = 1500 + localTime;
			playerModel.PlayAnimation("hit", 162, false);
			InDamageStunDelayTimer = 500 + localTime;
			savedPrevPlayerState = playerCurrentState;
			playerCurrentState = INDAMAGE;
			isPlayerInDamage = true;
			playerModel.SetSpeed(0);
		}
	}
}

void Player::playerCollision(std::vector<Enemy*> AllEnemies)
{
	//end of the map
	for (auto wallSeg : localMap->mapCollision)
	{
		if (playerModel.HitTest(wallSeg))
		{
			playerModel.SetPositionV(lastFramePos);
		}
	}

	//map objects
	for (auto collidingObj : localMap->collidingObjects)
	{
		float distance = 250;
		//if (collidingObj->GetStatus() == 4) distance = 300; // if rock
		if (playerModel.HitTest(collidingObj->GetPositionV(), distance))
		{
			playerModel.SetPositionV(lastFramePos);
		}
	}
	 
	//enemies
	/*for (auto enemy : AllEnemies) {
		if (enemy->deathAnimationTimer) continue;
		if (playerModel.HitTest(enemy->enemyModel)) 
		{
			playerModel.SetPositionV(lastFramePos);
		}
	}*/
}

void Player::addLoot()
{
	randomLootTimer = localTime + rand() % 15000;
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

void Player::performShot()
{
	playerCurrentState = INATTACK;
	attackDelay = localTime + shootingDelay;
	//chargedShot

	CModel* pShot = bullet.Clone();
	pShot->SetPositionV(playerModel.GetPositionV() + CVector(0, 150, 0));
	pShot->SetDirectionAndRotationToPoint(localMouse->GetX(), localMouse->GetZ());
	pShot->SetSpeed(900);
	pShot->SetYVelocity(pShot->GetYVelocity() + rangeOfAttack);
	rangeOfAttack = 600;

	pShot->Die(3500);
	pShot->SetStatus(0);
	playerShots.push_back(pShot);

	//sound
	shotSound.Play("shot.wav", 1);
	shotSound.SetVolume(30);

}

void Player::playerShotsHandler()
{
	for (CModel* pShot : playerShots)
	{
		float gravity = 20;
		pShot->SetYVelocity(pShot->GetYVelocity() - gravity);

		for (auto enemy : localEnemies)
		{
			if (pShot->HitTest(enemy->enemyModel->GetPositionV(), 250)) //add distance
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

				enemy->EnemyGetDamage(damage, onDamageVfx); // to redo vfx ......
			}
		}
	}

}

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
				armorComponents++;
				break;
			case 1:
				weaponComponents++;
				break;
			case 2:
				bossLoot++;
				break;
			default:
				break;
			}
			item->Delete();
			PickUpLootSound.Play("pickUp.wav", 1);
		}
	}
}



 
void Player::OnMouseMove(CVector currentMousePos)
{
	//playerModel.SetRotationToPoint(currentMousePos.x, currentMousePos.z);
}

void Player::OnLButtonDown(CVector pos, CVector currentMousePos, long t)
{
	if (playerCurrentState == UNOCCUPIED )
	{
		performShot();
	}
}


void Player::OnRButtonDown(long t)
{
	if (playerCurrentState == UNOCCUPIED )
	{
		//chargedShot
		if (startChargedShotTimer == 0) startChargedShotTimer = t + totalTimeToCharge;
		charginShotSound.Play("chargedshot.wav");
	}
}


void Player::OnRButtonUp()
{
	startChargedShotTimer = 0;
	ShotChargeBar.SetHealth(0);

	if (isShotCharged)
	{
		chargedShot = true;
		performShot();
		isShotCharged = false;
	}
}

 