#pragma once

class TextConverter
{
public:
	TextConverter();
	void splitTextToLines(string text, float xpos, float ypos, int fonSize);

private:
	CFont font;
};