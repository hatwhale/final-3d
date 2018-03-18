//#define _WIN32_WINNT 0x0501

#include "form.h"
#include <windows.h>

using namespace rendering;
[STAThread]
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{ 
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	Application::Run(gcnew RenderForm);
	return 0;
}