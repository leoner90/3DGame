#pragma once
#include "../headers/UIDialogBox.h"

//Forwarding Classes
class Map;
class AIPlayer;
class UIDialogBox;
class Shop;

class Player : public UIDialogBox
{
public:
	Player();

	//Main Functions
	void init(int curentGameLevel);
	void OnUpdate(Uint32 t, bool Dkey, bool Akey, bool Wkey, bool Skey, Map& map, std::vector<AIPlayer*> AllAIPlayers, CVector& mousePos, CVector playerWorldPos, float cameraYrot);
	void OnDraw(CGraphics* g);
	void OnRender3D(CGraphics* g);
	void UpdateForCutscene(UINT32 t, CVector playerWorldPos);

	//Main Controler
	void PlayerControl(bool Dkey, bool Akey, bool Wkey, bool Skey);

	//Getting Damage Fucntion
	void playerGettingDamage(float damage);

	//Collisions Handler
	void playerCollision(std::vector<AIPlayer*> AllAIPlayers);

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
	void OnKeyDown(SDLKey sym, CVector currentMousePos);
	
	CVector GetLastPos();
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! GETTERS + SETTERS -> comming soon, probably! !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 
	CModelMd3 playerModel;

	//death handler
	bool isPlayerDead;
	bool playerPreDeahAnimation;

	//Lives
	float playerMaxHp, playerCurrentHp;

	//shooting
	float playerDamage, chargedDamage;
	float shootingDelay, attackDelay; // rename to shootingRate
	int chargePercent;

	CSoundPlayer footsteps; // todo as is called from mygame cpp

	//Roll skill
	float dashCoolDown;
	bool isPlayerInDash;

	//IS Friend for TeamMates
	bool isFriend;

	//Buffs
	bool isPlayerInvulnerable, isPlayerUnderDistanceBuff;
	float InvulnerableBuffTimer, distanceBuffTimer;
	float rangeBuffModifer ;

	//States
	enum playerStates { UNOCCUPIED, INATTACK, INDASH, INDAMAGE, CUTSCENE };
	playerStates playerCurrentState;

	//Hit stan
	bool isPlayerInDamage;
private:
	//Fonts
	CFont font;

	//Player States
	playerStates savedPrevPlayerState;

	//local variables - mirroring
	long localTime;
	int localGameLvl;
	float localcameraYrot;
	Map* localMap;
	std::vector<AIPlayer*> localAllAIPlayers;
	CVector* localMouse;
	

	//player Shots 
	CModelList playerShots;
	CModel bullet;

	//loot => buffs
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

	//Dash - roll skill timer
	float dashTimer;

	//Hit Stan
	float InDamageStunDelayTimer, repeatStunDelayTimer;
	float rangeOfAttack;

	//VFX 
	CModel onDamageVfx;
	CModelList onHitEffect;

	//save Pos
	CVector lastFramePos;

	//Charged Shot Hanlder
	void chargedShotHandler();

	//loot => BUFF
	CModelList lootList;
	float randomLootTimer;
	int lootTimerOffset;
	bool chargingAnimation;
};