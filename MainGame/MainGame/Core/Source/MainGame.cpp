#include "MainGame/Core/Include/Application.hpp"
#include <iostream>
#include <Windows.h>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG
int main()
#else
int WINAPI WinMain(HINSTANCE hInstance,  HINSTANCE hPrevInstance, LPSTR lpCommandLine, int nShowCommand)
#endif
{
    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    /********************************************
    **         Initialize
    *********************************************/
    Application& application = Application::Instance();
    if (!application.Initialize())
    {
        std::cout << "Initialize Error" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    /********************************************
    **         MainLoop
    *********************************************/
    application.Run();

    /********************************************
    **         Termination Process
    *********************************************/
    application.Terminate();
    exit(EXIT_SUCCESS);

    /********************************************
    **         Check MemoryLeaks
    *********************************************/
    //_CrtDumpMemoryLeaks();
}

