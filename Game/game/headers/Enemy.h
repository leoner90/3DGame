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
	void init(int index, int enemyType, Map& map, CModelMd3& enemyModel);
	void OnUpdate(Uint32 t, Player& player, Map& map, std::vector<Enemy*>& AllEnemies, CVector enemypos);
	void OnDraw(CGraphics* g, CVector enemyPos);

	void Attack();
	void OnRender3D(CGraphics* g);
	void EnemyGetDamage(float damage, CModel Vfx);
	void EnemyControl();
	void ShotsHandler();
	CModelMd3* enemyModel;

	//TO DO GETTERS
	bool isDead;
	bool preDeahAnimation;
	bool OnSpawnHold;
	bool isFriend;
	bool IsInLineOfSight;
private:
	//local
	Player* localPlayer;
	Map* localMap;
	vector<Enemy*> AllAIPlayerList;
	Uint32 localTime;
	int localEnemyType;
	int localEnemyIndex;

	CModelList onHitEffect;
	CModel Vfx;
	CSoundPlayer deathSound;

	Uint32 deathAnimationTimer;
	CHealthBar enemyHpbar;

	//attack
	float enemyDamage;
	float attackDelay;

	//enemy stats
	float enemySpeed;
	float enemyMaxHp, enemyCurrentHp;

	

	//Change Pos Timer
	float changePosTimer;

	//Attack / Hide timer
	float AttackHideTimer;

 
	//CVector* entityAllPos;
	vector<CVector > entityAllPos;

	CVector currentDestinationPoint;

	CVector lastFramePos;

	int curentPosNum;

	CModelList snowBallList;
	CModel bullet;

};