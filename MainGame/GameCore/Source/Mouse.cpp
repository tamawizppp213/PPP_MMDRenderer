//////////////////////////////////////////////////////////////////////////////////
///             @file   Mouse.cpp
///             @brief  Mouse
///             @author Toide Yutaro
///             @date   2020_11_29
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "GameCore/Include/Mouse.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//							Implement
//////////////////////////////////////////////////////////////////////////////////
#pragma region Mouse Event
MouseEvent::MouseEvent()
{
	_type = EventType::Invalid;
	_x    = 0;
	_y    = 0;
}

MouseEvent::MouseEvent(const EventType type, int x, int y)
{
	_type = type;
	_x = x;
	_y = y;
}

bool MouseEvent::IsValid() const
{
	return this->_type != EventType::Invalid;
}

MouseEvent::EventType MouseEvent::GetType() const
{
	return this->_type;
}

MousePoint MouseEvent::GetPosition() const
{
	return{ this->_x,  this->_y };
}

int MouseEvent::GetPositionX() const
{
	return this->_x;
}

int MouseEvent::GetPositionY() const
{
	return this->_y;
}
#pragma endregion Mouse Event

#pragma region Mouse
void Mouse::OnLeftPressed(int x, int y)
{
	this->_leftIsDown = true;
	MouseEvent mouseEvent(MouseEvent::EventType::LPress, x, y);
	this->_eventBuffer.push(mouseEvent);
}

void Mouse::OnLeftReleased(int x, int y)
{
	this->_leftIsDown = false;
	this->_eventBuffer.push(MouseEvent(MouseEvent::EventType::LRelease, x, y));

}

void Mouse::OnRightPressed(int x, int y)
{
	this->_rightIsDown = true;
	this->_eventBuffer.push(MouseEvent(MouseEvent::EventType::RPress, x, y));
}

void Mouse::OnRightReleased(int x, int y)
{
	this->_rightIsDown = false;
	this->_eventBuffer.push(MouseEvent(MouseEvent::EventType::RRelease, x, y));
}

void Mouse::OnMiddlePressed(int x, int y)
{
	this->_mbuttonIsDown = true;
	this->_eventBuffer.push(MouseEvent(MouseEvent::EventType::MPress, x, y));
}

void Mouse::OnMiddleReleased(int x, int y)
{
	this->_mbuttonIsDown = false;
	this->_eventBuffer.push(MouseEvent(MouseEvent::EventType::MRelease, x, y));
}

void Mouse::OnWheelUp(int x, int y)
{
	this->_eventBuffer.push(MouseEvent(MouseEvent::EventType::WheelUp, x, y));
}

void Mouse::OnWheelDown(int x, int y)
{
	this->_eventBuffer.push(MouseEvent(MouseEvent::EventType::WheelDown, x, y));
}

void Mouse::OnMouseMove(int x, int y)
{
	this->_x = x;
	this->_y = y;
	this->_eventBuffer.push(MouseEvent(MouseEvent::EventType::Move, x, y));
}

void Mouse::OnMouseMoveRaw(int x, int y)
{
	this->_eventBuffer.push(MouseEvent(MouseEvent::EventType::RawMove, x, y));
}

bool Mouse::IsLeftDown()
{
	return this->_leftIsDown;
}

bool Mouse::IsMiddleDown()
{
	return this->_mbuttonIsDown;
}

bool Mouse::IsRightDown()
{
	return this->_rightIsDown;
}

int Mouse::GetPositionX()
{
	return this->_x;
}

int Mouse::GetPositionY()
{
	return this->_y;
}

MousePoint Mouse::GetPosition()
{
	return { this->_x, this->_y };
}

bool Mouse::EventBufferIsEmpty()
{
	return this->_eventBuffer.empty();
}

MouseEvent Mouse::ReadEvent()
{
	if (this->_eventBuffer.empty())
	{
		return MouseEvent();
	}
	else
	{
		MouseEvent event = this->_eventBuffer.front();
		this->_eventBuffer.pop();
		return event;
	}
}
#pragma endregion Mouse