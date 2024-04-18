#pragma once
#include "Game.h"

//Forward class declaration
class Map;
class Player;
class PlayerInterface;
class AIPlayer;
class Cutscene;
class LoadingScreen;
class Radar;

class CMyGame : public CGame
{
public:
	CMyGame();
private:
	// Main Objects
	Map* map;
	Player* player;
	PlayerInterface* playerInterface;
	Cutscene* cutscene;
	LoadingScreen* loadingScreen;
	Radar* radar;
	std::vector<AIPlayer*> AllAIPlayerList;

	//AIPLAYERS MODELS
	CModelMd3* AIPlayerModelOne;
	CModelMd3* AIPlayerModelTwo;
	CModelMd3* boss;

	//death screen
	CSprite deathScreen;
	Uint32 deathScreenTimer;

	//Menu Sprites
	CSprite startScreen, mainMenushowControlers;

	//Fonts
	CFont font;

	//Main Funtions
	virtual void OnInitialize();
	virtual void OnStartLevel(int level);
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);
	virtual void OnRender3D(CGraphics* g);

	//Control Functions
	void CameraControl(CGraphics* g);
	void MainMenuController(SDLKey sym);
	void EnableFog(); //fog
	void AIplayersSpawn(int level);

	//INIT SPRITES AND MODELS
	void InitSpritesAndModels();

	//Mouse Control
	void OnMouseMove(Uint16 x, Uint16 y, Sint16 relx, Sint16 rely, bool bLeft, bool bRight, bool bMiddle);

	// Keyboard Event Handlers
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnRButtonDown(Uint16 x, Uint16 y);
	virtual void OnRButtonUp(Uint16 x, Uint16 y);

	//pause menu
	void MaiMenuDraw(CGraphics* g);

	//Game Modes
	enum GameModes { MAIN_MENU, CHAR_STATS, SHOP, IN_GAME, CUTSCENE, SHOW_CONTROLLERS, DEATHSCREEN, LOADING_SCREEN };
	enum mainMenuOptions { NEW_GAME, CONTROLS, EXIT };
	GameModes currentMenuState;

	//Mouse Control variables
	CVector currentMousePosToScreen, currentMousePos;
	CVector cameraControlMouseInitPose;
	CSprite mousePointer;

	//Sound
	CSoundPlayer mainBgMusic;
	CSoundPlayer weatherBgSound;
 
	//variables
	bool gameStarted;
	int mainMenuOptionSelection;
	float localH, localW;
	int curentGameLevel;

	//SkyDome
	CModel skydome;
};