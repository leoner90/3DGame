#pragma once
#include "Game.h"

//Forwward class declaration
class Map;
class Player;
class PlayerInterface;
class Enemy;
class Shop;
class UIDialogBox;
class Cutscene;
class LoadingScreen;
class Portal;
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
	UIDialogBox* dialogBox;
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
	void OnWheelUp(Uint16 x, Uint16 y);
	void OnWheelDown(Uint16 x, Uint16 y);

	// Keyboard Event Handlers
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnLButtonDown(Uint16 x, Uint16 y);
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
	CSoundPlayer rainBgEffect;
	CSoundPlayer bossSpawnSound;

	void OnMButtonDown(Uint16 x, Uint16 y);
	void OnMButtonUp(Uint16 x, Uint16 y);

	bool cameraMovement;

	CVector currentMousePos;
	CVector cameraControlMouseInitPose;
	CSprite mousePointer;
	CModel skydome;

	float YcameraInitState, ZcameraInitState, XcameraInitState, YcameraInitRotation;

	float localH, localW;


	//Static
	CModelMd3* enemyModelOne;
	CModelMd3* enemyModelTwo;
	CModelMd3* boss;
	bool isBossSpawn = false;



};