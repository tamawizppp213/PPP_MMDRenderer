//////////////////////////////////////////////////////////////////////////////////
//              Title:  Screen.hpp
//            Content:  Screen Config
//             Author:  Toide Yutaro
//             Create:  2020_11_09
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Screen.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                  Window Class (width, height static)
//                  Default Size  width: 1920 Å~ height: 1080
//////////////////////////////////////////////////////////////////////////////////
int Screen::_width  = DEFAULT_SCREEN_WIDTH;
int Screen::_height = DEFAULT_SCREEN_HEIGHT;

Screen::Screen()
{
	_width  = DEFAULT_SCREEN_WIDTH;
	_height = DEFAULT_SCREEN_HEIGHT;
}

Screen::Screen(int width, int height)
{
	_width  = width;
	_height = height;
}

Screen::~Screen()
{

}

Screen& Screen::GetScreen()
{
	Screen screen = Screen(_width, _height);
	return screen;
}

int Screen::GetScreenWidth()
{
	return _width;
}

int Screen::GetScreenHeight()
{
	return _height;
}

void Screen::SetScreenWidth(int width)
{
	_width = width;
}

void Screen::SetScreenHeight(int height)
{
	_height = height;
}

float Screen::AspectRatio() const
{
	static_cast<float>(_width / _height);
}