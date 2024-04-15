#include "Game.h"
#include "headers/UIDialogBox.h"
 
 


UIDialogBox::UIDialogBox()
{ 
	font.LoadDefault();
	dialogBoxBg.LoadImage("dialogBox3.png");
	dialogBoxBg.SetSize(490, 345);
 
	isBoxShowen = false;
	hideInSec = 0;
	//DIALOGS
	name[0] = "Andreas";
	name[1] = "Victor";
	name[2] = "Jarek";
	name[3] = "Shinma";
	name[4] = "Leo"; // Player
	name[5] = "Light"; // Friend
 

	//cutscene 1
	text[0] = "Are you ready \nto eat snow. \nDon't cry later";
	text[1] = "We Will Win \nWe are the best! \nYes My Friend?";
	text[2] = "What Was that?????";
	text[3] = "Myne, somethings-\n hap- can’t- repon-\n shutting- dow-";
	text[4] = "Blinky?\n Hey!\n Blinky!";
	text[5] = "The ship control is \nnon-responsive, \nwhat the hell? What is going on? ";
	text[6] = "How much longer\n, Blinky? This is\n taking forever. ";
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

	dialogBoxBg.SetPosition(position.GetX() + 60, position.GetY() + 60);
	
}

void UIDialogBox::OnDraw(CGraphics* g)
{
	if (isBoxShowen )
	{
		dialogBoxBg.Draw(g);
		//text
		font.DrawText(dialogBoxBg.GetX() - 80, dialogBoxBg.GetY() + 35 , name[speaker], CColor::DarkRed(), 18);
		TextConverter::splitTextToLines(text[dialogNumber], dialogBoxBg.GetX() - 80, dialogBoxBg.GetY() + 15, 18);
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
 