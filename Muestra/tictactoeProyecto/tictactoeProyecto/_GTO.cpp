#include "_GTO.h"

using namespace System::Windows::Forms;
using namespace System;




[STAThread]
void main(array<String^> ^ argv) 
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	Application::Run(gcnew tictactoeProyecto::_GTO());
}