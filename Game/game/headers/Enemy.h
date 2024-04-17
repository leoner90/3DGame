#pragma once
#include "UIDialogBox.h"
#include <vector>

class Player;
class Map;

class Enemy : public UIDialogBox
{
public:
	~Enemy();
	
	//main Functions
	void init(int index, int enemyType, Map& map, CModelMd3& enemyModel, int curentGameLevel);
	void OnUpdate(Uint32 t, Player& player, Map& map, std::vector<Enemy*>& AllEnemies, CVector enemypos);
	void OnDraw(CGraphics* g);

	void Attack();
	void OnRender3D(CGraphics* g);
	//CModel Vfx)
	void EnemyGetDamage(float damage);
	void EnemyControl();
	void ShotsHandler();
	void initDialogues();
	CModelMd3* enemyModel;

	//TO DO GETTERS
	bool isDead;
	bool preDeahAnimation;
	bool OnSpawnHold;
	bool isFriend;
	bool IsInLineOfSight;
	int localEnemyType;
private:
	
	//local
	Player* localPlayer;
	Map* localMap;
	vector<Enemy*> AllAIPlayerList;
	Uint32 localTime;
	
	int localEnemyIndex;
	int localGameLvl;

	CModelList onHitEffect;
 
	CSoundPlayer deathSound;
	CModel onHitEffectModel;
	Uint32 deathAnimationTimer;
	CHealthBar enemyHpbar;

	//attack
	float enemyDamage;
	float attackDelay;

	//enemy stats
	float enemySpeed;
	float enemyMaxHp, enemyCurrentHp;
	bool onStartOfTheLevel, onStartOfTheLevel2;
 
	//Change Pos Timer
	float changePosTimer;

	//Attack / Hide timer
	float AttackHideTimer;


	vector<CVector > entityAllPos;

	CVector currentDestinationPoint;

	CVector lastFramePos;

	int curentPosNum;
	CModelList snowBallList;
	CModel bullet;
	CSprite hearthHP;

	float snowBallSize;

	float flyTime;

	float oldTime;
	float deltaTime;
};