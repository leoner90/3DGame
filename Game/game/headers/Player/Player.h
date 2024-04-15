#pragma once

//Forwarding Classes
class Map;
class Enemy;
class UIDialogBox;
class Shop;

class Player 
{
public:
	Player();

	//Main Functions
	void init(int curentGameLevel);
	void OnUpdate(Uint32 t, bool Dkey, bool Akey, bool Wkey, bool Skey, Map& map, std::vector<Enemy*> AllEnemies, CVector& mousePos, CVector playerWorldPos);
	void OnDraw(CGraphics* g, UIDialogBox& dialogBox);
	void OnRender3D(CGraphics* g);

	//Main Controler
	void PlayerControl(bool Dkey, bool Akey, bool Wkey, bool Skey);

	//Getting Damage Fucntion
	void playerGettingDamage(float damage);

	//Collisions Handler
	void playerCollision(std::vector<Enemy*> AllEnemies);

	//Loots
	void addLoot();
	void lootHandler();
	//buff
	void buffHandler();
	
	//shots
	void playerShotsHandler();
	void performShot();

	//Mouse And Keyboard Controll
	void OnRButtonDown(long t);
	void OnRButtonUp();
	void OnMouseMove(CVector currentMousePos);
	void OnLButtonDown(CVector pos, CVector currentMousePos, long t);
	void OnKeyDown(SDLKey sym, CVector currentMousePos);
	
	//GETTERS
		//comming soon,probably!
	//SETTERS
		//comming soon,probably!

	CModelMd3 playerModel;


	//death handler
	bool isPlayerDead;
	bool playerPreDeahAnimation;

	//Lives
	float playerMaxHp, playerCurrentHp;

	int chargePercent;

	//shooting
	float playerDamage, chargedDamage;
	float shootingDelay, attackDelay; // rename to shootingRate

	CSoundPlayer footsteps; // todo as is called from mygame cpp
	float dashCoolDown;
	bool isPlayerInDash;
	bool isFriend;

	//buffs
	bool isPlayerInvulnerable, isPlayerUnderDistanceBuff;
	float InvulnerableBuffTimer, distanceBuffTimer;
	float rangeBuffModifer ;


	bool isPlayerInDamage;
private:
	//Fonts
	CFont font;

	//Player States
	enum playerStates { UNOCCUPIED, INATTACK, INDASH, INDAMAGE };
	playerStates playerCurrentState;
	playerStates savedPrevPlayerState;

	//local variables - mirroring
	long localTime;
	Map* localMap;
	std::vector<Enemy*> localEnemies;
	CVector* localMouse;

	//player Shots 
	CModelList playerShots;
	CModel bullet;

	//loot
	CModel lootItemOne, lootItemTwo, lootItemThree;

	//Sounds
	CSoundPlayer shotSound, hitSound, deathSound, dashSound, PickUpLootSound, charginShotSound;

	//Death Handler
	Uint32 playerdeathAnimationTimer;

	// stop and start Footstep.wav
	bool isPlayerMoving;

	//DeltaTime
	float prevFrameTime, deltatime;

	//Charget Shot
	bool chargedShot, isShotCharged;
	float fullyChargargedShotRange;
	float startChargedShotTimer;
	float totalTimeToCharge;

	//Dash
	float dashTimer;

	//Hit Stan
	
	float InDamageStunDelayTimer, repeatStunDelayTimer;
	float rangeOfAttack;

	//VFX
	CModelList dashEffect;
	CModel dashVfx, onDamageVfx;
	CModelList onHitEffect;

	//save Pos
	CVector lastFramePos;

	//Charged Shot Hanlder
	void chargedShotHandler();

	//loot
	CModelList lootList;
	float randomLootTimer;
	int lootTimerOffset;

};