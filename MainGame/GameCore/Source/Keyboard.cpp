//////////////////////////////////////////////////////////////////////////////////
///             @file   Keyboard.cpp
///             @brief  Keyboard
///             @author Toide Yutaro
///             @date   2020_11_28
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Keyboard.hpp"
#include "GameCore/Include/KeyCode.hpp"
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//							Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region  KeyEvent
KeyboardEvent::KeyboardEvent()
{
	_keyEvent = KeyEvent::Press;
	_key      = 0u;
}

KeyboardEvent::KeyboardEvent(const KeyEvent type, const unsigned char key)
{
	_keyEvent = type;
	_key      = key;
}

bool KeyboardEvent::IsPress() const 
{
	return this->_keyEvent == KeyEvent::Press;
}

bool KeyboardEvent::IsRelease() const 
{
	return this->_keyEvent == KeyEvent::Release;
}

bool KeyboardEvent::IsValid() const
{
	return this->_keyEvent != KeyEvent::Invalid;
}

unsigned char KeyboardEvent::GetKeyCode() const
{
	return this->_key;
}

#pragma endregion KeyEvent

#pragma region Keyboard
Keyboard::Keyboard()
{
	for (int i = 0; i < 256; ++i)
	{
		this->_keyStates[i] = false;
	}
}

bool Keyboard::KeyIsPressed(const unsigned char keycode)
{
	return this->_keyStates[keycode];
}

bool Keyboard::GetDurationPressed(const unsigned char keycode)
{
	return false;
}

bool Keyboard::KeyBufferIsEmpty()
{
	return this->_keyBuffer.empty();
}

bool Keyboard::CharBufferIsEmpty()
{
	return this->_charBuffer.empty();
}

KeyboardEvent Keyboard::ReadKey()
{
	if (this->_keyBuffer.empty()) 
	{
		return KeyboardEvent();
	}
	else
	{
		KeyboardEvent e = this->_keyBuffer.front(); 
		this->_keyBuffer.pop(); 
		return e; 
	}
}
unsigned char Keyboard::ReadChar()
{
	if (this->_charBuffer.empty())
	{
		return 0u;
	}
	else
	{
		unsigned char e = this->_charBuffer.front(); 
		this->_charBuffer.pop();
		return e;
	}
}

void Keyboard::OnKeyPressed(const unsigned char key)
{
	this->_keyStates[key] = true;
	this->_keyBuffer.push(KeyboardEvent(KeyboardEvent::KeyEvent::Press, key));
}

void Keyboard::OnKeyReleased(const unsigned char key)
{
	this->_keyStates[key] = false;
	this->_keyBuffer.push(KeyboardEvent(KeyboardEvent::KeyEvent::Release, key));
}

void Keyboard::OnChar(const unsigned char key)
{
	this->_charBuffer.push(key);
}

void Keyboard::EnablePerFrameAutoRepeatKeys()
{
	this->_autoRepeatKeys = true;
}

void Keyboard::DisablePerFrameAutoRepeatKeys()
{
	this->_autoRepeatKeys = false;
}

void Keyboard::EnablePerFrameAutoRepeatChars()
{
	this->_autoRepeatChars = true;
}

void Keyboard::DisablePerFrameAutoRepeatChars()
{
	this->_autoRepeatChars = false;
}

bool Keyboard::IsKeysPerFrameAutoRepeat()
{
	return this->_autoRepeatKeys;
}

bool Keyboard::IsCharsPerFrameAutoRepeat()
{
	return this->_autoRepeatChars;
}
#pragma endregion Keyboard