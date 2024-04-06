#pragma once
 
class TextConverter;

class LoadingScreen
{
public:
	LoadingScreen(float w, float h);
	std::string text[15];

	//main Functions
	void init();
	void Update(Uint32 t);
	void OnDraw(CGraphics* g);

	bool loadingCompleted, loadingStarted;

private:
	CSprite loadingScreen;
	CSprite loadinBarBg;
	CFont font;
	TextConverter* textConverter;
	CHealthBar loadingBar;

	float loadingProgress;
	float localH, localW;
	int randomTextSelectionNum;
	float loadingTimer;
};