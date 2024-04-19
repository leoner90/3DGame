#include "Game.h"
#include "../headers/UIDialogBox.h"
 
UIDialogBox::UIDialogBox()
{ 
	font.LoadDefault();

	//BG
	dialogBoxBg.LoadImage("dialogBox3.png");
	dialogBoxBg.SetSize(490, 345);
 
	//variables
	isBoxShowen = false;
	hideInSec = 0;

	//SPEAKERS
	name[0] = "Peter"; // regular enemy
	name[1] = "Victor"; // regular enemy
	name[2] = "Bob"; // regular enemy
	name[3] = "Shinma (Boss)";// BOSS
	name[4] = "Leo"; // Player
	name[5] = "Light (Friend)"; // Friend
 

	//****** TEXTS
	//levele 1 start dialogues
	text[0] = "Are you ready \nto eat snow. \nDon't cry later.";
	text[1] = "Let's make sure \nthey regret \nchallenging us!";
	text[2] = "Crush them with \nour icy might! ";
	text[3] = "We Will Win \nWe are the best! \nYes My Friend?"; //friend

	//levele 2 Cutscene 2 BOSS
	text[4] = "Enough of these \nlittle distractions.";
	text[5] = "\nAre you the king ? ";
	text[6] = "\nI will crush you. ";

	//levele 2 start dialogues
	text[7] = "It's time to \nturn the snow into \nour weapon ";
	text[8] = "Let's dominate the \nsnow battlefield! ";
	text[9] = "Listen up, team! \nNo mercy in this \nsnow showdown!"; // boss
	text[10] = "Prepare for defeat \nonce more \nmark my words."; //friend

	//levele 3 Cutscene 3 Friend  Betrayal
	text[11] = "I don’t need \nyou anymore, \nyou naive fool";
	text[12] = "I have been hiding \nmy power for \nfar too long.";
	text[13] = "Witness!\nAs WE rule \nthis playground!!”.";

	//levele 3 start dialogues
	text[14] = "Let's chill him \nout, my new friend, \nFor the victory! ";
	text[15] = "Prepare to witness \nas we crush you \nand secure the win!";

	//Cutscene 4 VICTORY
	text[16] = "I did it! \nVictory is mine";
	text[17] = "Who is now tasting \nthe bitterness \nof defeat, huh?";

}

void UIDialogBox::OnUpdate(long t, CVector position)
{
	localTime = t;
	//Hide if hideInSec < time
	if (hideInSec < localTime)
	{
		//next Text ID if exists && timer reset 
		if (dialogNumber < dialogEndNumber)
		{
			dialogNumber++; // next text
			hideInSec =  hideInSec + 4000; // timer reset 
		}
		else hideBox();
	}

	//Update Box Pos In The World
	if(isBoxShowen) dialogBoxBg.SetPosition(position.GetX() + 60, position.GetY() + 60);
}

void UIDialogBox::OnDraw(CGraphics* g)
{
	if (isBoxShowen)
	{	
		//BG
		dialogBoxBg.Draw(g);
		//Speaker Draw
		font.DrawText(dialogBoxBg.GetX() - 80, dialogBoxBg.GetY() + 35 , name[speaker], speaker == 5 ? CColor::DarkGreen() :CColor::DarkRed(), 18);
		//Text Draw
		TextConverter::splitTextToLines(text[dialogNumber], dialogBoxBg.GetX() - 80, dialogBoxBg.GetY() + 15, 18);
	}
}

//*** SHOW BOX
void UIDialogBox::showBox(int speakerId, int textIdStart, int textIdEnd,  float autohideBoxin)
{
	hideInSec = localTime + autohideBoxin;
	isBoxShowen = true;
	speaker = speakerId;
	dialogNumber = textIdStart;
	dialogEndNumber = textIdEnd;
}

//*** HIDE BOX
void UIDialogBox::hideBox()
{
	isBoxShowen = false;
}