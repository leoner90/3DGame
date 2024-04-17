#include "headers/MyGame.h"
#include "headers/Cutscene.h"
#include "headers/UIDialogBox.h"
#include "headers/Player/Player.h"
#include "headers/Enemy.h"

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
	cameraOffset = 0;
	cutsceneTimer = 0;
	//resets
	dialogSwitcherTimer = 0;
	dialogNumber = 0;
	blackScreenTimer = 0;
	camerRotationAngle = 0;
	curentCutSceneNum = -1;
	cutSceneEndDimOn = false;
	delay = 0;
	onCutsceneStart = true;
	localPlayer = &player;
 

	//camera
	cutcceneCameraPosition = CVector(0, 0, 0);


}

void Cutscene::Update(Uint32 t, std::vector<Enemy*>& AIPlayers)
{
	localAllAIPlayers = AIPlayers;
	localTime = t;

	if (curentCutSceneNum == 1) cutSceneOne();
	else if (curentCutSceneNum == 2) cutSceneTwo();
	else if (curentCutSceneNum == 3) cutSceneThree();
	else if (curentCutSceneNum == 4) cutSceneFour();
}

void Cutscene::Draw2d(CGraphics* g)
{
	if (curentCutSceneNum == 4) victoryOverlay.Draw(g);
	screenEdges.Draw(g);

	if (cutSceneEndDimOn) darkTransition.Draw(g);
}

void Cutscene::Draw3d(CGraphics* g)
{
	if (curentCutSceneNum == 1)
	{
		if(blackScreenTimer < 40 )localPlayer->playerModel.Draw(g);
		particleList.Draw(g);
	}
}



void Cutscene::cutSceneOne()
{
	cutcceneCameraPosition = CVector(400, 1340, 1300);
	camerRotationAngle = 0.25;
	xWorldRotation = 40;
 

	if (dialogSwitcherTimer == 0 )
	{
		dialogSwitcherTimer = localTime + 8000;
		dialogNumber = 9;
	}

 
	if (dialogSwitcherTimer < localTime && dialogNumber == 9) 	dialogNumber++;
	if (dialogNumber == 10) dialogNumber++;
	if (dialogNumber == 11 && isCutscenePlaying)
	{
		if (!cutSceneEndDimOn) cutSceneEndDimOn = true;
		blackScreenTimer += 2;
		darkTransition.SetAlpha(blackScreenTimer);
	}


	if (blackScreenTimer >= 100)
	{
		isCutscenePlaying = false;
	}

	localPlayer->playerModel.Update(localTime);
}

void Cutscene::cutSceneTwo()
{
	if (onCutsceneStart)
	{
		if (cutsceneTimer == 0) cutsceneTimer = localTime + 15000;
		camerRotationAngle = -50;
		xWorldRotation = 20;
		cameraOffset = 2580;
		for (auto AIPlayer : localAllAIPlayers)
		{
			if (AIPlayer->localEnemyType == 2) // BOSS
			{
				cutcceneCameraPosition = AIPlayer->enemyModel->GetPositionV();
				AIPlayer->showBox(3, 4, 6, 7500);
			}
		}
		onCutsceneStart = false;
	}

	if (dialogNumber == 11)  	cutsceneTimer = -1; // skip
 
	if (cutsceneTimer < localTime)
	{

		for (auto AIPlayer : localAllAIPlayers)  AIPlayer->hideBox();
		if (!cutSceneEndDimOn) cutSceneEndDimOn = true;
		blackScreenTimer += 2;
		darkTransition.SetAlpha(blackScreenTimer);
	}


	if (blackScreenTimer >= 100)
	{
		isCutscenePlaying = false;
	}
}

void Cutscene::cutSceneThree()
{
	if (onCutsceneStart)
	{
		cutsceneTimer = localTime + 15000;
		camerRotationAngle = -50;
		xWorldRotation = 20;
		cameraOffset = 2080;
		

		for (auto AIPlayer : localAllAIPlayers)
		{
		 
			if (AIPlayer->localEnemyType == 0) // Friend traitor
			{
				cutcceneCameraPosition = AIPlayer->enemyModel->GetPositionV();
				AIPlayer->showBox(5, 11, 13, 7500);
			}
		}

		onCutsceneStart = false;
	}
	
	if (dialogNumber == 11) cutsceneTimer = -1; // skip


	if (cutsceneTimer < localTime)
	{
		for (auto AIPlayer : localAllAIPlayers)  AIPlayer->hideBox();
		if (!cutSceneEndDimOn) cutSceneEndDimOn = true;
		blackScreenTimer += 2;
		darkTransition.SetAlpha(blackScreenTimer);
	}

	if (blackScreenTimer >= 100) isCutscenePlaying = false;

}

void Cutscene::cutSceneFour()
{
	if (onCutsceneStart)
	{
		enum playerStates { UNOCCUPIED, INATTACK, INDASH, INDAMAGE, CUTSCENE };
		localPlayer->playerCurrentState = localPlayer->CUTSCENE;
		if (cutsceneTimer == 0) cutsceneTimer = localTime + 12000;
		camerRotationAngle = -60;
		xWorldRotation = 20;
		cameraOffset = 3580;


		cutcceneCameraPosition = localPlayer->playerModel.GetPositionV();
		localPlayer->playerModel.PlayAnimation("victory", 12, true);
		localPlayer->showBox(4, 16, 17, 7500);
		onCutsceneStart = false;

	}
	if (dialogNumber == 11) cutsceneTimer = -1; // skip
 

	if (cutsceneTimer < localTime)
	{
		for (auto AIPlayer : localAllAIPlayers)  AIPlayer->hideBox();
		if (!cutSceneEndDimOn) cutSceneEndDimOn = true;
		blackScreenTimer += 2;
		darkTransition.SetAlpha(blackScreenTimer);
	}

	if (blackScreenTimer >= 100) isCutscenePlaying = false;
 
}



void Cutscene::startCutscene(int sceneNum)
{
	curentCutSceneNum = sceneNum;
	isCutscenePlaying = true;
	cutSceneEndDimOn = false;
	blackScreenTimer = 0;
}


