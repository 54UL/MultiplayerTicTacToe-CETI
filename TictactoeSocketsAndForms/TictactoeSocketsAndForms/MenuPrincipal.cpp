#include "MenuPrincipal.h"

using namespace System;
using namespace System::Windows::Forms;

[STAThread]
void main(array<String^>^ args)
{
	TictactoeSocketsAndForms::MenuPrincipal^ MenuP;
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	Application::Run(MenuP);
}