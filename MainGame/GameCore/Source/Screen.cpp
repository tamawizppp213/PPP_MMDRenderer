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
#include <assert.h>

//////////////////////////////////////////////////////////////////////////////////
//                  Window Class (width, height static)
//                  Default Size  width: 1920 ~ height: 1080
//////////////////////////////////////////////////////////////////////////////////
int Screen::_width  = DEFAULT_SCREEN_WIDTH;
int Screen::_height = DEFAULT_SCREEN_HEIGHT;
float Screen::_pixelWidth  = 1.0f / DEFAULT_SCREEN_WIDTH;
float Screen::_pixelHeight = 1.0f / DEFAULT_SCREEN_HEIGHT;
Screen::Screen()
{
	//_width  = DEFAULT_SCREEN_WIDTH;
	//_height = DEFAULT_SCREEN_HEIGHT;
}

Screen::Screen(int width, int height)
{
	_width  = width;
	_height = height;
	_pixelWidth  = 1.0f / width;
	_pixelHeight = 1.0f / height;
}

Screen::~Screen()
{
	
}

Screen& Screen::GetScreen()
{
	return *this;
}

int Screen::GetScreenWidth()
{
	return _width;
}

int Screen::GetScreenHeight()
{
	return _height;
}

float Screen::GetOnePixelWidth()
{
	return _pixelWidth;
}
float Screen::GetOnePixelHeight()
{
	return _pixelHeight;
}
void Screen::SetScreenWidth(int width)
{
	_width = width;
	_pixelWidth = width >= 1.0f ? 1.0f / width : 0.0f;
}

void Screen::SetScreenHeight(int height)
{
	_height = height;
	_pixelHeight = height >= 1.0f ? 1.0f / height : 0.0f;
}

float Screen::AspectRatio() const
{
	return (float)_width / (float)_height;
}