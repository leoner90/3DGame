#include "Game.h"
#include "headers/textConverter.h"
#include <sstream>
#include <string>
#include <vector>

TextConverter::TextConverter()
{
	font.LoadDefault();
}

void TextConverter::splitTextToLines(string text, float xpos, float ypos, int fonSize)
{
	
	stringstream ss(text);
	string to;

	if (text != "")
	{
		float lineHeight = 0;
		while (getline(ss, to, '\n')) {
			font.DrawText(xpos, ypos - lineHeight, to, CColor::Red(), fonSize);
			lineHeight += fonSize + 2;
		}
	}
}


