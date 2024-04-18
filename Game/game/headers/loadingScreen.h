#pragma once
 
class LoadingScreen
{
public:
	LoadingScreen(float w, float h);
	std::string text[15];

	//main Functions
	void init();
	void Update(Uint32 t);
	void OnDraw(CGraphics* g);

	//TODO GETTERS AND SETTER
	bool loadingCompleted, loadingStarted;

private:
	CFont font;

	//loading screen bg and loading Bar
	CSprite loadingScreen;
	CSprite loadinBarBg;
	CHealthBar loadingBar;

	//variables
	float loadingProgress;
	float localH, localW;
	float loadingTimer;
};