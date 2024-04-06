#pragma once
#include "headers/textConverter.h"

class UIDialogBox :public TextConverter
{
public:
	UIDialogBox();

	//Main Functions
	void OnUpdate(long t, CVector position);
	void OnDraw(CGraphics* g);

	//Show Hide Dialog Box
	void showBox(int speakerId, int textIdStart, int textIdEnd,  float autohideBoxin  );
	void hideBox();

	bool isBoxShowen;


private:
	//settings
	int speaker;
	float hideInSec;
	CSprite dialogBoxBg;

	//Set current Speacker
	enum currentSpeaker { MYNE, BLINKY };

	float localW, localH;

	int dialogNumber, dialogEndNumber;
	std::string text[27], name[2];
	
	CFont font;

	vector2f initPosDialogBox;

	float localTime;

	TextConverter* textConverter;	 

	CVector dialogBoxPos;
};