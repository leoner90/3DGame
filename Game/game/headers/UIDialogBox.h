#pragma once
#include "textConverter.h"

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

	//TO DO GETTERS SETTERS
	bool isBoxShowen;

private:
	//Allows to use new line in text \n
	TextConverter* textConverter;
	
	//Texts and Speakers
	std::string text[18], name[6];

	//fonts
	CFont font;

	//local Variables
	float localW, localH;
	float localTime;

	//Dialog box BG
	CSprite dialogBoxBg;

	//settings
	int speaker;
	float hideInSec;
	int dialogNumber, dialogEndNumber;

	//Dialog Box Position in the World
	CVector dialogBoxPos;
};