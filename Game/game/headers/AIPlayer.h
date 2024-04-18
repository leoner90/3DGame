#pragma once
#include "UIDialogBox.h"
#include <vector>

class Player;
class Map;

class AIPlayer : public UIDialogBox
{
public:
	~AIPlayer();
	
	//main Functions
	void init(int index, int enemyType, Map& map, CModelMd3& AIPlayerModel, int curentGameLevel);
	void OnUpdate(Uint32 t, Player& player, Map& map, std::vector<AIPlayer*>& AllAIPlayers, CVector enemypos);
	void OnDraw(CGraphics* g);

	//Control Functions
	void Attack();
	void OnRender3D(CGraphics* g);
	void EnemyGetDamage(float damage);
	void AIPlayerControl();
	void ShotsHandler();
	void initDialogues();
	void AIPlayersInitPositions();
	void AIPlayersInitAnimations();


	//TO DO GETTERS
	bool isDead;
	bool preDeahAnimation;
	bool OnSpawnHold;
	bool isFriend;
	bool IsInLineOfSight;
	int localEnemyType;
	CModelMd3* AIPlayerModel;
private:
	
	//local
	Player* localPlayer;
	Map* localMap;
	vector<AIPlayer*> AllAIPlayerList;
	Uint32 localTime;
	int localEnemyIndex;
	int localGameLvl;

	//onHit
	CModel onHitEffectModel;
	CModelList onHitEffect;

	//Death Tiemr
	Uint32 deathAnimationTimer;

	//attack
	float enemyDamage;
	float attackDelay;

	//enemy stats
	float enemySpeed;
	float enemyMaxHp, enemyCurrentHp;
	bool onStartOfTheLevel;
 
	//Change Pos Timer
	float changePosTimer;

	//Attack / Hide timer
	float AttackHideTimer;

	//AIPlayer Positions
	vector<CVector> entityAllPos;
	CVector currentDestinationPoint;
	int curentPosNum;
	CVector lastFramePos;
	
	//HP
	CSprite hearthHP;

	//Snoball control
	float flyTime;
	float snowBallSize;
	CModel bullet;
	CModelList snowBallList;

	//sound
	CSoundPlayer deathSound;

	//delta time
	float oldTime;
	float deltaTime;
};