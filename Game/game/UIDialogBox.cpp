#include "Game.h"
#include "headers/UIDialogBox.h"
 
 


UIDialogBox::UIDialogBox()
{ 
	font.LoadDefault();
	dialogBoxBg.LoadImage("dialogBox3.png");
	dialogBoxBg.SetSize(150, 100);
 
	isBoxShowen = false;
	hideInSec = 0;
	//DIALOGS
	name[MYNE] = "Myne";
	name[BLINKY] = "Blinky";
	//cutscene 1
	text[0] = "How much longer\n, Blinky? This is\n taking forever. ";
	text[1] = "expl";
	text[2] = "What Was that?????";
	text[3] = "Myne, somethings-\n hap- can’t- repon-\n shutting- dow-";
	text[4] = "Blinky?\n Hey!\n Blinky!";
	text[5] = "The ship control is \nnon-responsive, \nwhat the hell? What is going on? ";
	text[6] = "Myne- They’re- \nNot- Help-";
}

void UIDialogBox::OnUpdate(long t, CVector position)
{
	localTime = t;
	if (hideInSec < localTime)
	{
		if (dialogNumber < dialogEndNumber)
		{
			dialogNumber++; // next text
			hideInSec =  hideInSec + 4000; // hide in sec == time
		}
		else hideBox();
		
	}

	dialogBoxBg.SetPosition(position.GetX(), position.GetY() + 140);
	
}

void UIDialogBox::OnDraw(CGraphics* g)
{
	if (isBoxShowen )
	{
		dialogBoxBg.Draw(g);
		//text
	
		TextConverter::splitTextToLines(text[dialogNumber], dialogBoxBg.GetX() - 80, dialogBoxBg.GetY() + 20, 20);
	}
}

void UIDialogBox::showBox(int speakerId, int textIdStart, int textIdEnd,  float autohideBoxin)
{
 
	hideInSec = localTime + autohideBoxin;
	isBoxShowen = true;
	speaker = speakerId;
	dialogNumber = textIdStart;
	dialogEndNumber = textIdEnd;
}

void UIDialogBox::hideBox()
{
	isBoxShowen = false;
}
 