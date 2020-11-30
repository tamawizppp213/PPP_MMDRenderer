//////////////////////////////////////////////////////////////////////////////////
///             @file   TemplateText.hpp
///             @brief  TemplateText
///             @author Toide Yutaro
///             @date   2020_11_
//////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef MOUSE_HPP
#define MOUSE_HPP

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include <queue>
//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                           Mouse, MouseEvent Class
//////////////////////////////////////////////////////////////////////////////////
struct MousePoint
{
	int x;
	int y;
};
/****************************************************************************
*				  			MouseEvent Class
*************************************************************************//**
*  @class     MouseEvent
*  @brief     Mouse
*****************************************************************************/
#pragma region Mouse Event
class MouseEvent
{
public:
	enum class EventType
	{
		LPress,
		LRelease,
		RPress,
		RRelease,
		MPress,
		MRelease,
		WheelUp,
		WheelDown,
		Move,
		RawMove,
		Invalid
	};

	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	bool       IsValid() const;
	EventType  GetType() const;
	MousePoint GetPosition() const;
	int GetPositionX() const;
	int GetPositionY() const;

	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/
	
	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	MouseEvent();
	MouseEvent(const EventType type, int x, int y);
	MouseEvent(const MouseEvent&)            = default;
	MouseEvent& operator=(const MouseEvent&) = default;
	MouseEvent(MouseEvent&&)                 = default;
	MouseEvent& operator=(MouseEvent&&)      = default;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	EventType _type;
	int _x;
	int _y;
};

#pragma endregion Mouse Event
#pragma region Mouse
/****************************************************************************
*				  			MouseEvent Class
*************************************************************************//**
*  @class     MouseEvent
*  @brief     Mouse
*****************************************************************************/
class Mouse
{
public:
	/****************************************************************************
	**                Public Function
	*****************************************************************************/
	void OnLeftPressed   (int x, int y);
	void OnLeftReleased  (int x, int y);
	void OnRightPressed  (int x, int y);
	void OnRightReleased (int x, int y);
	void OnMiddlePressed (int x, int y);
	void OnMiddleReleased(int x, int y);
	void OnWheelUp(int x, int y);
	void OnWheelDown(int x, int y);
	void OnMouseMove(int x, int y);
	void OnMouseMoveRaw(int x, int y);

	bool IsLeftDown();
	bool IsMiddleDown();
	bool IsRightDown();

	int GetPositionX();
	int GetPositionY();
	MousePoint GetPosition();

	bool EventBufferIsEmpty();
	MouseEvent ReadEvent();
	
	/****************************************************************************
	**                Public Member Variables
	*****************************************************************************/

	/****************************************************************************
	**                Constructor and Destructor
	*****************************************************************************/
	Mouse()                        = default;
	Mouse(const Mouse&)            = default;
	Mouse& operator=(const Mouse&) = default;
	Mouse(Mouse&&)                 = default;
	Mouse& operator=(Mouse&&)      = default;
private:
	/****************************************************************************
	**                Private Function
	*****************************************************************************/

	/****************************************************************************
	**                Private Member Variables
	*****************************************************************************/
	std::queue<MouseEvent> _eventBuffer;
	bool _leftIsDown     = false;
	bool _rightIsDown    = false;
	bool _mbuttonIsDown  = false;
	int _x = 0;
	int _y = 0;
};

#pragma endregion
#endif
