#include "MainGame/Include/Application.hpp"
#include <stdlib.h>
#include <iostream>
#include <Windows.h>
#include "GameCore/Include/Audio/AudioSource3D.hpp"

#ifdef _DEBUG
int main()
#else
int WINAPI WinMain(HINSTANCE hInstance,  HINSTANCE hPrevInstance, LPSTR lpCommandLine, int nShowCommand)
#endif
{
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

}

