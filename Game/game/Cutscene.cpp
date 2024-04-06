#include "headers/MyGame.h"
#include "headers/Cutscene.h"
#include "headers/UIDialogBox.h"
#include "headers/Player/Player.h"

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

 
}

void Cutscene::init(float w, float h, Player& player)
{
 

	//resets
	dialogSwitcherTimer = 0;
	dialogNumber = 0;
	blackScreenTimer = 0;
	camerRotationAngle = 0;
	curentCutSceneNum = -1;
	cutSceneEndDimOn = false;
	delay = 0;

	//cutscene2
	localPlayer = &player;
 
	localPlayer->playerModel.SetSpeed(0);
	cutsceneTwoStarted = false;
	CutSceneTwoReachedPortal = false;
	particleList.delete_all();

	//camera
	cutcceneCameraPosition = CVector(0, 0, 0);
}

void Cutscene::Update(Uint32 t, UIDialogBox& dialogBox)
{
	localDialogBox = &dialogBox;
	
	localTime = t;
	if (curentCutSceneNum == 0)
	{
 
		cutSceneOne();
		
	}
	
	if (curentCutSceneNum == 1)
	{
		localPlayer->playerModel.Update(localTime);
		particleList.Update(localTime);
		particleList.delete_if(true);
		cutSceneTwo();	
	}

}

void Cutscene::Draw2d(CGraphics* g)
{
	screenEdges.Draw(g);

	if (cutSceneEndDimOn)
	{
		darkTransition.Draw(g);
	}
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
	cutcceneCameraPosition = CVector(800, 1740, 500);
	camerRotationAngle = 0.25;

	if (dialogSwitcherTimer == 0 )
	{
		dialogSwitcherTimer = localTime + 8000;
		dialogNumber = 9;
	}




 
	if (dialogSwitcherTimer < localTime && dialogNumber == 9)
		dialogNumber++;
	




	if (dialogNumber == 10)
	{
		dialogNumber++;
	}


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

 
}


void Cutscene::cutSceneTwo()
{
	if (!cutsceneTwoStarted)
	{
		localPlayer->playerModel.SetPosition(1500, 0, 1500);
	 
		cutsceneTwoStarted = true;
		//localDialogBox->showBox(1, 22, 22, 3, 3800);
		
		localPlayer->playerModel.SetSpeed(300);
		localPlayer->playerModel.PlayAnimation("runF", 22, true);
		localPlayer->playerModel.SetDirectionAndRotationToPoint(400, 300);
	}

	CVector displ =CVector(400, 0, 300) - localPlayer->playerModel.GetPositionV() ;
	float distance = hypot(displ.GetX(), displ.GetZ());

 
 

	if (CutSceneTwoReachedPortal && delay < localTime)
	{
		if (!cutSceneEndDimOn) cutSceneEndDimOn = true;
		blackScreenTimer += 1;
		darkTransition.SetAlpha(blackScreenTimer);
	}

	if (blackScreenTimer >= 100)
	{
		isCutscenePlaying = false;
 
		cutsceneTwoStarted = false;
		CutSceneTwoReachedPortal = false;

		particleList.delete_all();
	}

	cutcceneCameraPosition = CVector(localPlayer->playerModel.GetX() - 300, localPlayer->playerModel.GetY() + 600 , localPlayer->playerModel.GetZ());
 
}



void Cutscene::startCutscene(int sceneNum)
{
	curentCutSceneNum = sceneNum;
	isCutscenePlaying = true;
	cutSceneEndDimOn = false;
	blackScreenTimer = 0;
}


