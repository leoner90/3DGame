#pragma once
#include "Game.h"

//Forwward class declaration
class Map;
class Player;
class PlayerInterface;
class Enemy;
class Cutscene;
class LoadingScreen;
class Radar;

class CMyGame : public CGame
{
public:
	CMyGame();
private:
	CSprite startScreen, mainMenushowControlers, CharStatsMenu, deathScreen;
	CFont font;

	// Main Objects
	Map* map;
	Player* player;
	PlayerInterface* playerInterface;
	Cutscene* cutscene;
	LoadingScreen* loadingScreen;
	Radar* radar;
	std::vector<Enemy*> AIPlayers;

	//death screen
	Uint32 deathScreenTimer;

	//enemies spawn properties
	Uint32 totalEnemiesOnHold;
	float  enemyOneSpawnDelay, enemyTwoSpawnDelay, InitSpawnDelay;
	int totalEnemiesToSpawn;

	// Game Funtions
	virtual void OnInitialize();
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);
	virtual void OnRender3D(CGraphics* g);
	virtual void OnStartLevel(int level);

	void CameraControl(CGraphics* g);
	void MainMenuController(SDLKey sym);
	void enemySpawn(int level);

	//INIT SPRITES AND MODELS
	void InitSpritesAndModels();

	//Mouse Control
	void OnMouseMove(Uint16 x, Uint16 y, Sint16 relx, Sint16 rely, bool bLeft, bool bRight, bool bMiddle);

	// Keyboard Event Handlers
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnRButtonDown(Uint16 x, Uint16 y);
	virtual void OnRButtonUp(Uint16 x, Uint16 y);

	//Game Modes
	enum GameModes { MAIN_MENU, CHAR_STATS, SHOP, IN_GAME, CUTSCENE, SHOW_CONTROLLERS, DEATHSCREEN, LOADING_SCREEN };
	enum mainMenuOptions { NEW_GAME, CONTROLS, EXIT };

	GameModes currentMenuState;
	bool gameStarted;
	int mainMenuOptionSelection;

	//fog
	void EnableFog();

	//pause menus
	void MaiMenuDraw(CGraphics* g);
 
	//sound
	CSoundPlayer mainBgMusic;
	CSoundPlayer weatherBgEffect;
 

	CVector currentMousePosToScreen, currentMousePos;
 
	CVector cameraControlMouseInitPose;
	CSprite mousePointer;
	CModel skydome;

	float YcameraInitState, ZcameraInitState, XcameraInitState, YcameraInitRotation;

	float localH, localW;

	int curentGameLevel;

	CModelMd3* AIPlayerModelOne;
	CModelMd3* AIPlayerModelTwo;
	CModelMd3* boss;
	bool isBossSpawn = false;
};