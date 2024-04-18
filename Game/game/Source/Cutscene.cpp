#include "../headers/MyGame.h"
#include "../headers/Cutscene.h"
#include "../headers/UIDialogBox.h"
#include "../headers/Player/Player.h"
#include "../headers/AIPlayer.h"

Cutscene::Cutscene(float w, float h)
{
	//Cutscene Edges
	screenEdges.LoadImage("screenEdges.png");
	screenEdges.SetSize(w, h);
	screenEdges.SetPosition(w / 2, h / 2);

	//transition
	darkTransition.LoadImage("darkTransition.png");
	darkTransition.SetSize(w, h);
	darkTransition.SetPosition(w / 2, h / 2);

	victoryOverlay.LoadImage("victoryOverlay.png");
	victoryOverlay.SetSize(w, h - 150);
	victoryOverlay.SetPosition(w / 2, h / 2);
}

void Cutscene::init(float w, float h, Player& player)
{
	//local var 
	localPlayer = &player;
	
	//resets
	cameraOffset = 0;
	cutsceneTimer = 0;
	blackScreenTimer = 0;
	camerRotationAngle = 0;
	curentCutSceneNum = -1;
	cutSceneEndDimOn = false;
	onCutsceneStart = true;
	cutcceneCameraPosition = CVector(0, 0, 0);
}

//*** UPDATE
void Cutscene::Update(Uint32 t, std::vector<AIPlayer*>& AllAIPlayerList)
{
	//local Var
	localAllAIPlayers = AllAIPlayerList;
	localTime = t;

	//Cutscene Selection
	if (curentCutSceneNum == 1) cutSceneOne();
	else if (curentCutSceneNum == 2) cutSceneTwo();
	else if (curentCutSceneNum == 3) cutSceneThree();
	else if (curentCutSceneNum == 4) cutSceneFour();
}

//*** DRAW
void Cutscene::Draw2d(CGraphics* g)
{
	if (curentCutSceneNum == 4) victoryOverlay.Draw(g);
	screenEdges.Draw(g);
	if (cutSceneEndDimOn) darkTransition.Draw(g);
}

//CUTSCENE ONE
void Cutscene::cutSceneOne()
{
	if (onCutsceneStart)
	{
		// Cutscene Timer
		if (cutsceneTimer == 0) cutsceneTimer = localTime + 9000; 

		//camer Settings
		cutcceneCameraPosition = CVector(400, 1340, 1300);
		xWorldRotation = 40;
		onCutsceneStart = false;
	}
	//camera Rotation
	camerRotationAngle -= 0.25;
 
	//Finish Cutscene if cutsceneTimer < time
	cutsceneFinisher();
}

//CUTSCENE TWO
void Cutscene::cutSceneTwo()
{
	if (onCutsceneStart)
	{
		// Cutscene Timer
		if (cutsceneTimer == 0) cutsceneTimer = localTime + 15000; 

		//camer Settings
		camerRotationAngle = -50;
		xWorldRotation = 20;
		cameraOffset = 2580;

		//FIND BOSS
		for (auto AIPlayer : localAllAIPlayers)
		{
			if (AIPlayer->localEnemyType == 2) // BOSS
			{
				cutcceneCameraPosition = AIPlayer->AIPlayerModel->GetPositionV(); //camer Settings
				AIPlayer->showBox(3, 4, 6, 7500);
			}
		}
		onCutsceneStart = false;
	}

	//Finish Cutscene if cutsceneTimer < time
	cutsceneFinisher();
}

//CUTSCENE THREE
void Cutscene::cutSceneThree()
{
	if (onCutsceneStart)
	{
		// Cutscene Timer
		cutsceneTimer = localTime + 15000; 

		//camera Settings
		camerRotationAngle = -50;
		xWorldRotation = 20;
		cameraOffset = 2080;
		
		//Find Friend-traitor
		for (auto AIPlayer : localAllAIPlayers)
		{
			if (AIPlayer->localEnemyType == 0) // Friend traitor
			{
	
				cutcceneCameraPosition = AIPlayer->AIPlayerModel->GetPositionV(); //camera Settings
				AIPlayer->showBox(5, 11, 13, 7500);	//Show Dialog Box
			}
		}
		onCutsceneStart = false;
	}

	//Finish Cutscene if cutsceneTimer < time
	cutsceneFinisher();
}

//CUTSCENE FOUR FINAL
void Cutscene::cutSceneFour()
{
	if (onCutsceneStart)
	{
		// Cutscene Timer
		if (cutsceneTimer == 0) cutsceneTimer = localTime + 12000; 

		//camera Settings
		camerRotationAngle = -60;
		xWorldRotation = 20;
		cameraOffset = 3580;
		cutcceneCameraPosition = localPlayer->playerModel.GetPositionV();

		//player Animation
		localPlayer->playerModel.PlayAnimation("victory", 12, true);

		//Show Dialog Box
		localPlayer->showBox(4, 16, 17, 7500);
		onCutsceneStart = false;
	}

	//Finish Cutscene if cutsceneTimer < time
	cutsceneFinisher(); 
}


//START A CUTSCENE
void Cutscene::startCutscene(int sceneNum)
{
	curentCutSceneNum = sceneNum;
	isCutscenePlaying = true;
	cutSceneEndDimOn = false;
	blackScreenTimer = 0;
}

//FINISH CUTSCENE
void Cutscene::cutsceneFinisher()
{
	if (cutsceneTimer < localTime)
	{
		for (auto AIPlayer : localAllAIPlayers)  AIPlayer->hideBox();
		if (!cutSceneEndDimOn) cutSceneEndDimOn = true;
		blackScreenTimer += 2;
		darkTransition.SetAlpha(blackScreenTimer);
	}

	if (blackScreenTimer >= 100) isCutscenePlaying = false;
}