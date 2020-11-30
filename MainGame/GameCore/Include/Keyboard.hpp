//////////////////////////////////////////////////////////////////////////////////
///             @file   Keyboard.hpp
///             @brief  Keyboard
///             @author Toide Yutaro
///             @date   2020_11_
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include <queue>

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                         Keyboard, keyboard event class
//////////////////////////////////////////////////////////////////////////////////

/****************************************************************************
*				  			Keyboard Event
*************************************************************************//**
*  @class     Keyboard
*  @brief     Keyboard Event Trigger
*****************************************************************************/
class KeyboardEvent
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool IsPress()   const;
	bool IsRelease() const;
	bool IsValid()   const;
	unsigned char GetKeyCode() const;
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	enum class KeyEvent
	{
		Press   = 0,
		Release = 1,
		Invalid = 2
	};
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	KeyboardEvent();
	KeyboardEvent(const KeyEvent type, const unsigned char key);
	KeyboardEvent(const KeyboardEvent&)            = default;
	KeyboardEvent& operator=(const KeyboardEvent&) = default;
	KeyboardEvent(KeyboardEvent&&)                 = default;
	KeyboardEvent& operator=(KeyboardEvent&&)      = default;

	~KeyboardEvent() {};
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/
	KeyEvent _keyEvent;
	unsigned char _key;
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
};


#pragma region Keyboard
/****************************************************************************
*				  			TemplateClass
*************************************************************************//**
*  @class     Keyboard
*  @brief     Keyboard Input 
*  @note      In the future, I want to introduce the virtual keyboard 
*****************************************************************************/
class Keyboard
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool KeyIsPressed(const unsigned char keycode);
	bool GetDurationPressed(const unsigned char keycode);
	bool KeyBufferIsEmpty ();
	bool CharBufferIsEmpty();
	KeyboardEvent ReadKey ();
	unsigned char ReadChar();
	void OnKeyPressed (const unsigned char key);
	void OnKeyReleased(const unsigned char key);
	void OnChar(const unsigned char key);
	void EnablePerFrameAutoRepeatKeys  ();
	void DisablePerFrameAutoRepeatKeys ();
	void EnablePerFrameAutoRepeatChars ();
	void DisablePerFrameAutoRepeatChars();
	bool IsKeysPerFrameAutoRepeat ();
	bool IsCharsPerFrameAutoRepeat();
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	Keyboard();
	Keyboard(const Keyboard& )           = default;
	Keyboard& operator=(const Keyboard&) = default;
	Keyboard(Keyboard&&)                 = default;
	Keyboard& operator=(Keyboard&&)      = default;

	~Keyboard() {};
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	bool _autoRepeatKeys  = false;
	bool _autoRepeatChars = false;
	bool _keyStates[256];
	std::queue<KeyboardEvent> _keyBuffer;
	std::queue<unsigned char> _charBuffer;

};

#endif