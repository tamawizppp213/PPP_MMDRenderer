#include "MainGame/Include/Application.hpp"
#include <stdlib.h>
#include <iostream>
#include <Windows.h>


#if _DEBUG
int main()
#else
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCommandLine, _In_ int nShowCommand)
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

