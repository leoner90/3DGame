#include "Game.h"
#include "headers/loadingScreen.h"
#include "headers/textConverter.h"

LoadingScreen::LoadingScreen(float w, float h)
{
	localH = h;
	localW = w;
	textConverter = new TextConverter();

	font.LoadDefault();
	font.Load("arial.ttf");




	// Main Menu screen
	loadingScreen.LoadImage("loadingScreen.jpg");
	loadingScreen.SetSize(localW, localH);
	loadingScreen.SetPosition(localW / 2, localH / 2);

	//loadinBar
	loadingBar.SetSize(594, 75);
	loadingBar.SetPosition(889, 124);
	loadingBar.SetColors(CColor::Blue(), CColor::None(), CColor::None());

	//loadinBarBg
	loadinBarBg.LoadImage("loadinBarBg.png");
	loadinBarBg.SetSize(645, 115);
	loadinBarBg.SetPosition(900, 117);

}


void LoadingScreen::init()
{
	loadingCompleted = false;
	loadingStarted = false;
	loadingTimer = 0;
	loadingProgress = 0;
	loadingBar.SetHealth(loadingProgress);
}

void LoadingScreen::Update(Uint32 t)
{
	if (loadingTimer == 0 && loadingStarted) loadingTimer = t + 1000;
	if (loadingTimer < t && loadingStarted) loadingCompleted = true;

	loadingProgress = 100 - ( (loadingTimer - t) / 4000 * 100);
	loadingBar.SetHealth(loadingProgress);
	loadingBar.Update(t);
 
}

void LoadingScreen::OnDraw(CGraphics* g)
{
	
	loadingScreen.Draw(g);
	loadingBar.Draw(g);
	loadinBarBg.Draw(g);




 
	if (loadingCompleted)
		font.DrawText(localW / 2 - 200, 40, "Press Enter To Continue", CColor::Black(), 32);
	else
		font.DrawText(localW / 2, 40, "Loading...", CColor::Black(), 32);
	
}
