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
		
		array<Byte> ^ Tablero;
		array<Byte> ^ ExtraData;


		char player;//Current Tooken selected
		bool IsRunning=true;
		bool IsServer = false;
		bool IsConected = false;

		

	private: System::Windows::Forms::Button^  button12;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label6;
    private: System::Windows::Forms::Label^  label4;

	public:

		_GTO(void)
		{
			InitializeComponent();
			//
			//TODO: agregar código de constructor aquí
			//Initialze the network data
			Tablero = gcnew array<unsigned char>(9);
			ExtraData = gcnew array<unsigned char>(4);
		}
		//variables 
		//app functions

		void Send()
		{
			//Transmission Overview :  XXXXXXXXX--X----X----------X-----------X
			//                         TABLERO   TURNO VICTORIASA VICTORIASB  Status
			//TO DO: AÑADIR EXCEPCIONES
			int BytesSended = Cliente->Send(Tablero, 9, SocketFlags::None);
			int ByteS2 = Cliente->Send(ExtraData, 4, SocketFlags::None);
			label5->Text = String::Concat("Bytes enviados:", (BytesSended + ByteS2).ToString());
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
					label6->Text = String::Concat("bytes recividos:", (BytesRecived + BytesRecived2).ToString());
				}
			}
			catch (SocketException ^e) 
			{
				label1->Text = e->Message->ToString();
				MessageBox::Show("Connection lost");
				CleanUp();
				
			}
		}

		void HoldForData()// network thread func
		{
			try
			{
				while (NetworkThread->IsAlive)
				{
					ReciveGameData();
				}
			}
			catch (SocketException ^e)
			{
				label1->Text = e->Message->ToString();
			}
		}

		void UpdateForms(void) //Forms thread func
		{
			
			
			Monitor::Enter(ExtraData);
			Monitor::Enter(label2);
			Monitor::Enter(label3);
			Monitor::Enter(label4);
			Monitor::Enter(button1);
			Monitor::Enter(button3);
			Monitor::Enter(button4);
			Monitor::Enter(button5);
			Monitor::Enter(button6);
			Monitor::Enter(button7);
			Monitor::Enter(button8);
			Monitor::Enter(button9);
		
			
			while (FormThread->IsAlive)
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


			    //y tambien para ver quien gano *emoticono de la luna negra*
				CheckWinner();
				// yyyy tambien preguntamos el byte de status haber que hacer respecto
				if (ExtraData[3] == 1) //Some one did a rage quit
				{
					MessageBox::Show("Tu Adversario ha abandonado la partida :(");
					CleanUp();
					ExtraData[3] = 0;
				}
				
				//Labels 
				label2->Text = String::Concat("Jugador1 pts: ", ExtraData[1].ToString());
				label3->Text = String::Concat("Jugador2 pts : ", ExtraData[2].ToString());
				label4->Text = String::Concat("Turn: ", ExtraData[0].ToString());
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

			Monitor::Exit(ExtraData);
			Monitor::Exit(label2);
			Monitor::Exit(label3);
			Monitor::Exit(label4);
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
				return true;
			else if (Tablero[3] == PlayerToken && Tablero[4] == PlayerToken && Tablero[5] == PlayerToken)
				return true;
			else if (Tablero[6] == PlayerToken && Tablero[7] == PlayerToken && Tablero[8] == PlayerToken)
				return true;
			//vertical
			else if (Tablero[0] == PlayerToken && Tablero[3] == PlayerToken && Tablero[6] == PlayerToken)
				return true;

			else if (Tablero[1] == PlayerToken && Tablero[4] == PlayerToken && Tablero[7] == PlayerToken)
				return true;
			else if (Tablero[2] == PlayerToken && Tablero[5] == PlayerToken && Tablero[8] == PlayerToken)
				return true;
			//Diagonal IZQ
			else if (Tablero[6] == PlayerToken && Tablero[4] == PlayerToken && Tablero[2] == PlayerToken)
				return true;
			//Diagonal Derecha
			else if (Tablero[0] == PlayerToken && Tablero[4] == PlayerToken && Tablero[8] == PlayerToken)
				return true;
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
			button10->Enabled = true;
			button11->Enabled = true;
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
				FormThread->Abort();
			}
		}

	protected:
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label1;
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
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
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
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->button12 = (gcnew System::Windows::Forms::Button());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(208, 215);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(66, 13);
			this->label3->TabIndex = 30;
			this->label3->Text = L"Player2Wins";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(208, 201);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(66, 13);
			this->label2->TabIndex = 29;
			this->label2->Text = L"Player1Wins";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->ImageAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->label1->Location = System::Drawing::Point(12, 257);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(44, 13);
			this->label1->TabIndex = 28;
			this->label1->Text = L"status...";
			// 
			// button11
			// 
			this->button11->Location = System::Drawing::Point(210, 43);
			this->button11->Name = L"button11";
			this->button11->Size = System::Drawing::Size(64, 23);
			this->button11->TabIndex = 27;
			this->button11->Text = L"Conectar";
			this->button11->UseVisualStyleBackColor = true;
			this->button11->Click += gcnew System::EventHandler(this, &_GTO::button11_Click);
			// 
			// button10
			// 
			this->button10->Location = System::Drawing::Point(210, 14);
			this->button10->Name = L"button10";
			this->button10->Size = System::Drawing::Size(64, 23);
			this->button10->TabIndex = 25;
			this->button10->Text = L"Host";
			this->button10->UseVisualStyleBackColor = true;
			this->button10->Click += gcnew System::EventHandler(this, &_GTO::button10_Click);
			// 
			// button13
			// 
			this->button13->Location = System::Drawing::Point(210, 101);
			this->button13->Name = L"button13";
			this->button13->Size = System::Drawing::Size(64, 23);
			this->button13->TabIndex = 32;
			this->button13->Text = L"Salir";
			this->button13->UseVisualStyleBackColor = true;
			this->button13->Click += gcnew System::EventHandler(this, &_GTO::button13_Click);
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(12, 12);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(60, 68);
			this->button1->TabIndex = 16;
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &_GTO::button1_Click);
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(11, 86);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(60, 68);
			this->button2->TabIndex = 17;
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &_GTO::button2_Click);
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(12, 160);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(60, 68);
			this->button3->TabIndex = 18;
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &_GTO::button3_Click);
			// 
			// button6
			// 
			this->button6->Location = System::Drawing::Point(78, 12);
			this->button6->Name = L"button6";
			this->button6->Size = System::Drawing::Size(60, 68);
			this->button6->TabIndex = 19;
			this->button6->UseVisualStyleBackColor = true;
			this->button6->Click += gcnew System::EventHandler(this, &_GTO::button6_Click);
			// 
			// button5
			// 
			this->button5->Location = System::Drawing::Point(77, 86);
			this->button5->Name = L"button5";
			this->button5->Size = System::Drawing::Size(60, 68);
			this->button5->TabIndex = 20;
			this->button5->UseVisualStyleBackColor = true;
			this->button5->Click += gcnew System::EventHandler(this, &_GTO::button5_Click);
			// 
			// button4
			// 
			this->button4->Location = System::Drawing::Point(78, 160);
			this->button4->Name = L"button4";
			this->button4->Size = System::Drawing::Size(60, 68);
			this->button4->TabIndex = 21;
			this->button4->UseVisualStyleBackColor = true;
			this->button4->Click += gcnew System::EventHandler(this, &_GTO::button4_Click);
			// 
			// button9
			// 
			this->button9->Location = System::Drawing::Point(144, 12);
			this->button9->Name = L"button9";
			this->button9->Size = System::Drawing::Size(60, 68);
			this->button9->TabIndex = 22;
			this->button9->UseVisualStyleBackColor = true;
			this->button9->Click += gcnew System::EventHandler(this, &_GTO::button9_Click);
			// 
			// button8
			// 
			this->button8->Location = System::Drawing::Point(143, 86);
			this->button8->Name = L"button8";
			this->button8->Size = System::Drawing::Size(60, 68);
			this->button8->TabIndex = 23;
			this->button8->UseVisualStyleBackColor = true;
			this->button8->Click += gcnew System::EventHandler(this, &_GTO::button8_Click);
			// 
			// button7
			// 
			this->button7->Location = System::Drawing::Point(144, 160);
			this->button7->Name = L"button7";
			this->button7->Size = System::Drawing::Size(60, 68);
			this->button7->TabIndex = 24;
			this->button7->UseVisualStyleBackColor = true;
			this->button7->Click += gcnew System::EventHandler(this, &_GTO::button7_Click);
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(207, 188);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(32, 13);
			this->label4->TabIndex = 33;
			this->label4->Text = L"Turn:";
			// 
			// button12
			// 
			this->button12->Location = System::Drawing::Point(211, 73);
			this->button12->Name = L"button12";
			this->button12->Size = System::Drawing::Size(63, 23);
			this->button12->TabIndex = 34;
			this->button12->Text = L"disconect";
			this->button12->UseVisualStyleBackColor = true;
			this->button12->Click += gcnew System::EventHandler(this, &_GTO::button12_Click);
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->ImageAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->label5->Location = System::Drawing::Point(12, 231);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(79, 13);
			this->label5->TabIndex = 35;
			this->label5->Text = L"Bytes enviados";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->ImageAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->label6->Location = System::Drawing::Point(12, 244);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(77, 13);
			this->label6->TabIndex = 36;
			this->label6->Text = L"bytes recividos";
			// 
			// _GTO
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(283, 301);
			this->ControlBox = false;
			this->Controls->Add(this->label6);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->button12);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->button13);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
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
			this->Text = L"_GTO";
			this->Load += gcnew System::EventHandler(this, &_GTO::_GTO_Load);
			this->ResumeLayout(false);
			this->PerformLayout();

		}

#pragma endregion


	private: System::Void _GTO_Load(System::Object^  sender, System::EventArgs^  e)
	{
		CheckForIllegalCrossThreadCalls = false; //alv usamos esta madre pa que funcione
		FormThread = gcnew Thread(gcnew ThreadStart(this,&_GTO::UpdateForms)); //Initialize the form updater thread
	    NetworkThread  = gcnew Thread(gcnew ThreadStart(this, &_GTO::HoldForData)); //Initialize the NetworkThread
		FormThread->Start();//inicializamos este thread ya que solo es para actualizar lo que se ve en pantalla
	}

	//Host Button
	private: System::Void button10_Click(System::Object^  sender, System::EventArgs^  e)
	{
		try
		{
			label1->Text = "Creating Server...";
			_Socket = gcnew Socket(AddressFamily::InterNetwork, SocketType::Stream, ProtocolType::Tcp);
			_Socket->Bind(gcnew IPEndPoint(0, 5000));
			_Socket->Listen(0);
			Cliente = _Socket->Accept();
			if (Cliente != nullptr)
			{
				label1->Text = "MatchCreated";
				IsServer = true;
				player = 'X';
				GameReady = true;
				//Recive the message

				//Esperamos a que llegue un mensaje de prueba
				if (Cliente->Available > 0)
				{

					array<Byte> ^data = gcnew array<Byte>(1);
					int bytesS = Cliente->Receive(data, 1, SocketFlags::None);
					label6->Text = bytesS.ToString();
					if (data[0] == 43)
					label1->Text = "Host";

					NetworkThread->Start();
				}

				button10->Enabled = false;
				button11->Enabled = false;


			}
		}
		catch (System::Net::Sockets::SocketException^ error)
		{
			label1->Text = String::Concat("Error:", error->ToString());
		}
	}
			 //Conect button
	private: System::Void button11_Click(System::Object^  sender, System::EventArgs^  e)
	{
		try
		{
			Cliente = gcnew Socket(AddressFamily::InterNetwork, SocketType::Stream, ProtocolType::Tcp);
			Cliente->Connect(gcnew IPEndPoint(IPAddress::Parse("127.0.0.1"), 5000)); //TODO: cambiar esto por el campo de un textbox para la ip
			if (Cliente->IsBound)
			{
				label1->Text = String::Concat("Conected to:", Cliente->RemoteEndPoint->ToString());
				GameReady = true;
				player = 'O';
				//mandamos un mensaje
				array<Byte> ^data = gcnew array<Byte>(1);
				data[0] = 43;//Initial Network Code
				int bytesS = Cliente->Send(data, 1, SocketFlags::None);

				if (bytesS > 0)
				{
					label1->Text = "Client";
				}
				NetworkThread->Start();
				label5->Text = bytesS.ToString();
				button10->Enabled = false;
				button11->Enabled = false;
			}
		}
		catch (System::Net::Sockets::SocketException^ error)
		{
			label1->Text = String::Concat("Error:", error->ToString());
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
		ExtraData[3] = 1;
		Send();
		CleanUp();
	}
};
}