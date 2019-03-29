#include "_GTO.h"

using namespace System::Windows::Forms;
using namespace System;

[STAThread]
int main(array<String^> ^ argv) 
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	Application::Run(gcnew tictactoeProyecto::_GTO());
	return 0;
}