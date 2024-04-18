#include "../headers/MyGame.h"
#include "../headers/Player/Player.h"
#include "../headers/Player/PlayerInterface.h"
#include "../headers/AIPlayer.h"
#include "../headers/Map.h"
#include "../headers/Cutscene.h"
#include "../headers/loadingScreen.h"
#include "../headers/Radar.h"

CMyGame::CMyGame()
{
	HideMouse(); // :)
}

//*************** INIT ***************
void CMyGame::OnInitialize()
{
	//Base init variables
	currentMenuState = MAIN_MENU;
	mainMenuOptionSelection = NEW_GAME;
	SetGameMode(MODE_PAUSED);
	curentGameLevel = 1;
	localW = (float)Width;
	localH = (float)Height;

	//Fog & Light
	Light.Enable();
	font.LoadDefault();
	EnableFog();

	//Main Objects
	map = new Map();
	cutscene = new Cutscene(localW, localH);
	player = new Player();
	playerInterface = new PlayerInterface(localW, localH);
	radar = new Radar(localW, localH); // radar
	loadingScreen = new LoadingScreen(localW, localH);
	loadingScreen->init();	//loading screen init
	
	//SPRITES AND Models INIT
	InitSpritesAndModels();

	//music
	mainBgMusic.Play("mainBg.wav", -1);
	mainBgMusic.SetVolume(11);
 
	//Mouse Init Controll
	mousePointer.SetPosition(localW / 2, localH / 2 + 300);
	HideMouse();
	//SDL_WM_GrabInput(SDL_GRAB_ON);
}

//ON NEW LEVEL START
void CMyGame::OnStartLevel(int level)
{
	// for final cutscene to draw map and player from Level 3!!!!
	bool finalCutscene = false;
	if (level > 3)
	{
		finalCutscene = true;
		level = 3;
	}

	//Main inits
	map->init(level);
	playerInterface->init(localW, localH); 
	radar->init();
	player->init(level);
 
	//resets
	gameStarted = false;
	SetGameMode(MODE_PAUSED);
	deathScreenTimer = 0;

	//Wind Sound Pause
	weatherBgSound.Pause();

	//****AIplayer clear
	for (auto AIplayer : AllAIPlayerList) delete AIplayer;
	AllAIPlayerList.clear();

	// spawn
	if(!finalCutscene) AIplayersSpawn(level);
	cutscene->init(localW, localH, *player);
}

//*************** UPDATE ***************
void CMyGame::OnUpdate() 
{
	Uint32 t = (float)GetTime();
	currentMousePosToScreen = ScreenToFloorCoordinate(mousePointer.GetX(), mousePointer.GetY());

	//End Of the game Win
	if (cutscene->curentCutSceneNum == 4 && !cutscene->isCutscenePlaying)
	{
		currentMenuState = MAIN_MENU;
		SetGameMode(MODE_PAUSED);
		gameStarted = false;
	}

	//END OF EACH CUTSCENE
	if (!cutscene->isCutscenePlaying && currentMenuState == CUTSCENE && cutscene->curentCutSceneNum == curentGameLevel)
	{
		currentMenuState = IN_GAME;
		camera.rotation.y = 0;
		camera.position.z = 2000;
		weatherBgSound.Play("weather.wav", -1);
		weatherBgSound.SetVolume(150);
		for (auto AIPlayer : AllAIPlayerList) AIPlayer->OnSpawnHold = false;
	}

	//UPDATES IF CUSCENE RUNNING
	if (currentMenuState == CUTSCENE && gameStarted)
	{
		map->OnUpdate(t, *player);
		CVector playerWorldPos = player->playerModel.GetPositionV() + CVector(0, player->playerModel.GetTop(), 0);
		player->UpdateForCutscene(t, WorldToScreenCoordinate(playerWorldPos));
		bool fullWidth = true;
		
		for (auto AiPlayer : AllAIPlayerList)
		{
			CVector AiPlayerpos = AiPlayer->AIPlayerModel->GetPositionV() + CVector(0, AiPlayer->AIPlayerModel->GetTop(), 0);
			AiPlayer->OnUpdate(GetTime(), *player, *map, AllAIPlayerList, WorldToScreenCoordinate(AiPlayerpos));
		}
		cutscene->Update(t, AllAIPlayerList);
		return;
	}

	//GAME OVER - Death Screen Handler  
	if ((player->isPlayerDead) && gameStarted)
	{
		currentMenuState = DEATHSCREEN;
		gameStarted = false;
		SetGameMode(MODE_MENU);
		deathScreenTimer = t + 4000;
	} 
	else if(player->isPlayerDead && deathScreenTimer <= t)
	{
		currentMenuState = MAIN_MENU;
		SetGameMode(MODE_PAUSED);
		gameStarted = false;
	}

	//IF LOADING_SCREEN 
	if(currentMenuState == LOADING_SCREEN) loadingScreen->Update(t);

	//RETURN IF MENU MODE
	if ( IsGameOver() || IsPaused() || IsMenuMode() || currentMenuState != IN_GAME) return; // IN GAME , when exit the game mode == exit rising bugs
	
	//IF GAME RUNNING UPDATES
	map->OnUpdate(t, *player);
	player->OnUpdate(t, IsKeyDown(SDLK_d), IsKeyDown(SDLK_a), IsKeyDown(SDLK_w), IsKeyDown(SDLK_s), *map, AllAIPlayerList, currentMousePosToScreen, WorldToScreenCoordinate(player->playerModel.GetPositionV()), camera.rotation.y);
	playerInterface->OnUpdate(t, *player);
	radar->OnUpdate(t, AllAIPlayerList, *player, *map);
	
 
	//ALL AI PLAYERS
	bool isEnemyAlive = false;
	for (auto AiPlayer : AllAIPlayerList)
	{
		if (!AiPlayer->isFriend) isEnemyAlive = true; // If at least one enemy alive
	
		CVector AiPlayerpos = AiPlayer->AIPlayerModel->GetPositionV() + CVector(0, AiPlayer->AIPlayerModel->GetTop(), 0);
		AiPlayer->OnUpdate(GetTime(), *player, *map, AllAIPlayerList, WorldToScreenCoordinate(AiPlayerpos));

		//* IF AIPlayer is dead -> delete;
		if (AiPlayer->isDead)
		{
			delete AiPlayer;
			AllAIPlayerList.erase(find(AllAIPlayerList.begin(), AllAIPlayerList.end(), AiPlayer));
		}
	}

	//IF NO ENEMY LEFT - START NEXT GAME LEVEL
	if (!isEnemyAlive)
	{
		curentGameLevel++;
		OnStartLevel(curentGameLevel);
		StartGame();
		gameStarted = true;
		cutscene->startCutscene(curentGameLevel);
		currentMenuState = CUTSCENE;

	}
}

//*************** 2D RENDER ***************
void CMyGame::OnDraw(CGraphics* g)
{
	// IF MENU MODE
	if (IsPaused() || IsMenuMode())
	{
		if (currentMenuState == MAIN_MENU || currentMenuState == SHOW_CONTROLLERS) MaiMenuDraw(g);
		else if (currentMenuState == DEATHSCREEN) deathScreen.Draw(g);
		else if (currentMenuState == LOADING_SCREEN) loadingScreen->OnDraw(g);
		return;
	}

	// IF IN CUTSCENE
	if (currentMenuState == CUTSCENE && gameStarted)
	{
		cutscene->Draw2d(g); 
		for (auto AIPlayer : AllAIPlayerList) AIPlayer->OnDraw(g);
		player->OnDraw(g);
		return;
	}

	//NOT IN GAME
	if(currentMenuState != IN_GAME) return; 

	//IN GAME!
	mousePointer.Draw(g);
	player->OnDraw(g);
	radar->OnDraw(g);
	playerInterface->OnDraw(g);
	for (auto AIPlayer : AllAIPlayerList) AIPlayer->OnDraw(g);
}

//*************** 3D RENDER ***************
void CMyGame::OnRender3D(CGraphics* g)
{ 
	//NOT IN GAME
	if (IsGameOver() || IsPaused() || IsMenuMode()) return;

	//IN GAME!
	CameraControl(g);
	map->OnRender3D(g);
	player->OnRender3D(g);
	for (auto AIPlayer : AllAIPlayerList) AIPlayer->OnRender3D(g);
}

//*************** CAMERA ***************
void CMyGame::CameraControl(CGraphics* g)
{
	// ------ Global Transformation Functions -----
	//glScalef(world.scale, world.scale, world.scale);  // scale the game world
	//glTranslatef(world.position.x, world.position.y, world.position.z);  // translate game world

	if (currentMenuState == CUTSCENE && gameStarted)
	{
		glTranslatef(0, 0, cutscene->cutcceneCameraPosition.GetZ() + cutscene->cameraOffset);		//zzzzz
		glRotatef(cutscene->xWorldRotation, 1, 0, 0);	// rotate game world around x-axis // change
	}
	else
	{
		glTranslatef(0, 0, camera.position.z);  // translate game world in z-axis direction
		glRotatef(15, 1, 0, 0);	// rotate game world around x-axis
	}
	
	if (currentMenuState == CUTSCENE && gameStarted)
	{
		camera.rotation.y = cutscene->camerRotationAngle;
		glRotatef(camera.rotation.y, 0, 1, 0);	// rotate game world around x-axis
	}
	else  glRotatef(camera.rotation.y, 0, 1, 0);	// rotate game world around x-axis

	if (currentMenuState == CUTSCENE && gameStarted ) glTranslatef(0, -cutscene->cutcceneCameraPosition.GetY() - 800 ,  0);
	else  
		//glTranslatef(0, -player->playerModel.GetY() + 0 - camera.position.y, 0);
		glRotatef(-player->playerModel.GetDirection() + 90, 0, 1, 0); 	
		
	// it makes the skydome stationary / draw the skydome before game world is translated
	skydome.Draw(g);
	
	// position game world at the player position (excluding skydome)
	if (currentMenuState == CUTSCENE && gameStarted ) glTranslatef(-cutscene->cutcceneCameraPosition.GetX(), 700, -cutscene->cutcceneCameraPosition.GetZ());
	else glTranslatef(-player->playerModel.GetX(), -150, -player->playerModel.GetZ());
	
	UpdateView();
	Light.Apply();
}


//INIT SPRITES
void CMyGame::InitSpritesAndModels()
{
	// Main Menu screen
	startScreen.LoadImage("startScreen.jpg");
	startScreen.SetSize((float)Width, (float)Height);
	startScreen.SetPosition((float)Width / 2, (float)Height / 2);

	//Main Menu Controllers
	mainMenushowControlers.LoadImage("mainMenushowControlers.jpg");
	mainMenushowControlers.SetSize((float)Width, (float)Height);
	mainMenushowControlers.SetPosition((float)Width / 2, (float)Height / 2);

	// Load skydome geometry and texture
	skydome.LoadModel("Skydome/Skydome.obj", "Skydome/Skydome.bmp");
	skydome.SetScale(400);
	skydome.SetY(-300.0f);

	//deathScreen
	deathScreen.LoadImage("deathScreen.jpg");
	deathScreen.SetSize((float)Width, (float)Height);
	deathScreen.SetPosition((float)Width / 2, (float)Height / 2);

	//mouse Pointer 
	mousePointer.LoadImage("mousePointer.png");
	mousePointer.SetSize(20, 20);

	//*** AIPlayers
	AIPlayerModelOne = new CModelMd3();
	AIPlayerModelTwo = new CModelMd3(); // Friend Model
	boss = new CModelMd3();

	AIPlayerModelOne->LoadModel("enemies/lacky.md3");
	AIPlayerModelOne->LoadTexture("enemies/lacky.png");
	AIPlayerModelOne->SetScale(1.2f);

	AIPlayerModelTwo->LoadModel("enemies/lacky.md3");
	AIPlayerModelTwo->LoadTexture("enemies/lackyFriend.png");
	AIPlayerModelTwo->SetScale(1.2f);

	boss->LoadModel("enemies/Bully.md3");
	boss->SetScale(15.2f);
	boss->LoadTexture("enemies/Bully.png");
}


//FOG
void CMyGame::EnableFog()
{
	GLuint filter;                      // Which Filter To Use
	GLuint fogMode[] = { GL_EXP, GL_EXP2, GL_LINEAR };   // Storage For Three Types Of Fog
	GLuint fogfilter = 1;                    // Which Fog To Use
	GLfloat fogColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };      // Fog Color

	//fog settings
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);          // We'll Clear To The Color Of The Fog ( Modified )
	glFogi(GL_FOG_MODE, fogMode[fogfilter]);        // Fog Mode
	glFogfv(GL_FOG_COLOR, fogColor);            // Set Fog Color
	glFogf(GL_FOG_DENSITY, 0.03f);              // How Dense Will The Fog Be
	glHint(GL_FOG_HINT, GL_DONT_CARE);          // Fog Hint Value
	glFogf(GL_FOG_START, 0.1f);             // Fog Start Depth
	glFogf(GL_FOG_END, 3.0f);               // Fog End Depth
	glEnable(GL_FOG);                   // Enables GL_FOG
}


//AIPLAYERS SPAWN
void CMyGame::AIplayersSpawn(int level)
{
	enum allEnemiesTypes { AIEnemy, AIFriend, BOSS };

	//LEVEL 1
	if (level == 1)
	{
		//Enemy Teams
		for (int enemyIndex = 0; enemyIndex < 3; enemyIndex++)
		{
			AllAIPlayerList.push_back(new AIPlayer());
			AllAIPlayerList.back()->init(enemyIndex, AIEnemy, *map, *AIPlayerModelOne, curentGameLevel);
		}
		
		//Friend Team
		AllAIPlayerList.push_back(new AIPlayer());
		AllAIPlayerList.back()->init(0, AIFriend, *map, *AIPlayerModelTwo, curentGameLevel);
	}

	//LEVEL 2
	if ( level == 2)
	{
		//Enemy Teams 2 regular enemies + boss
		for (int enemyIndex = 0; enemyIndex < 2; enemyIndex++)
		{
			AllAIPlayerList.push_back(new AIPlayer());
			AllAIPlayerList.back()->init(enemyIndex, AIEnemy, *map, *AIPlayerModelOne, curentGameLevel);
		}
		//boss
		AllAIPlayerList.push_back(new AIPlayer());
		AllAIPlayerList.back()->init(2, BOSS, *map, *boss, curentGameLevel);
	 
		//Friend Team
		AllAIPlayerList.push_back(new AIPlayer());
		AllAIPlayerList.back()->init(0, AIFriend, *map, *AIPlayerModelTwo, curentGameLevel);
	}

	//LEVEL 3
	if (level == 3)
	{
		//Enemy Teams 2 regular enemies + boss
		AllAIPlayerList.push_back(new AIPlayer());
		AllAIPlayerList.back()->init(0, AIEnemy, *map, *AIPlayerModelTwo, curentGameLevel);

		//boss
		AllAIPlayerList.push_back(new AIPlayer());
		AllAIPlayerList.back()->init(1, BOSS, *map, *boss, curentGameLevel);
	}
}

//*************** KEYBOARD EVENTS ***************
void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	//Player KeyBoard Controller
	if (!player->playerPreDeahAnimation && currentMenuState == IN_GAME) player->OnKeyDown(sym, currentMousePosToScreen);

	//Start
	MainMenuController(sym);
}

//*************** Main Menu DRAW
void CMyGame::MaiMenuDraw(CGraphics* g)
{
	if (currentMenuState == SHOW_CONTROLLERS)
	{
		mainMenushowControlers.Draw(g);
		font.DrawText((float)Width / 2 - 100, (float)Height / 2 + 280, "CONTROLS", CColor::Black(), 52);
		font.DrawText((float)Width / 2 - 220.f, (float)Height / 2 + 200, "LEFT MOUSE BTN. - SHOOT", CColor::Black(), 22);
		font.DrawText((float)Width / 2 - 220.f, (float)Height / 2 + 150, "W / S / A / D  - MOVE CHARACTER", CColor::Black(), 22);
		font.DrawText((float)Width / 2 - 220.f, (float)Height / 2 + 100, "SPACE - USE SKILL", CColor::Black(), 22);
		font.DrawText((float)Width / 2 - 220.f, (float)Height / 2, "E - INTERACT", CColor::Black(), 22);
		font.DrawText((float)Width / 2 - 70.f, 100, "BACK", CColor::Black(), 42);
	}
	else
	{
		startScreen.Draw(g);
		font.DrawText((float)Width / 2 - 80.f, (float)Height / 2 + 100, "NEW GAME", mainMenuOptionSelection == NEW_GAME ? CColor::White() : CColor::Gray(), 45);
		font.DrawText((float)Width / 2 - 80.f, (float)Height / 2 - 30, "CONTROLS", mainMenuOptionSelection == CONTROLS ? CColor::White() : CColor::Gray(), 45);
		font.DrawText((float)Width / 2 - 40.f, (float)Height / 2 - 160, "EXIT", mainMenuOptionSelection == EXIT ? CColor::White() : CColor::Gray(), 45);
	}
}


//MAIN MENU HANDLER
void CMyGame::MainMenuController(SDLKey sym)
{
	//On ESC btn. pressed OPEN MAIN MENU - IF GAME STARTED
	if (!IsPaused() && gameStarted && currentMenuState == IN_GAME)
	{
		if (sym == SDLK_ESCAPE)
		{
			player->footsteps.Pause();// bug with a speed
			SetGameMode(MODE_PAUSED);
			currentMenuState = MAIN_MENU;
		}
	}

	//SKIP CUTSCENE
	else if (sym == SDLK_ESCAPE && gameStarted && currentMenuState == CUTSCENE)
		cutscene->cutsceneTimer = -1; //skip dialog

	//RETURN TO THE GAME IF GAME STARTED
	else if (IsPaused() && sym == SDLK_ESCAPE  && gameStarted && currentMenuState == MAIN_MENU)
	{
		HideMouse();
		SetGameMode(MODE_RUNNING);
		currentMenuState = IN_GAME;
	}

	//LOADING COMPLETED AND ENTER PRESSED
	if (sym == 13 && currentMenuState == LOADING_SCREEN && loadingScreen->loadingCompleted)
	{
		StartGame();
		gameStarted = true;
		cutscene->startCutscene(curentGameLevel); 
		currentMenuState = CUTSCENE;
	}


	////******* IF NOT IN MENU MODE -> RETURN~~~~~~~~~~~~~~~~~~~~~~
	if (!IsPaused()) return;

	//******* IF SHOW_CONTROLLERS
	if (currentMenuState == SHOW_CONTROLLERS)
	{
		if (sym == 13 || sym == SDLK_ESCAPE) currentMenuState = MAIN_MENU;
	}

	//*******  MAIN_MENU HANDLER
	else if (currentMenuState == MAIN_MENU )
	{
		//Start NEW GAME
		if (sym == 13 && mainMenuOptionSelection == NEW_GAME)
		{
			curentGameLevel = 1; // 1st level allways
			OnStartLevel(curentGameLevel); // 1st level allways
			SetGameMode(MODE_MENU); // to update loading sprite
			loadingScreen->init();
			loadingScreen->loadingStarted = true;
			currentMenuState = LOADING_SCREEN;
		}

		//SHOW CONTROLERS
		if (sym == 13 && mainMenuOptionSelection == CONTROLS) currentMenuState = SHOW_CONTROLLERS;
	 
		//EXIT
		if (sym == 13 && mainMenuOptionSelection == EXIT) StopGame();

		//Menu Scroller
		if ((sym == SDLK_s) || (sym == SDLK_DOWN))
		{
			mainMenuOptionSelection++;
			if (mainMenuOptionSelection > 2)  mainMenuOptionSelection = 0;
		}
		else if ((sym == SDLK_w) || (sym == SDLK_UP))
		{
			mainMenuOptionSelection--;
			if (mainMenuOptionSelection < 0)  mainMenuOptionSelection = 2;
		}
	}
}

//MOUSE EVENTS
void CMyGame::OnMouseMove(Uint16 x, Uint16 y, Sint16 relx, Sint16 rely, bool bLeft, bool bRight, bool bMiddle)
{
	//Prevents player from shooting behind the model
	 if (y < localH / 2 + 100)
	{
		mousePointer.SetPosition(x, localH / 2 + 120);
		currentMousePos = CVector{ (float)x, 0 ,(float)localH / 2 + 120 };
	}
	else if (y > localH - 140)
	{
		mousePointer.SetPosition(x, localH - 150);
		currentMousePos = CVector{ (float)x, 0 ,(float)localH - 150 };
	}
	else
	{
		mousePointer.SetPosition(x, y);
		currentMousePos = CVector{ (float)x, 0 ,(float)y };
	} 

	/*
	float sensitivity = 0.07f;

	static float avgrelx = 0;
	static float avgrely = 0;

	if (currentMenuState != IN_GAME) return;
	// weighted everage between past and current values
	avgrelx = 0.75 * avgrelx + 0.25 * (float)relx;
	avgrely = 0.75 * avgrely + 0.25 * (float)rely;

 
 
	player->playerModel.Rotate(-sensitivity * avgrelx); player->playerModel.SetDirection(player->playerModel.GetRotation());
 
	camera.rotation.y += sensitivity * avgrelx;

	// tilting/pitching of the game world
	camera.rotation.x += -sensitivity * avgrely;
	if (camera.rotation.x < 0) camera.rotation.x = 0;
	if (camera.rotation.x > 80) camera.rotation.x = 80;

	if (camera.rotation.y > 360 || camera.rotation.y < -360) camera.rotation.y = 0;

	*/
}

void CMyGame::OnRButtonDown(Uint16 x, Uint16 y)
{
	if(!player->playerPreDeahAnimation && currentMenuState == IN_GAME) player->OnRButtonDown(GetTime());
}

void CMyGame::OnRButtonUp(Uint16 x, Uint16 y)
{
	if (!player->playerPreDeahAnimation && currentMenuState == IN_GAME) player->OnRButtonUp();
}