//////////////////////////////////////////////////////////////////////////////////
//              Title:  GameManager.cpp
//            Content:  Game Scene Management (Singleton)
//             Author:  Toide Yutaro
//             Create:  2020_11_
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
//                             Include
//////////////////////////////////////////////////////////////////////////////////
#include "MainGame/Include/Application.hpp"

//////////////////////////////////////////////////////////////////////////////////
//                              Define
//////////////////////////////////////////////////////////////////////////////////
#define CLASS_NAME L"Game Window"
#define GAME_TITLE L"Animal Kingdom"

//////////////////////////////////////////////////////////////////////////////////
//                              Implement
//////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return Application::Instance().WindowMessageProcedure(hwnd, message, wParam, lParam);
}


bool Application::Initialize()
{
	if (!InitializeMainWindow()) { return false; }
	if (!InitializeGameInput())  { return false; }
	if (!InitializeGameAudio())  { return false; }
	return true;
}

void Application::Run()
{
	MSG message = { 0 };

	_gameTimer.Reset();
	
	_gameManager.Instance().SetHWND(_mainWindow);
	_gameManager.Instance().GameStart(_gameTimer);
	/*---------------------------------------------------------------
						Main Loop
	-----------------------------------------------------------------*/
	while (WM_QUIT != message.message)
	{
		if (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			_gameTimer.Tick();

			if (!_isApplicationPaused)
			{
				_gameTimer.AverageFrame(_mainWindow);
				_gameInput.Update();
				_gameManager.Instance().GameMain();
			}
		}
	}

}

void Application::Terminate()
{
	_gameAudio.Finalize();
	_gameInput.Finalize();
	_gameManager.Instance().GameEnd();
}

#pragma region Private Function
LRESULT Application::WindowMessageProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	/*-----------------------------------------------------------------
	  WM_ACTIVATE is sent when the windows is activated or deactivated
	--------------------------------------------------------------------*/
	case WM_ACTIVATE:  
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			_gameInput.GetGamePad().EnableXInput(FALSE);
			_isApplicationPaused = true;
			_gameTimer.Stop();
		}
		else
		{
			_gameInput.GetGamePad().EnableXInput(TRUE);
			_isApplicationPaused = false;
			_gameTimer.Start();

		}
		return 0;


	/*-----------------------------------------------------------------
	  WM_SIZE is sent when the windows size is changed or changing
	--------------------------------------------------------------------*/
	case WM_SIZE:

		_screen.SetScreenWidth (LOWORD(lParam));
		_screen.SetScreenHeight(HIWORD(lParam));
		
		if (_gameManager.GetDirectX12()->GetDevice())
		{
			switch (wParam)
			{
			case SIZE_MINIMIZED:
				_isApplicationPaused = true;
				_isMinimized         = true;
				_isMaximized         = false;

				break;

			case SIZE_MAXIMIZED:
				_isApplicationPaused = false;
				_isMinimized         = false;
				_isMaximized         = true;
				_gameManager.GetDirectX12()->OnResize();
				break;

			case SIZE_RESTORED:
				
				// Restoring from minimized state?
				if (_isMinimized)
				{
					_isApplicationPaused = false;
					_isMinimized         = false;
					_gameManager.GetDirectX12()->OnResize();
				}
				// Restoring from maximized state?
				else if (_isMaximized)
				{
					_isApplicationPaused = false;
					_isMaximized = false;
					_gameManager.GetDirectX12()->OnResize();
				}
				else if (_isResizing) {}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					_gameManager.GetDirectX12()->OnResize();
				}
				break;
			default:
				break;
			}
		}
		return 0;

	case WM_ENTERSIZEMOVE:
		_isApplicationPaused = true;
		_isResizing          = true;
		
		_gameTimer.Stop();
		return 0;
	
	case WM_EXITSIZEMOVE:
		_isApplicationPaused = false;
		_isResizing          = false;
		_gameTimer.Start();
		_gameManager.GetDirectX12()->OnResize();
		return 0;
	/*-----------------------------------------------------------------
		WM_CLOSE is sent when the window is closed
	--------------------------------------------------------------------*/
	case WM_CLOSE:  
		if (MessageBox(hwnd, L"              ゲームを終了しますか?", L" 確認", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK)
		{
			DestroyWindow(hwnd);
		}
		return 0;
	/*-----------------------------------------------------------------
		WM_DESTROY is sent when the window is deleted
	--------------------------------------------------------------------*/
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);
	/*-----------------------------------------------------------------
		WM_KEYUP is sent when the key is inputed 
	--------------------------------------------------------------------*/
	case WM_KEYUP:
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
		}
		else if ((int)wParam == VK_F2)
		{
			_gameManager.GetDirectX12()->Set4xMsaaState(!_gameManager.GetDirectX12()->Get4xMsaaState());
		}
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

bool Application::InitializeMainWindow()
{
	/*---------------------------------------------------------------
						Register Window Class
	-----------------------------------------------------------------*/
	_appInstance = GetModuleHandle(NULL);
	WNDCLASS wc = {};
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProcedure;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = _appInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = CLASS_NAME;

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"Register Class Failed.", 0, 0);
	}

	/*---------------------------------------------------------------
						Create Window Object
	-----------------------------------------------------------------*/
	DWORD window_style = WS_OVERLAPPEDWINDOW;
	RECT  window_rect  = { 0,0,_screen.GetScreenWidth(),_screen.GetScreenHeight() }; 	// Create Window Rect
	AdjustWindowRect(&window_rect, window_style, false);

	// Recreate window size
	int window_width  = window_rect.right - window_rect.left;
	int window_height = window_rect.bottom - window_rect.top;
	_screen.SetScreenWidth (window_width);
	_screen.SetScreenHeight(window_height);

	// Acquire desktop resolution
	int desktop_width  = GetSystemMetrics(SM_CXSCREEN);
	int desktop_height = GetSystemMetrics(SM_CYSCREEN);

	// Centering window
	int window_x = max((desktop_width - window_width) / 2, 0);
	int window_y = max((desktop_height - window_height) / 2, 0);

	// Create window
	_mainWindow = CreateWindow(
		CLASS_NAME,   // Name to register with Windows
		GAME_TITLE,   // Game Title
		window_style, // Window style
		window_x,     // Center position_x
		window_y,     // Center position_y
		window_width, // Window width
		window_height,// Window height
		NULL,         // Parent window
		NULL,         // Menu handler
		_appInstance, // instance handler
		NULL);        // Additional expansion area

	// Check window
	if (!_mainWindow)
	{
		MessageBox(0, L"Create Window Failed.", 0, 0);
		return false;
	}

	/*---------------------------------------------------------------
							Show Window
	-----------------------------------------------------------------*/
	ShowWindow(_mainWindow, SW_SHOW);
	UpdateWindow(_mainWindow);
	return true;
}

bool Application::InitializeGameInput()
{
	bool result = true;
	result      = _gameInput.Initialize(_appInstance, _mainWindow);
	return result;
}

bool Application::InitializeGameAudio()
{
	bool result = _gameAudio.Initialize();
	return result;
}
#pragma endregion Private Function