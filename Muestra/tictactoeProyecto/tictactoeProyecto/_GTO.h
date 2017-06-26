#pragma once

namespace tictactoeProyecto
{

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;

	using namespace System::Drawing;

	using namespace System::Net::Sockets;
	using namespace System::Net;
	using namespace System::Threading;
	using namespace System::Text;
	

	/// <summary>
	/// Resumen de _GTO
	/// </summary>



	public ref class _GTO : public System::Windows::Forms::Form
	{

		Socket ^_Socket, ^Cliente;
		Thread ^ FormThread,^NetworkThread;

		bool GameReady = false;
	    bool NetworkReady = true;// variable que controla el bucle del thread
	    bool FormsReady=true;   
		array<Byte> ^ Tablero; 
		array<Byte> ^ ExtraData;


		char player;//Current Tooken selected
		bool IsRunning=true;
		bool IsServer = false;
		bool IsConected = false;

		

	private: System::Windows::Forms::Button^  button12;


	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::TextBox^  textBox2;
	private: System::Windows::Forms::TextBox^  textBox3;
	private: System::Windows::Forms::Button^  button14;
	private: System::Windows::Forms::TextBox^  textBox4;
	private: System::Windows::Forms::TextBox^  textBox5;
	private: System::Windows::Forms::Button^  button15;

	private: System::Windows::Forms::ContextMenuStrip^  contextMenuStrip1;


	public:

		_GTO(void)
		{
			InitializeComponent();
			//
			//TODO: agregar código de constructor aquí
			//Initialze the network data
			Tablero = gcnew array<unsigned char>(9);
			ExtraData = gcnew array<unsigned char>(4);
			button1->Hide();
			button2->Hide();
			button3->Hide();
			button4->Hide();
			button5->Hide();
			button6->Hide();
			button7->Hide();
			button8->Hide();
			button9->Hide();
			button12->Hide();
			textBox1->Hide();
			textBox2->Hide();
			textBox3->Hide();
			label2->Hide();
			label3->Hide();

			button1->BackColor = Color::White;
			button1->ForeColor = Color::Black;
			button2->BackColor = Color::White;
			button2->ForeColor = Color::Black;
			button3->BackColor = Color::White;
			button3->ForeColor = Color::Black;
			button4->BackColor = Color::White;
			button4->ForeColor = Color::Black;
			button5->BackColor = Color::White;
			button5->ForeColor = Color::Black;
			button6->BackColor = Color::White;
			button6->ForeColor = Color::Black;
			button7->BackColor = Color::White;
			button7->ForeColor = Color::Black;
			button8->BackColor = Color::White;
			button8->ForeColor = Color::Black;
			button9->BackColor = Color::White;
			button9->ForeColor = Color::Black;



		}
		//variables 
		//app functions

		void Send()
		{
			//if(Cliente->)
			//Transmission Overview :  XXXXXXXXX--X----X----------X-----------X
			//                         TABLERO   TURNO VICTORIASA VICTORIASB  Status

			try
			{
				if(Cliente->Connected)
				{
					int BytesSended = Cliente->Send(Tablero, 9, SocketFlags::None);
					int ByteS2 = Cliente->Send(ExtraData, 4, SocketFlags::None);
				}
			}
			catch (SocketException ^ e) 
			{
				MessageBox::Show("The client doesn't exist anymore");//no es correcto hacer esto pero :V alv quiero acabar
			}
		}

		void ReciveGameData()
		{
			try 
			{
				if (Cliente->Available > 0)
				{
					//FirstPartOfTheTransmission...
					int BytesRecived = Cliente->Receive(Tablero, 9, SocketFlags::None);
					int BytesRecived2 = Cliente->Receive(ExtraData, 4, SocketFlags::None);
				}
			}
			catch (SocketException ^e) 
			{
				MessageBox::Show("Conexión perdida");
				CleanUp();
			}
		}

		void HoldForData()// network thread func
		{
			try
			{
				while (NetworkReady)
				{
					ReciveGameData();

				
			
					CheckWinner();
					// yyyy tambien preguntamos el byte de status haber que hacer respecto
					if (ExtraData[3] == 1) //Some one did a rage quit
					{
						MessageBox::Show("Un jugador ha abandonado la partida");
						
						CleanUp();
						NetworkReady = false;
					}
				}
			}
			catch (SocketException ^e)
			{
				NetworkReady = false;
				MessageBox::Show("Error");
			}
		}

		void UpdateForms(void) //Forms thread func
		{
			
			Monitor::Enter(ExtraData);
			Monitor::Enter(label2);
			Monitor::Enter(label3);
			Monitor::Enter(textBox1);
			Monitor::Enter(textBox2);
			Monitor::Enter(button1);
			Monitor::Enter(button3);
			Monitor::Enter(button4);
			Monitor::Enter(button5);
			Monitor::Enter(button6);
			Monitor::Enter(button7);
			Monitor::Enter(button8);
			Monitor::Enter(button9);
		
			
			while (FormsReady)
			{
				Thread::Sleep(60);

				//aprovechamos este loop para preguntar de quien es turno 
			    if (IsServer) 
				{
					if (ExtraData[0] % 2 == 0)
						GameReady = true;
					else
						GameReady = false;
			    }
				else
				{
					if (ExtraData[0] % 2 != 0)
						GameReady = true;
					else
						GameReady = false;
				}

				if (FormsReady)
				{
					//Labels 
					label2->Text = String::Concat("Ganadas J1 X: ", ExtraData[1].ToString());
					label3->Text = String::Concat("Ganadas J2 O: ", ExtraData[2].ToString());
					textBox1->BackColor = (ExtraData[0] % 2 == 0 ? Color::Red : Color::Blue);
					textBox2->Text = (ExtraData[0] % 2 == 0 ? "J1 X" : "J2 O");
					//ARRAY TO GRAPHICAL button->Text
					button1->Text = Convert::ToChar(Tablero[0]).ToString();
					button6->Text = Convert::ToChar(Tablero[1]).ToString();
					button9->Text = Convert::ToChar(Tablero[2]).ToString();
					button2->Text = Convert::ToChar(Tablero[3]).ToString();
					button5->Text = Convert::ToChar(Tablero[4]).ToString();
					button8->Text = Convert::ToChar(Tablero[5]).ToString();
					button3->Text = Convert::ToChar(Tablero[6]).ToString();
					button4->Text = Convert::ToChar(Tablero[7]).ToString();
					button7->Text = Convert::ToChar(Tablero[8]).ToString();
				}
			}
			
			Monitor::Exit(ExtraData);
			Monitor::Exit(label2);
			Monitor::Exit(label3);
			Monitor::Exit(textBox1);
			Monitor::Exit(textBox2);
			Monitor::Exit(button1);
			Monitor::Exit(button3);
			Monitor::Exit(button4);
			Monitor::Exit(button5);
			Monitor::Exit(button6);
			Monitor::Exit(button7);
			Monitor::Exit(button8);
			Monitor::Exit(button9);
			
		}

		bool CheckBoardFor(char PlayerToken) 
		{
		    //horizontal
			if (Tablero[0] == PlayerToken && Tablero[1] == PlayerToken && Tablero[2] == PlayerToken)
			{
				button1->BackColor = Color::Green;
				button1->ForeColor = Color::White;
				button6->BackColor = Color::Green;
				button6->ForeColor = Color::White;
				button9->BackColor = Color::Green;
				button9->ForeColor = Color::White;
				return true;
			}
			else if (Tablero[3] == PlayerToken && Tablero[4] == PlayerToken && Tablero[5] == PlayerToken)
			{
				button2->BackColor = Color::Green;
				button2->ForeColor = Color::White;
				button5->BackColor = Color::Green;
				button5->ForeColor = Color::White;
				button8->BackColor = Color::Green;
				button8->ForeColor = Color::White;
				return true;
			}
			else if (Tablero[6] == PlayerToken && Tablero[7] == PlayerToken && Tablero[8] == PlayerToken)
			{
				button3->BackColor = Color::Green;
				button3->ForeColor = Color::White;
				button4->BackColor = Color::Green;
				button4->ForeColor = Color::White;
				button7->BackColor = Color::Green;
				button7->ForeColor = Color::White;
				return true;
			}
			//vertical
			else if (Tablero[0] == PlayerToken && Tablero[3] == PlayerToken && Tablero[6] == PlayerToken)
			{
				button1->BackColor = Color::Green;
				button1->ForeColor = Color::White;
				button2->BackColor = Color::Green;
				button2->ForeColor = Color::White;
				button3->BackColor = Color::Green;
				button3->ForeColor = Color::White;
				return true;
			}
			else if (Tablero[1] == PlayerToken && Tablero[4] == PlayerToken && Tablero[7] == PlayerToken)
			{
				button4->BackColor = Color::Green;
				button4->ForeColor = Color::White;
				button6->BackColor = Color::Green;
				button6->ForeColor = Color::White;
				button5->BackColor = Color::Green;
				button5->ForeColor = Color::White;
				return true;
			}
			else if (Tablero[2] == PlayerToken && Tablero[5] == PlayerToken && Tablero[8] == PlayerToken)
			{
				button7->BackColor = Color::Green;
				button7->ForeColor = Color::White;
				button8->BackColor = Color::Green;
				button8->ForeColor = Color::White;
				button9->BackColor = Color::Green;
				button9->ForeColor = Color::White;
				return true;
			}
			//Diagonal IZQ
			else if (Tablero[6] == PlayerToken && Tablero[4] == PlayerToken && Tablero[2] == PlayerToken)
			{
				button3->BackColor = Color::Green;
				button3->ForeColor = Color::White;
				button5->BackColor = Color::Green;
				button5->ForeColor = Color::White;
				button9->BackColor = Color::Green;
				button9->ForeColor = Color::White;
				return true;
			}
			//Diagonal Derecha
			else if (Tablero[0] == PlayerToken && Tablero[4] == PlayerToken && Tablero[8] == PlayerToken)
			{
				button1->BackColor = Color::Green;
				button1->ForeColor = Color::White;
				button5->BackColor = Color::Green;
				button5->ForeColor = Color::White;
				button7->BackColor = Color::Green;
				button7->ForeColor = Color::White;
				return true;
			}
			else
				return 0;
		}
		
		void CheckWinner()
		{
			if (IsServer) 
			{
				if (CheckBoardFor('X')) 
				{
				    //Gano x
					ExtraData[1]++;
					ResetGame();
					Send();
					MessageBox::Show("X ha ganado <3");
					
				}
			}
			else if(ExtraData[0]>=9) //comprobamos primero que no sea turno 9
			{
				//Empate
				ResetGame();
				Send();
				MessageBox::Show("Nadie ha ganado 7u7");
			}
			else 
			{
				if (CheckBoardFor('O'))
				{
					//gano 
					ExtraData[2]++;
					ResetGame();
					Send();
					MessageBox::Show("O ha ganado");
				}
			}

		}
		void ResetGame()
		{
			ExtraData[0] = 0;// regresamos los turnos a 0 
			for (int i = 0; i < 9; i++)
			{
				Tablero[i] = 0;
			}
			button1->BackColor = Color::White;
			button1->ForeColor = Color::Black;
			button2->BackColor = Color::White;
			button2->ForeColor = Color::Black;
			button3->BackColor = Color::White;
			button3->ForeColor = Color::Black;
			button4->BackColor = Color::White;
			button4->ForeColor = Color::Black;
			button5->BackColor = Color::White;
			button5->ForeColor = Color::Black;
			button6->BackColor = Color::White;
			button6->ForeColor = Color::Black;
			button7->BackColor = Color::White;
			button7->ForeColor = Color::Black;
			button8->BackColor = Color::White;
			button8->ForeColor = Color::Black;
			button9->BackColor = Color::White;
			button9->ForeColor = Color::Black;
		}

		void CleanUp() 
		{
			
			if (IsServer)
			{
				_Socket->Close();//cerramos el listener para conexiones entrantes
				Cliente->Close();//Cerramos el socket principal
								 //label1->Text = "Server Disconected...";
			}
			else
			{
				Cliente->Close();//Cerramos el socket principal
								 //label1->Text = "Client Disconected...";
			}
			
			ResetGame();
			ExtraData[1] = 0;
			ExtraData[2] = 0;
			ExtraData[3] = 0;
			button12->Enabled = true;
			//back to main menu
			button10->Show();
			button11->Show();
			button15->Show();
			textBox4->Show();
			textBox5->Show();
			button1->Hide();
			button2->Hide();
			button3->Hide();
			button4->Hide();
			button5->Hide();
			button6->Hide();
			button7->Hide();
			button8->Hide();
			button9->Hide();
			button12->Hide();
			textBox1->Hide();
			textBox2->Hide();
			textBox3->Hide();
			label2->Hide();
			label3->Hide();
		
		}
	


	protected:
		/// <summary>
		/// Limpiar los recursos que se estén usando.
		/// </summary>
		~_GTO()
		{
			if (components)
			{
				IsRunning = false;
				delete components;

				FormsReady = false;
				NetworkReady = false;
			}
		}

	protected:
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Button^  button11;
	private: System::Windows::Forms::Button^  button10;
	private: System::Windows::Forms::Button^  button13;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::Button^  button3;
	private: System::Windows::Forms::Button^  button6;
	private: System::Windows::Forms::Button^  button5;
	private: System::Windows::Forms::Button^  button4;
	private: System::Windows::Forms::Button^  button9;
	private: System::Windows::Forms::Button^  button8;
	private: System::Windows::Forms::Button^  button7;
	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Variable del diseñador necesaria.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Método necesario para admitir el Diseñador. No se puede modificar
		/// el contenido de este método con el editor de código.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->button11 = (gcnew System::Windows::Forms::Button());
			this->button10 = (gcnew System::Windows::Forms::Button());
			this->button13 = (gcnew System::Windows::Forms::Button());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->button6 = (gcnew System::Windows::Forms::Button());
			this->button5 = (gcnew System::Windows::Forms::Button());
			this->button4 = (gcnew System::Windows::Forms::Button());
			this->button9 = (gcnew System::Windows::Forms::Button());
			this->button8 = (gcnew System::Windows::Forms::Button());
			this->button7 = (gcnew System::Windows::Forms::Button());
			this->button12 = (gcnew System::Windows::Forms::Button());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->textBox2 = (gcnew System::Windows::Forms::TextBox());
			this->contextMenuStrip1 = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->textBox3 = (gcnew System::Windows::Forms::TextBox());
			this->button14 = (gcnew System::Windows::Forms::Button());
			this->textBox4 = (gcnew System::Windows::Forms::TextBox());
			this->textBox5 = (gcnew System::Windows::Forms::TextBox());
			this->button15 = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->ForeColor = System::Drawing::Color::Black;
			this->label3->Location = System::Drawing::Point(93, 271);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(78, 13);
			this->label3->TabIndex = 30;
			this->label3->Text = L"Ganadas J2 O:";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->ForeColor = System::Drawing::Color::Black;
			this->label2->Location = System::Drawing::Point(93, 243);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(77, 13);
			this->label2->TabIndex = 29;
			this->label2->Text = L"Ganadas J1 X:";
			this->label2->Click += gcnew System::EventHandler(this, &_GTO::label2_Click);
			// 
			// button11
			// 
			this->button11->ForeColor = System::Drawing::Color::Black;
			this->button11->Location = System::Drawing::Point(94, 111);
			this->button11->Name = L"button11";
			this->button11->Size = System::Drawing::Size(75, 23);
			this->button11->TabIndex = 27;
			this->button11->Text = L"Conectar";
			this->button11->UseVisualStyleBackColor = true;
			this->button11->Click += gcnew System::EventHandler(this, &_GTO::button11_Click);
			// 
			// button10
			// 
			this->button10->ForeColor = System::Drawing::Color::Black;
			this->button10->Location = System::Drawing::Point(94, 82);
			this->button10->Name = L"button10";
			this->button10->Size = System::Drawing::Size(75, 23);
			this->button10->TabIndex = 25;
			this->button10->Text = L"Crear ";
			this->button10->UseVisualStyleBackColor = true;
			this->button10->Click += gcnew System::EventHandler(this, &_GTO::button10_Click);
			// 
			// button13
			// 
			this->button13->ForeColor = System::Drawing::Color::Black;
			this->button13->Location = System::Drawing::Point(292, 263);
			this->button13->Name = L"button13";
			this->button13->Size = System::Drawing::Size(64, 23);
			this->button13->TabIndex = 32;
			this->button13->Text = L"Salir";
			this->button13->UseVisualStyleBackColor = true;
			this->button13->Click += gcnew System::EventHandler(this, &_GTO::button13_Click);
			// 
			// button1
			// 
			this->button1->ForeColor = System::Drawing::Color::Black;
			this->button1->Location = System::Drawing::Point(94, 48);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(52, 57);
			this->button1->TabIndex = 20;
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &_GTO::button1_Click);
			// 
			// button2
			// 
			this->button2->ForeColor = System::Drawing::Color::Black;
			this->button2->Location = System::Drawing::Point(94, 111);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(52, 57);
			this->button2->TabIndex = 17;
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &_GTO::button2_Click);
			// 
			// button3
			// 
			this->button3->ForeColor = System::Drawing::Color::Black;
			this->button3->Location = System::Drawing::Point(95, 175);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(52, 57);
			this->button3->TabIndex = 18;
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &_GTO::button3_Click);
			// 
			// button6
			// 
			this->button6->ForeColor = System::Drawing::Color::Black;
			this->button6->Location = System::Drawing::Point(153, 48);
			this->button6->Name = L"button6";
			this->button6->Size = System::Drawing::Size(52, 57);
			this->button6->TabIndex = 19;
			this->button6->UseVisualStyleBackColor = true;
			this->button6->Click += gcnew System::EventHandler(this, &_GTO::button6_Click);
			// 
			// button5
			// 
			this->button5->ForeColor = System::Drawing::Color::Black;
			this->button5->Location = System::Drawing::Point(153, 111);
			this->button5->Name = L"button5";
			this->button5->Size = System::Drawing::Size(52, 57);
			this->button5->TabIndex = 20;
			this->button5->UseVisualStyleBackColor = true;
			this->button5->Click += gcnew System::EventHandler(this, &_GTO::button5_Click);
			// 
			// button4
			// 
			this->button4->ForeColor = System::Drawing::Color::Black;
			this->button4->Location = System::Drawing::Point(154, 175);
			this->button4->Name = L"button4";
			this->button4->Size = System::Drawing::Size(52, 57);
			this->button4->TabIndex = 21;
			this->button4->UseVisualStyleBackColor = true;
			this->button4->Click += gcnew System::EventHandler(this, &_GTO::button4_Click);
			// 
			// button9
			// 
			this->button9->ForeColor = System::Drawing::Color::Black;
			this->button9->Location = System::Drawing::Point(212, 48);
			this->button9->Name = L"button9";
			this->button9->Size = System::Drawing::Size(52, 57);
			this->button9->TabIndex = 22;
			this->button9->UseVisualStyleBackColor = true;
			this->button9->Click += gcnew System::EventHandler(this, &_GTO::button9_Click);
			// 
			// button8
			// 
			this->button8->ForeColor = System::Drawing::Color::Black;
			this->button8->Location = System::Drawing::Point(212, 111);
			this->button8->Name = L"button8";
			this->button8->Size = System::Drawing::Size(52, 57);
			this->button8->TabIndex = 23;
			this->button8->UseVisualStyleBackColor = true;
			this->button8->Click += gcnew System::EventHandler(this, &_GTO::button8_Click);
			// 
			// button7
			// 
			this->button7->ForeColor = System::Drawing::Color::Black;
			this->button7->Location = System::Drawing::Point(212, 175);
			this->button7->Name = L"button7";
			this->button7->Size = System::Drawing::Size(52, 57);
			this->button7->TabIndex = 24;
			this->button7->UseVisualStyleBackColor = true;
			this->button7->Click += gcnew System::EventHandler(this, &_GTO::button7_Click);
			// 
			// button12
			// 
			this->button12->ForeColor = System::Drawing::Color::Black;
			this->button12->Location = System::Drawing::Point(270, 48);
			this->button12->Name = L"button12";
			this->button12->Size = System::Drawing::Size(86, 29);
			this->button12->TabIndex = 34;
			this->button12->Text = L"Desconectar";
			this->button12->UseVisualStyleBackColor = true;
			this->button12->Click += gcnew System::EventHandler(this, &_GTO::button12_Click);
			// 
			// textBox1
			// 
			this->textBox1->Location = System::Drawing::Point(9, 20);
			this->textBox1->Margin = System::Windows::Forms::Padding(2);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(64, 20);
			this->textBox1->TabIndex = 37;
			this->textBox1->Text = L"TURNO";
			this->textBox1->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->textBox1->TextChanged += gcnew System::EventHandler(this, &_GTO::textBox1_TextChanged);
			// 
			// textBox2
			// 
			this->textBox2->Location = System::Drawing::Point(9, 35);
			this->textBox2->Margin = System::Windows::Forms::Padding(2);
			this->textBox2->Name = L"textBox2";
			this->textBox2->Size = System::Drawing::Size(64, 20);
			this->textBox2->TabIndex = 38;
			this->textBox2->Text = L"J1";
			this->textBox2->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->textBox2->TextChanged += gcnew System::EventHandler(this, &_GTO::textBox2_TextChanged);
			// 
			// contextMenuStrip1
			// 
			this->contextMenuStrip1->ImageScalingSize = System::Drawing::Size(20, 20);
			this->contextMenuStrip1->Name = L"contextMenuStrip1";
			this->contextMenuStrip1->Size = System::Drawing::Size(61, 4);
			this->contextMenuStrip1->Opening += gcnew System::ComponentModel::CancelEventHandler(this, &_GTO::contextMenuStrip1_Opening);
			// 
			// textBox3
			// 
			this->textBox3->ForeColor = System::Drawing::SystemColors::InfoText;
			this->textBox3->Location = System::Drawing::Point(142, 10);
			this->textBox3->Margin = System::Windows::Forms::Padding(2);
			this->textBox3->Name = L"textBox3";
			this->textBox3->Size = System::Drawing::Size(76, 20);
			this->textBox3->TabIndex = 39;
			this->textBox3->Text = L"ERES";
			this->textBox3->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// button14
			// 
			this->button14->ForeColor = System::Drawing::Color::Black;
			this->button14->Location = System::Drawing::Point(270, 10);
			this->button14->Margin = System::Windows::Forms::Padding(2);
			this->button14->Name = L"button14";
			this->button14->Size = System::Drawing::Size(86, 28);
			this->button14->TabIndex = 40;
			this->button14->Text = L"AYUDA";
			this->button14->UseVisualStyleBackColor = true;
			this->button14->Click += gcnew System::EventHandler(this, &_GTO::button14_Click);
			// 
			// textBox4
			// 
			this->textBox4->Location = System::Drawing::Point(175, 82);
			this->textBox4->Name = L"textBox4";
			this->textBox4->Size = System::Drawing::Size(100, 20);
			this->textBox4->TabIndex = 41;
			this->textBox4->Text = L"127.0.0.1";
			// 
			// textBox5
			// 
			this->textBox5->Location = System::Drawing::Point(175, 111);
			this->textBox5->Name = L"textBox5";
			this->textBox5->Size = System::Drawing::Size(100, 20);
			this->textBox5->TabIndex = 42;
			this->textBox5->Text = L"127.0.0.1";
			// 
			// button15
			// 
			this->button15->Location = System::Drawing::Point(94, 140);
			this->button15->Name = L"button15";
			this->button15->Size = System::Drawing::Size(75, 23);
			this->button15->TabIndex = 43;
			this->button15->Text = L"Sobre...";
			this->button15->UseVisualStyleBackColor = true;
			this->button15->Click += gcnew System::EventHandler(this, &_GTO::button15_Click);
			// 
			// _GTO
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(359, 297);
			this->ControlBox = false;
			this->Controls->Add(this->button15);
			this->Controls->Add(this->textBox5);
			this->Controls->Add(this->textBox4);
			this->Controls->Add(this->button14);
			this->Controls->Add(this->textBox3);
			this->Controls->Add(this->textBox2);
			this->Controls->Add(this->textBox1);
			this->Controls->Add(this->button12);
			this->Controls->Add(this->button13);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->button11);
			this->Controls->Add(this->button10);
			this->Controls->Add(this->button7);
			this->Controls->Add(this->button8);
			this->Controls->Add(this->button9);
			this->Controls->Add(this->button4);
			this->Controls->Add(this->button5);
			this->Controls->Add(this->button6);
			this->Controls->Add(this->button3);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->button1);
			this->Cursor = System::Windows::Forms::Cursors::Cross;
			this->DoubleBuffered = true;
			this->ForeColor = System::Drawing::Color::DarkGoldenrod;
			this->Name = L"_GTO";
			this->ShowIcon = false;
			this->Text = L"Tic Tac Gato ";
			this->Load += gcnew System::EventHandler(this, &_GTO::_GTO_Load);
			this->ResumeLayout(false);
			this->PerformLayout();

		}

#pragma endregion


	private: System::Void _GTO_Load(System::Object^  sender, System::EventArgs^  e)
	{
		CheckForIllegalCrossThreadCalls = false; //alv usamos esta madre pa que funcione
		FormThread = gcnew Thread(gcnew ThreadStart(this,&_GTO::UpdateForms)); //Initialize the form updater thread
	   
		FormThread->Start();//inicializamos este thread ya que solo es para actualizar lo que se ve en pantalla
	}

	//Host Button
	private: System::Void button10_Click(System::Object^  sender, System::EventArgs^  e)
	{
		try
		{
			_Socket = gcnew Socket(AddressFamily::InterNetwork, SocketType::Stream, ProtocolType::Tcp);
			_Socket->Bind(gcnew IPEndPoint(IPAddress::Parse(textBox4->Text), 5000));
			_Socket->Listen(0);
			Cliente = _Socket->Accept();
			if (Cliente != nullptr)
			{
				IsServer = true;
				player = 'X';
				
		
				
					textBox3->Text = "ERES J1 X";
					textBox3->BackColor = Color::Red;
					textBox3->ForeColor = Color::White;
					NetworkReady = true;
					NetworkThread = gcnew Thread(gcnew ThreadStart(this, &_GTO::HoldForData)); //Initialize the NetworkThread
					NetworkThread->Start();
				

				button10->Hide();
				button11->Hide();
				button15->Hide();
				textBox4->Hide();
				textBox5->Hide();
				button1->Show();
				button2->Show();
				button3->Show();
				button4->Show();
				button5->Show();
				button6->Show();
				button7->Show();
				button8->Show();
				button9->Show();
				button12->Show();
				textBox1->Show();
				textBox2->Show();
				textBox3->Show();
				label2->Show();
				label3->Show();

			}
		}
		catch (System::Net::Sockets::SocketException^ error)
		{
			MessageBox::Show("Error");
		}
	}
			 //Conect button
	private: System::Void button11_Click(System::Object^  sender, System::EventArgs^  e)
	{
		try
		{
			Cliente = gcnew Socket(AddressFamily::InterNetwork, SocketType::Stream, ProtocolType::Tcp);
			Cliente->Connect(gcnew IPEndPoint(IPAddress::Parse(textBox5->Text), 5000)); //TODO: cambiar esto por el campo de un textbox para la ip
			if (Cliente->IsBound)
			{
				IsServer = false;
				player = 'O';
				
				textBox3->Text = "ERES J2 O";
				textBox3->BackColor = Color::Blue;
				textBox3->ForeColor = Color::White;
				
				NetworkReady = true;
				NetworkThread = gcnew Thread(gcnew ThreadStart(this, &_GTO::HoldForData)); //Initialize the NetworkThread
				NetworkThread->Start();

				button10->Hide();
				button11->Hide();
				button15->Hide();
				textBox4->Hide();
				textBox5->Hide();
				button1->Show();
				button2->Show();
				button3->Show();
				button4->Show();
				button5->Show();
				button6->Show();
				button7->Show();
				button8->Show();
				button9->Show();
				button12->Show();
				textBox1->Show();
				textBox2->Show();
				textBox3->Show();
				label2->Show();
				label3->Show();
			}
		}
		catch (System::Net::Sockets::SocketException^ error)
		{
			MessageBox::Show("Error:Can't connect");
		}
	}

	// EL CODIGO AQUI REPRESENTA LA MISMA FUNCIONALIDAD EN CADA BOTON PERO CON DIFERENTE INDICE EN TABLERO(botones)
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if (GameReady)
		{
			if (Tablero[0] == 0)
			{
				Tablero[0] = player;
				ExtraData[0]++;
				Send();
			}
		}
	}

	private: System::Void button6_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if (GameReady)
		{
			if (Tablero[1] == 0)
			{
				Tablero[1] = player;
				ExtraData[0]++;
				Send();
			}
		}
	}

	private: System::Void button9_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if (GameReady)
		{
			if (Tablero[2] == 0)
			{
				Tablero[2] = player;
				ExtraData[0]++;
				Send();
			}

		}
	}

	private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if (GameReady)
		{
			if (Tablero[3] == 0)
			{
				Tablero[3] = player;
				ExtraData[0]++;
				Send();
			}
		}
	}

	private: System::Void button5_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if (GameReady)
		{
			if (Tablero[4] == 0)
			{
				Tablero[4] = player;
				ExtraData[0]++;
				Send();
			}
		}
	}

	private: System::Void button8_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if (GameReady)
		{
			if (Tablero[5] == 0)
			{
				Tablero[5] = player;
				ExtraData[0]++;
				Send();
			}
		}
	}

	private: System::Void button3_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if (GameReady)
		{
			if (Tablero[6] == 0)
			{
				Tablero[6] = player;
				ExtraData[0]++;
				Send();
			}
		}
	}

	private: System::Void button4_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if (GameReady)
		{
			if (Tablero[7] == 0)
			{
				Tablero[7] = player;
				ExtraData[0]++;
				Send();
			}
		}
	}

	private: System::Void button7_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if (GameReady)
		{
			if (Tablero[8] == 0)
			{
				Tablero[8] = player;
				ExtraData[0]++;
				Send();
			}
		}
	}

   //exit button
    private: System::Void button13_Click(System::Object^  sender, System::EventArgs^  e)
	{
		ExtraData[3] = 1;
		Send();
		this->Close();
	}
	//Disconnect button
	private: System::Void button12_Click(System::Object^  sender, System::EventArgs^  e)
	{
		button12->Enabled = false;
		ExtraData[3] = 1;
		Send();
		NetworkReady = false;
		CleanUp();
	}

private: System::Void label4_Click(System::Object^  sender, System::EventArgs^  e) 
{
}
private: System::Void label2_Click(System::Object^  sender, System::EventArgs^  e) 
{
}
private: System::Void label1_Click(System::Object^  sender, System::EventArgs^  e) 
{
}
private: System::Void textBox1_TextChanged(System::Object^  sender, System::EventArgs^  e) 
{
}
private: System::Void contextMenuStrip1_Opening(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) 
{
}
private: System::Void textBox2_TextChanged(System::Object^  sender, System::EventArgs^  e) 
{
}
private: System::Void button14_Click(System::Object^  sender, System::EventArgs^  e) 
{
	MessageBox::Show("Si presionas la opción CONECTAR se creará un servidor en donde tendrás que esperar a que otro jugador se conecte a el. \n \n Si presionas la opción BUSCAR podrás ingresar a un servidor que otro jugador haya abierto previamente. \n \n El juego consiste en lograr marcar tres casillas en ya seavertical, horizontal o en diagonal, en cuanto esto suceda el juego habrá finalizado \n \n Puedes retirarte en cualquier momento de la partida con el botón SALIR, en caso de que el jugador contrincante desee salir del juego se enviará un mensaje de error");
}
private: System::Void button15_Click(System::Object^  sender, System::EventArgs^  e) {
	MessageBox::Show("Luis Gabriel Hernadez Hernadez    	15300123\nSaúl Aceves Montes                      15100004\nMariana Ramírez Llamas              	15300228\nAxel Germán Fernández Rojas		15100117\nLuis Fernando Aguirre Esteves     	15300822\nJesús Emmanuel Garza Flores	   	15300100\nSaúl Napoleón Haro Hernández            15300116\n4C1\nMtra. Susana Elizabeth Ferrer Hernández\n22/06/2017\n");

}
};
}