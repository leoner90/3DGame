#pragma once
class Player;
class UIDialogBox;
class AIPlayer;

class Cutscene
{
public:
	Cutscene(float w, float h);
	
	//Main Functions
	void init(float w, float h, Player& player);
	void Update(Uint32 t, std::vector<AIPlayer*>& AIPlayers);
	void Draw2d(CGraphics* g);

	//Cutscenes
	void cutSceneOne();
	void cutSceneTwo();
	void cutSceneThree();
	void cutSceneFour();

	//CutScene Starter
	void startCutscene(int sceneNum);

	//CutScene Finisher
	void cutsceneFinisher();


	//TODO GETTERS SETTERS

	//camera
	float camerRotationAngle;
	CVector cutcceneCameraPosition;
	float cameraOffset, xWorldRotation;

	//cutscene settings
	int curentCutSceneNum;
	bool isCutscenePlaying = true;

	//how long to play cutscene
	float cutsceneTimer; // if -1 skip!

private:
	//Local Var
	std::vector<AIPlayer*> localAllAIPlayers;
	UIDialogBox* localDialogBox;
	Player* localPlayer;
	float localTime;

	//Cutscene Effect
	CSprite screenEdges, darkTransition;
	bool cutSceneEndDimOn;
	int blackScreenTimer;

	//CAMERA Settings
	CVector initRotation, initPos, initVelocity;
	
	//On Start Of Cutcene
	bool onCutsceneStart;



	//Cutscene 4 victory Sprite Overlay
	CSprite victoryOverlay;
};