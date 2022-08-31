
#include "comun.h"
#include "tipos.h"
#include "barquito.h"
#include "sobre.h"

class TPanel : public TWindow {
  public:
	 TPanel();
	 ~TPanel();

  private:
	 TWindow		 *Padre; 
	 HFONT 		 hFont1;	
    HFONT		 hFont2; 

	 TBitmap		 *Bmp[7]; /
    TipoTablero *Jugador, *Ordenador, *Trabajo, *Trabajo2, *Mapa;
	 int			 Raton;	 
	 int			 Barco[2][4]; 
    								  
	 bool			 BarSel; 
	 int			 Turno;  
  
	 int			 Tocados; 
	 int			 Estrategia; 
	 int			 Resto; 
	 int			 EstadoOrdenador; 
	 int 			 t;	 
	 TipoCoor	 Coor[4]; 

	 TButton 	 *btOk;	
	 TRect		 RectBarcos[10]; 
	 int			 Lon[10]; 
    bool			 Ori[10]; 
    bool 		 BOri;    
    int 			 BSel, Xx, Yy, Xp, Yp, Op; 
	 TRect		 Re; 
	 int			 Xd, Yd;	
    int			 EstadoJugador;
    TMciWaveAudio* sndExplosion;
    TMciWaveAudio* sndChoff;

  protected:
	 void cmOk();
	 void cmCancel();
	 void cmSobre();
	 void EvPaint();
	 void PintaFicha(TDC& dc, char n, int x, int y);
	 void ColocaBarcos(int X);
	 void SetupWindow();
	 void EvLButtonUp(uint modKeys, TPoint& point);
	 void EvRButtonUp(uint modKeys, TPoint& point);
	 void EvLButtonDown(uint modKeys, TPoint& point);
	 void EvMouseMove(uint modKeys, TPoint& point);
	 void PintaCasilla(int J, int x, int y);
	 bool Hundido(TipoTablero *M, TipoTablero*M2, bool J, int x, int y);
	 void Contador();
	 void EvTimer(UINT);

	 void ProcesaMapa();
	 void ProcesaTocado();
	 void Suerte(int s);
	 void DispararOrdenador(int x, int y);
	 void DispararJugador(int x, int y);
    void Paripe(int j, int x, int y, bool c);
	 void Mensaje(char *Cad);
	 void VerResto();
  public:

  DECLARE_RESPONSE_TABLE(TPanel);
};

DEFINE_RESPONSE_TABLE1(TPanel, TWindow)
	 EV_WM_TIMER,
	 EV_WM_PAINT,
	 EV_WM_LBUTTONUP,
	 EV_WM_RBUTTONUP,
	 EV_WM_LBUTTONDOWN,
	 EV_WM_MOUSEMOVE,
	 EV_WM_CREATE,
	 EV_COMMAND(IDCANCEL, cmCancel),
	 EV_COMMAND(CM_SOBRE, cmSobre),
	 EV_COMMAND(IDOK, cmOk),
END_RESPONSE_TABLE;

TPanel::TPanel(void):TWindow(0, 0, ::Module)
{
	// medidas de la ventana:
	Attr.X = 11;
	Attr.Y = 65;
	Attr.W = 500;
	Attr.H = 380;
	btOk = new TButton(this, IDOK, "Nuevo", 400, 16, 90, 41, TRUE);
	new TButton(this, IDCANCEL, "Salir", 400, 91, 90, 41, FALSE);
	new TButton(this, CM_SOBRE, "&Sobre", 400, 315, 90, 41, FALSE);
	Padre = Parent;
   sndExplosion = new TMciWaveAudio("whoof.wav");
   sndChoff = new TMciWaveAudio("bubble.wav");

  	Bmp[0] = new TBitmap(*GetModule(), TResId(Blanco));
  	Bmp[1] = new TBitmap(*GetModule(), TResId(Rojo));
  	Bmp[2] = new TBitmap(*GetModule(), TResId(Azul));
  	Bmp[3] = new TBitmap(*GetModule(), TResId(Verde));
  	Bmp[4] = new TBitmap(*GetModule(), TResId(Negro));
  	Bmp[5] = new TBitmap(*GetModule(), TResId(Impacto));
  	Bmp[6] = new TBitmap(*GetModule(), TResId(Choff));
	hFont1 = CreateFont(16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Comic Sans MS");
	hFont2 = CreateFont(24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Comic Sans MS");

   Ordenador = ::new TipoTablero;
   Trabajo 	 = ::new TipoTablero;
   Trabajo2  = ::new TipoTablero;
   Mapa		 = ::new TipoTablero;
   Jugador	 = ::new TipoTablero;

	Re.left   = 220;
	Re.top    = 20;
	Re.right  = Re.left + 160;
	Re.bottom = Re.top  + 160;
}

TPanel::~TPanel(void)
{
	KillTimer(1);
	DeleteObject(hFont1);
	DeleteObject(hFont2);
   for(int i = 0; i < 7; i++) delete Bmp[i];
   ::delete Ordenador;
   ::delete Trabajo;
   ::delete Trabajo2;
   ::delete Mapa;
   ::delete Jugador;
   delete sndExplosion;
   delete sndChoff;
}

void TPanel::SetupWindow()
{
	int i, j;

	TWindow::SetupWindow();

	BarSel = false;
	Turno = NADIE;
	Raton = ESPERA;

	for(i = 0; i < 10; i++)
		for(j = 0; j < 10; j++)
		{
			Ordenador->Contenido[i][j] = 0;
			Ordenador->Visto[i][j] = 0;
			Jugador->Contenido[i][j] = 0;
			Jugador->Visto[i][j] = 0;
			Trabajo->Contenido[i][j] = 0;
			Trabajo->Visto[i][j] = 0;
			Trabajo2->Contenido[i][j] = 0;
			Trabajo2->Visto[i][j] = 0;
		}
	for(i = 0; i < 4; i++)
	{
		Barco[0][i] = 0;
		Barco[1][i] = 0;
	}
}

void TPanel::ColocaBarcos(int J)
{
	int i, j, k, l=0;
   bool o;
	int x, y, xa, ya, c;
	int m, n;

	for(j = 4; j >= 1; j--) // longitud de los barcos es j
		for(i = j; i <= 4; i++) // número de barcos de "j" de longitud es i
		{
			do  // repetir hasta encontrar un hueco
			{
				o = (random(2) == HORIZONTAL); // 1-> horizontal, 0-> vertical
				if(o)  // las x varían entre 0 y 10-longitud
            		 // las y varían entre 0 y 9
				{
					x = random(11-j);
					y = random(10);
					xa = 1;			// Dirección de avance
					ya = 0;
				}
				else  // las x varían entre 0 y 9
            		// las y varían entre 0 y 10-longitud
				{
					x = random(10);
					y = random(11-j);
					xa = 0;
					ya = 1;       // Dirección de avance
				}
				c = 0;
				for(k = 0; k < j; k++) // para toda la longitud del barco
            							  // Comprobar si hay algún barco cerca
					if(J) c +=   Jugador->Contenido[x+xa*k][y+ya*k];
					else  c += Ordenador->Contenido[x+xa*k][y+ya*k];
			}
			while(c); // repetir mientras el hueco no sirva para el barco
			// Variables para edición de posición:
			if(J)
			{
         	// Asigna el rectángulo y longitud de cada barco del jugador
				RectBarcos[l].Set(20+x*16, 20+y*16, 35+(x+xa*(j-1))*16, 35+(y+ya*(j-1))*16);
				Lon[l] = j;
            Ori[l] = o;
				l++;
			}
			for(k = 0; k < j; k++)
				if(J) // Jugador
				{
					// Prohibir casillas alrededor del barco
					for(n = -1; n < 2; n++)
						for(m = -1; m < 2; m++)
							if(x+xa*k+n >= 0 && x+xa*k+n < 10 &&
								y+ya*k+m >= 0 && y+ya*k+m < 10)
								if(!Jugador->Contenido[x+xa*k+n][y+ya*k+m])
									Jugador->Contenido[x+xa*k+n][y+ya*k+m] = 1;
					// Colocar sección de barco:
					Jugador->Contenido[x+xa*k][y+ya*k] = (char)(2*(j+1)+o);
				}
				else // Ordenador
				{
					// Prohibir casillas alrededor del barco
					for(n = -1; n < 2; n++)
						for(m = -1; m < 2; m++)
							if(x+xa*k+n >= 0 && x+xa*k+n < 10 &&
								y+ya*k+m >= 0 && y+ya*k+m < 10)
								if(!Ordenador->Contenido[x+xa*k+n][y+ya*k+m])
									Ordenador->Contenido[x+xa*k+n][y+ya*k+m] = 1;
					// Colocar sección de barco:
					Ordenador->Contenido[x+xa*k][y+ya*k] = (char)(2*(j+1)+o);
				}
		}
}

void TPanel::cmOk()
{
	int i, j;

	if(Raton == EDICION)
	{
		for(i = 0; i < 10; i++)
			for(j = 0; j < 10; j++)
			{
         	if(Trabajo2->Contenido[i][j] > 2)
            	Jugador->Contenido[i][j] = Trabajo2->Contenido[i][j];
				else
            	Jugador->Contenido[i][j] = 0;
         	Jugador->Visto[i][j] = 0; //Trabajo2->Visto[i][j];
           	Trabajo2->Contenido[i][j] = Jugador->Contenido[i][j];
         }
      btOk->SetCaption("Detener");
		Raton = JUEGO;
		Turno = random(2);
		if(!SetTimer(1, 500U, 0))
			MessageBox("Out of Timers", GetApplication()->GetName(), MB_ICONEXCLAMATION|MB_OK);
		return;
	}

  	if(Raton == JUEGO)
   {
   	KillTimer(1);
      Raton = ESPERA;
	   btOk->SetCaption("Nuevo");
      return;
	}
   for(i = 0; i < 10; i++)
		for(j = 0; j < 10; j++)
		{
			Ordenador->Contenido[i][j] = 0;
			Ordenador->Visto[i][j] = 0;
			Jugador->Contenido[i][j] = 0;
			Jugador->Visto[i][j] = 0;
			Trabajo->Contenido[i][j] = 0;
			Trabajo->Visto[i][j] = 0;
			Trabajo2->Contenido[i][j] = 0;
			Trabajo2->Visto[i][j] = 0;
		}
	t = random(0x8000);
	srand(t);
	// Ordenador:
	ColocaBarcos(ORDENADOR);
	Resto = 100;
	Estrategia = BUSCA;
	EstadoOrdenador = AVISO;
	EstadoJugador = AVISO;
	Tocados = 0;
	for(i = 0; i < 4; i++)
	{
		Barco[0][i] = 0;
		Barco[1][i] = 0;
	}
	// Jugador:
	ColocaBarcos(JUGADOR);
	if(MessageBox("¿Quieres colocar tus barcos?.", "Iniciando partida", MB_ICONQUESTION | MB_YESNO) == IDYES)
	{
   	Raton = EDICION;
      btOk->SetCaption("Comenzar");
	}
   else
	{
		Raton = JUEGO;
		Turno = random(2);
		if(!SetTimer(1, 500U, 0))
			MessageBox("Out of Timers", GetApplication()->GetName(),
					MB_ICONEXCLAMATION|MB_OK);
      btOk->SetCaption("Detener");
	}

	for(i = 0; i < 10; i++)
		for(j = 0; j < 10; j++)
			if(Jugador->Contenido[i][j] > 2)
         	Trabajo2->Contenido[i][j] = Jugador->Contenido[i][j];

	// Blanquear, poner el mapa a cero, salvo donde hay barcos:
	for(i = 0; i < 10; i++)
		for(j = 0; j < 10; j++)
      {
      	if(Jugador->Contenido[i][j] < 3) Jugador->Contenido[i][j] = 0;
			if(Ordenador->Contenido[i][j] < 3) Ordenador->Contenido[i][j] = 0;
		}

	Invalidate();
}

void TPanel::EvTimer(UINT)
{
   char Cad[40];

	KillTimer(1);
   switch(Turno)
	{
   	case ORDENADOR:
			switch(EstadoOrdenador)
		 	{
				case AVISO:
					Mensaje("MI TURNO");
					EstadoOrdenador++;
					break;
				case ANALISIS:
					//t++;
					if(!Estrategia)  // Buscar
						ProcesaMapa();
					else
						ProcesaTocado();
					break;
				case PREPARADO:
               Paripe(ORDENADOR, Xd, Yd, true);
					sprintf(Cad, "Coordenadas %c%d", 'A'+Xd, Yd+1);
					Mensaje(Cad);
               EstadoOrdenador++;
					break;
				case DISPARO:
					// disparar en (x,y)
					DispararOrdenador(Xd, Yd);
               Paripe(ORDENADOR, Xd, Yd, false);
					EstadoOrdenador = AVISO;
					break;
			}
         break;
      case JUGADOR:
			switch(EstadoJugador)
         {
         	case AVISO:
					Mensaje("TU TURNO");
               EstadoJugador++;
           	   SetCursor(GetModule(), Disparo);
               break;
            case ANALISIS:
            	// Lee coordenadas del ratón
            	break;
            case PREPARADO:
               Paripe(JUGADOR, Xd, Yd, true);
					sprintf(Cad, "Coordenadas %c%d", 'A'+Xd, Yd+1);
               Mensaje(Cad);
               EstadoJugador++;
               break;
            case DISPARO:
					DispararJugador(Xd, Yd);
               Paripe(JUGADOR, Xd, Yd, false);
			   	SetCursor(GetModule(), IDC_ARROW);
					EstadoJugador = AVISO;
            	break;
         }
         break;
		case NADIE:
      	Raton = ESPERA;
			Mensaje("FIN DE JUEGO");
         break;
   }
   if(Turno != NADIE) SetTimer(1, 500U, 0);
}

// Botón izquierdo del ratón, pinchar o soltar barco
void TPanel::EvLButtonDown(uint, TPoint& point) // Botón izquierdo
{
	int i, j, xa=0, ya=0;
	int x, y;
	TRect re;
	TRect r;
	TClientDC dc(*this);

	if(Raton == JUEGO) return; // Si estamos jugando, ignorar
   if(BarSel) // Si estamos editando, y hay un barco seleccionado, colocarlo
	{
		// Dirección del barco:
		if(BOri) xa = 1; else ya = 1;
		// Vericar si hay hueco libre:
		re.Set(20, 20, 179-(Lon[BSel]-1)*16*xa, 179-(Lon[BSel]-1)*16*ya);

		if(re.Contains(point))
		{
			x = (point.x - 20)>>4;
			y = (point.y - 20)>>4;
			r.Set(15+16*x, 15+16*y, 41+(x+xa*(Lon[BSel]-1))*16, 41+(y+ya*(Lon[BSel]-1))*16);
			for(i = 0; i < 10; i++) if(i != BSel && RectBarcos[i].Touches(r)) return;
			// Borrar el anterior:
			for(i = 0; i < Lon[BSel]; i++)
				if(Xx+i*xa <10 && Yy+i*ya < 10)
					PintaFicha(dc, Trabajo2->Contenido[Xx+i*xa][Yy+i*ya], 20+(Xx+i*xa)*16, 20+(Yy+i*ya)*16);
			// Pintar el nuevo:
			for(i = 0; i < Lon[BSel]; i++)
				PintaFicha(dc, NEGRO, 20+(x+i*xa)*16, 20+(y+i*ya)*16);
			Xx = x;
			Yy = y;

	      // Colocar barco:
			for(j = 0; j < Lon[BSel]; j++)
			{
				PintaFicha(dc, NEGRO, 20+(Xx+j*xa)*16, 20+(Yy+j*ya)*16);
				Trabajo2->Contenido[Xx+j*xa][Yy+j*ya] = (char)(2*(Lon[BSel]+1)+xa);
   	      Ori[BSel] = BOri;
			}
			RectBarcos[BSel].Set(20+16*Xx, 20+16*Yy, 35+(Xx+xa*(Lon[BSel]-1))*16, 35+(Yy+ya*(Lon[BSel]-1))*16);
			BarSel = false;
		}
   }
   else
   {
	   for(i = 0; i < 10; i++)
			if(RectBarcos[i].Contains(point))
			{
				BSel = i;
				Xx = (RectBarcos[i].left-20)>>4;
				Yy = (RectBarcos[i].top-20) >>4;
				r.Set(15+16*Xx, 15+16*Yy, 25+(Xx+Lon[BSel]-1)*16, 41+Yy*16);
				for(j = 0; j < 10; j++) if(j != BSel && RectBarcos[j].Touches(r)) return;
				if(Trabajo2->Contenido[Xx][Yy] %2) xa = 1; else ya = 1;
				Xp = Xx;
				Yp = Yy;
            BOri = Ori[i];
				BarSel = true;
				// Dirección del barco:
				if(BOri) xa = 1; else ya = 1;
				for(j = 0; j < Lon[i]; j++)
				{
					PintaFicha(dc, VERDE, 20+(Xx+xa*j)*16, 20+(Yy+ya*j)*16);
					Trabajo2->Contenido[Xx+xa*j][Yy+ya*j] = 0;
				}
            // Marcar casillas posibles:
            for(i = 0; i < 10; i++)
            	for(j = 0; j < 10; j++)
               	if(Trabajo2->Contenido[i][j] < 3) Trabajo2->Contenido[i][j] = 0;
			   for(i = 0; i < 10; i++) // Para todos los barcos menos el editado:
            	if(i != BSel)
               {
						x = (RectBarcos[i].left-20)>>4;
						y = (RectBarcos[i].top-20) >>4;
                  if(Ori[i])
                  {
                  	if(x > 0) Trabajo2->Contenido[x-1][y] = 1;
                  	if(x+Lon[i] < 10) Trabajo2->Contenido[x+Lon[i]][y] = 1;
	   		         for(j = -1; j <= Lon[i]; j++)
                     	if(x+j >= 0 && x+j < 10)
		                  {
                           if(y > 0) Trabajo2->Contenido[x+j][y-1] = 1;
                           if(y < 9) Trabajo2->Contenido[x+j][y+1] = 1;
                        }
                  }
                  else
                  {
                  	if(y > 0) Trabajo2->Contenido[x][y-1] = 1;
                  	if(y+Lon[i] < 10) Trabajo2->Contenido[x][y+Lon[i]] = 1;
	   		         for(j = -1; j <= Lon[i]; j++)
                     	if(y+j >= 0 && y+j < 10)
		                  {
                           if(x > 0) Trabajo2->Contenido[x-1][y+j] = 1;
                           if(x < 9) Trabajo2->Contenido[x+1][y+j] = 1;
                        }
                  }
               }
				break;
			}
   }
}

void TPanel::EvMouseMove(uint, TPoint& point)
{
	int x, y, xa, ya, i;
	TRect re, r;
	TClientDC dc(*this);

	if(Raton == JUEGO || !BarSel) return;

	xa = 0; ya = 0;
	if(BOri) xa = 1; else ya = 1;
	re.Set(20, 20, 179-(Lon[BSel]-1)*16*xa, 179-(Lon[BSel]-1)*16*ya);

	if(re.Contains(point))
	{
		x = (point.x - 20)>>4;
		y = (point.y - 20)>>4;
		r.Set(15+16*x, 15+16*y, 41+(x+xa*(Lon[BSel]-1))*16, 41+(y+ya*(Lon[BSel]-1))*16);
		// Borrar el anterior:
		for(i = 0; i < Lon[BSel]; i++)
			if(Xx+i*xa <10 && Yy+i*ya < 10)
         	if(Trabajo2->Contenido[Xx+i*xa][Yy+i*ya] > 2)
					PintaFicha(dc, NEGRO, 20+(Xx+i*xa)*16, 20+(Yy+i*ya)*16);
            else
					PintaFicha(dc, BLANCO, 20+(Xx+i*xa)*16, 20+(Yy+i*ya)*16);
		// Pintar el nuevo:
		for(i = 0; i < Lon[BSel]; i++)
			if(Trabajo2->Contenido[x+i*xa][y+i*ya] == 0)
				PintaFicha(dc, VERDE, 20+(x+i*xa)*16, 20+(y+i*ya)*16);
			else
         	PintaFicha(dc, ROJO, 20+(x+i*xa)*16, 20+(y+i*ya)*16);
		Xx = x;
		Yy = y;
	}
}

void TPanel::EvLButtonUp(uint, TPoint& point)
{
	TRect re;
	TClientDC dc(*this);

	if(Raton == EDICION) return;
	if(Turno != JUGADOR || EstadoJugador != ANALISIS) return;
	if(Re.Contains(point))
	{
		Xd = (point.x - Re.left)>>4;
		Yd = (point.y - Re.top )>>4;
		// Disparo anterior:
		if(Ordenador->Visto[Xd][Yd]) return;
		EstadoJugador++;
   }
}

void TPanel::EvRButtonUp(uint, TPoint& point)
{
	int x, y, xa, ya, i;
	TRect re, r;
	TClientDC dc(*this);

	if(Raton == JUEGO || !BarSel) return;

	xa = 0; ya = 0;
   BOri = !BOri;
	if(BOri) xa = 1; else ya = 1;
	re.Set(20, 20, 179-(Lon[BSel]-1)*16*xa, 179-(Lon[BSel]-1)*16*ya);

	// Borrar el anterior:
	for(i = 0; i < Lon[BSel]; i++)
		if(Xx+i*ya <10 && Yy+i*xa < 10)
        	if(Trabajo2->Contenido[Xx+i*ya][Yy+i*xa] > 2)
				PintaFicha(dc, NEGRO, 20+(Xx+i*ya)*16, 20+(Yy+i*xa)*16);
         else
				PintaFicha(dc, BLANCO, 20+(Xx+i*ya)*16, 20+(Yy+i*xa)*16);

	if(re.Contains(point))
	{
		x = (point.x - 20)>>4;
		y = (point.y - 20)>>4;
		r.Set(15+16*x, 15+16*y, 41+(x+xa*(Lon[BSel]-1))*16, 41+(y+ya*(Lon[BSel]-1))*16);
		// Pintar el nuevo:
		for(i = 0; i < Lon[BSel]; i++)
			if(Trabajo2->Contenido[x+i*xa][y+i*ya] == 0)
				PintaFicha(dc, VERDE, 20+(x+i*xa)*16, 20+(y+i*ya)*16);
			else
         	PintaFicha(dc, ROJO, 20+(x+i*xa)*16, 20+(y+i*ya)*16);
		Xx = x;
		Yy = y;
	}
}

bool TPanel::Hundido(TipoTablero *M, TipoTablero *M2, bool J, int x, int y)
{
	int o, c, ax, ay, m, n;

	// Verificar Hundido:
	o = M->Contenido[x][y] % 2;
	c = 0;
	ax = 0;
	ay = 0;
	if(o) ax = 1; else ay = 1;
	n = x;
	m = y;
	while(M->Contenido[n][m])
	{
		n -=ax;
		m -=ay;
		if(n < 0 || m < 0) break;
	}
	n +=ax;
	m +=ay;
	while(M->Contenido[n][m])
	{
		if(!M->Visto[n][m]) c++;
		n +=ax;
		m +=ay;
		if(n >= 10 || m >= 10) break;
	}
	if(!c) // Hundido depurar
	{
		n = x;
		m = y;
		while(M->Contenido[n][m])
		{
			n -=ax;
			m -=ay;
			if(n < 0 || m < 0) break;
		}
		n +=ax;
		m +=ay;
		while(M->Contenido[n][m] && n < 10 && m < 10)
		{
			if(n > 0) if(!M->Contenido[n-1][m])
			{
				if(J && !M->Visto[n-1][m]) Resto--;
				M2->Contenido[n-1][m] = AZUL;
				M2->Visto[n-1][m] = 1;
				M->Visto[n-1][m] = 1;
				PintaCasilla(J, n-1, m);
			}
			if(n < 9) if(!M->Contenido[n+1][m])
			{
				if(J && !M->Visto[n+1][m]) Resto--;
				M2->Contenido[n+1][m] = AZUL;
				M2->Visto[n+1][m] = 1;
				M->Visto[n+1][m] = 1;
				PintaCasilla(J, n+1, m);
			}
			if(m > 0) if(!M->Contenido[n][m-1])
			{
				if(J && !M->Visto[n][m-1]) Resto--;
				M2->Contenido[n][m-1] = AZUL;
				M2->Visto[n][m-1] = 1;
				M->Visto[n][m-1] = 1;
				PintaCasilla(J, n, m-1);
			}
			if(m < 9) if(!M->Contenido[n][m+1])
			{
				if(J && !M->Visto[n][m+1]) Resto--;
				M2->Contenido[n][m+1] = AZUL;
				M2->Visto[n][m+1] = 1;
				M->Visto[n][m+1] = 1;
				PintaCasilla(J, n, m+1);
			}
			n +=ax;
			m +=ay;
			if(n >= 10 || m >= 10) break;
		}
		return true;
	}
	return false;
}

void TPanel::Contador()
{
	int i, j, n;
	TClientDC dc(*this);

	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < Barco[0][i]; j++)
			for(n = 0; n <= i; n++) PintaFicha(dc, ROJO, 20+(n+j*(i+2))*16, 220+32*i);
		for(j = Barco[0][i]; j < 4-i; j++)
			for(n = 0; n <= i; n++) PintaFicha(dc, NEGRO, 20+(n+j*(i+2))*16, 220+32*i);

		for(j = 0; j < Barco[1][i]; j++)
			for(n = 0; n <= i; n++) PintaFicha(dc, ROJO, 220+(n+j*(i+2))*16, 220+32*i);
		for(j = Barco[1][i]; j < 4-i; j++)
			for(n = 0; n <= i; n++) PintaFicha(dc, NEGRO, 220+(n+j*(i+2))*16, 220+32*i);
	}
}

void TPanel::PintaCasilla(int J, int x, int y)
{
	 TClientDC dc(*this);

	 if(J)
		 PintaFicha(dc, Trabajo2->Contenido[x][y], 20+x*16, 20+y*16);
	 else
		 PintaFicha(dc, Trabajo->Contenido[x][y], 220+x*16, 20+y*16);
}

void TPanel::PintaFicha(TDC& dc, char n, int x, int y)
{
  int i;

  if(n < 0 || n > 3) i = 4; else i = n;
  if(Bmp[i])
  {
		TMemoryDC memDC;
		memDC.SelectObject(*Bmp[i]);
		dc.BitBlt(x, y, 16, 16, memDC, 0, 0);
  }
}

void TPanel::VerResto()
{
	int x, y;
	TClientDC dc(*this);

	for(x = 0; x < 10; x++)
		for(y = 0; y < 10; y++)
		{
			if(!Ordenador->Visto[x][y] && Ordenador->Contenido[x][y])
			{
				PintaFicha(dc, VERDE, 220+x*16, 20+y*16);
				Trabajo->Contenido[x][y] = VERDE;
			}
			if(!Jugador->Visto[x][y] && Jugador->Contenido[x][y])
			{
				PintaFicha(dc, VERDE, 20+x*16, 20+y*16);
				Trabajo2->Contenido[x][y] = VERDE;
			}
		}
}

void TPanel::Mensaje(char *Cad)
{
  TClientDC dc(*this);
  TRect	re(10, 185, 300, 212);

  dc.SetBkMode(TRANSPARENT);
  dc.SelectObject(hFont2);
  dc.SetTextColor(RGB(0, 0, 0));
  dc.Rectangle(re);
  dc.TextOut( 30, 187, Cad, strlen(Cad));
}

void TPanel::EvPaint()
{
   TPaintDC	dc(*this);
   int i, j, n;
   TRect re;
   char cad[15];

   HBRUSH Fondo = (HBRUSH)GetStockObject(LTGRAY_BRUSH);


   GetClientRect(re);
   dc.FillRect(re, Fondo);

	dc.SetBkMode(TRANSPARENT);
   dc.SelectObject(hFont1);
   dc.SetTextColor(RGB(0, 0, 0));

   // Coordenadas A, B, C... 1, 2, 3...
	for(i = 0; i < 10; i++)
	{
		cad[0] = (char)('A'+i);
		dc.TextOut( 22+16*i, 4, cad, 1);
		dc.TextOut(222+16*i, 4, cad, 1);
		sprintf(cad, "%2d", 1+i);
		dc.TextOut(   3, 20+i*16, cad, strlen(cad));
		dc.TextOut( 203, 20+i*16, cad, strlen(cad));
		for(j = 0; j < 10; j++)
		{
			PintaFicha(dc, Trabajo2->Contenido[i][j],  20+i*16,  20+j*16);
			PintaFicha(dc, Trabajo->Contenido[i][j],  220+i*16,  20+j*16);
		}
	}
   // Diagramas de barcos:
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < Barco[0][i]; j++)
			for(n = 0; n <= i; n++) PintaFicha(dc, ROJO, 20+(n+j*(i+2))*16, 220+32*i);
		for(j = Barco[0][i]; j < 4-i; j++)
			for(n = 0; n <= i; n++) PintaFicha(dc, NEGRO, 20+(n+j*(i+2))*16, 220+32*i);

		for(j = 0; j < Barco[1][i]; j++)
			for(n = 0; n <= i; n++) PintaFicha(dc, ROJO, 220+(n+j*(i+2))*16, 220+32*i);
		for(j = Barco[1][i]; j < 4-i; j++)
			for(n = 0; n <= i; n++) PintaFicha(dc, NEGRO, 220+(n+j*(i+2))*16, 220+32*i);
	}
}

void TPanel::Paripe(int J, int x, int y, bool c)
{
   char cad[15];
	TMemoryDC memDC;
   TClientDC dc(*this);

	dc.SetBkMode(TRANSPARENT);
   dc.SelectObject(hFont1);
   if(c) dc.SetTextColor(RGB(255,0,0));
   else  dc.SetTextColor(RGB(0,0,0));

	cad[1] = 0;
	cad[0] = (char)('A'+x);
	if(J == ORDENADOR) dc.TextOut( 22+16*x, 4, cad, 1);
	else 					 dc.TextOut(222+16*x, 4, cad, 1);
	sprintf(cad, "%2d", 1+y);
	if(J == ORDENADOR) dc.TextOut(   3, 20+y*16, cad, strlen(cad));
	else					 dc.TextOut( 203, 20+y*16, cad, strlen(cad));

	if(c)
   {
   	if((Trabajo2->Contenido[x][y] && J == ORDENADOR) ||
         (Ordenador->Contenido[x][y] && J == JUGADOR))
		{
      	memDC.SelectObject(*Bmp[IMPACTO]);
		   if(sndExplosion) sndExplosion->Play(MCI_NOTIFY);
      }
      else
		{
			memDC.SelectObject(*Bmp[CHOFF]);
		   if(sndChoff) sndChoff->Play(MCI_NOTIFY);
		}
      if(J == ORDENADOR) dc.BitBlt( 20+x*16,  20+y*16, 16, 16, memDC, 0, 0);
		else					 dc.BitBlt(220+x*16,  20+y*16, 16, 16, memDC, 0, 0);
   }
}

void TPanel::cmSobre()
{
	TSobre(this).Execute();
}

void TPanel::cmCancel()
{
	Destroy(IDOK);
}

// Parte de pensar:
void TPanel::Suerte(int s)
{
	int i;

	i = random(s);
	Xd = 0;
	Yd = 0;
	while(i)
	{
		if(!Mapa->Visto[Xd][Yd]) i--;
		Xd++;
		if(Xd == 10)
		{
			Xd = 0;
			Yd++;
		}
	}
	while(Mapa->Visto[Xd][Yd])
	{
		Xd++;
		if(Xd == 10)
		{
			Xd = 0;
			Yd++;
		}
	}
}

void TPanel::ProcesaMapa()
{
	int x, y, lm, Xa, Ya, Xb, Yb, n, m = 0;

	// a) Criba eliminar casillas donde no es posible que quepa
	//    el barco mayor que queda por hundir:
	for(lm = 3; lm >= 0; lm--)
		if(Barco[0][lm] < 4-lm) break;

	if(lm) // si queda algo más que submarinos
	{
		for(x = 0; x < 10; x++)
			for(y = 0; y < 10; y++)
			{
				Mapa->Contenido[x][y] = 0;
				if(!Jugador->Visto[x][y])
				{
					// Caso horizontal:
					Xa = x;
					Xb = x;
					while(Xa > 0 && !Jugador->Visto[Xa-1][y] && x-Xa < lm) Xa--;
					while(Xb < 9 && !Jugador->Visto[Xb+1][y] && Xb-x < lm) Xb++;
					Ya = y;
					Yb = y;
					while(Ya > 0 && !Jugador->Visto[x][Ya-1] && y-Ya < lm) Ya--;
					while(Yb < 9 && !Jugador->Visto[x][Yb+1] && Yb-y < lm) Yb++;
					if(Xb-Xa >= lm) Mapa->Contenido[x][y] = (char)(Xb-Xa-lm+1);
					if(Yb-Ya >= lm) Mapa->Contenido[x][y] += (char)(Yb-Ya-lm+1);
					m = max((char)m, Mapa->Contenido[x][y]);
				}
			}
		// Mapa hecho, valor máximo m, elegir entre los de valor m
		// optimización: elegir el que tenga más esquinas libres:
		n = 0;
		for(x = 0; x < 10; x++)
			for(y = 0; y < 10; y++)
			{
				if(Mapa->Contenido[x][y] == m)
				{
					// Contar esquinas:
					if(x > 0 && y > 0) if(!Jugador->Visto[x-1][y-1]) Mapa->Contenido[x][y]++;
					if(x < 9 && y > 0) if(!Jugador->Visto[x+1][y-1]) Mapa->Contenido[x][y]++;
					if(x > 0 && y < 9) if(!Jugador->Visto[x-1][y+1]) Mapa->Contenido[x][y]++;
					if(x < 9 && y < 9) if(!Jugador->Visto[x+1][y+1]) Mapa->Contenido[x][y]++;
					n = max((char)n, Mapa->Contenido[x][y]);
				}
				//else Mapa[x][y].Contenido = 0;
			}
		// contar
		m = n;
		n = 0;
		for(x = 0; x < 10; x++)
			for(y = 0; y < 10; y++)
			{
				Mapa->Visto[x][y] = !(Mapa->Contenido[x][y] == m);
				if(!Mapa->Visto[x][y]) n++;
			}
	}
	else
	{
		m = 0;
		for(x = 0; x < 10; x++)
			for(y = 0; y < 10; y++)
			{
				Mapa->Contenido[x][y] = 0;
				if(!Jugador->Visto[x][y])
				{
					// Contar esquinas: y alrededores...
					if(x > 0 && y > 0) if(!Jugador->Visto[x-1][y-1]) Mapa->Contenido[x][y] += (char)5;
					if(x < 9 && y > 0) if(!Jugador->Visto[x+1][y-1]) Mapa->Contenido[x][y] += (char)5;
					if(x > 0 && y < 9) if(!Jugador->Visto[x-1][y+1]) Mapa->Contenido[x][y] += (char)5;
					if(x < 9 && y < 9) if(!Jugador->Visto[x+1][y+1]) Mapa->Contenido[x][y] += (char)5;
					if(x > 0) if(!Jugador->Visto[x-1][y]) Mapa->Contenido[x][y]++;
					if(x < 9) if(!Jugador->Visto[x+1][y]) Mapa->Contenido[x][y]++;
					if(y < 9) if(!Jugador->Visto[x][y+1]) Mapa->Contenido[x][y]++;
					if(y < 9) if(!Jugador->Visto[x][y+1]) Mapa->Contenido[x][y]++;
					Mapa->Contenido[x][y]++;
					m = max((char)m, Mapa->Contenido[x][y]);
				}
				//else Mapa[x][y].Contenido = 0;
			}
		// opción primaria: confiar en la suerte
		n = 0;
		for(x = 0; x < 10; x++)
			for(y = 0; y < 10; y++)
				if(Mapa->Contenido[x][y] == m)
				{
					n++;
					Mapa->Visto[x][y] = 0;
				}
				else Mapa->Visto[x][y] = 1;
	// b) Prioridad por casillas menos aisladas
	}
	Suerte(n);
	EstadoOrdenador++;
}

void TPanel::ProcesaTocado()
{
	int i, p, o, n, m, lm, Xa, Ya, Xb, Yb, x, y;
	TipoCoor C[4];

	if(Tocados == 3)
	{
		Xa = min(min(Coor[0].X, Coor[1].X), Coor[2].X);
		Xb = max(max(Coor[0].X, Coor[1].X), Coor[2].X);
		Ya = min(min(Coor[0].Y, Coor[1].Y), Coor[2].Y);
		Yb = max(max(Coor[0].Y, Coor[1].Y), Coor[2].Y);
		Coor[0].X = Xa;
		Coor[1].X = (Xa+Xb)/2;
		Coor[2].X = Xb;
		Coor[0].Y = Ya;
		Coor[1].Y = (Ya+Yb)/2;
		Coor[2].Y = Yb;
	}
	if(Tocados > 1) // sabemos la dirección
		o = (Coor[0].Y == Coor[1].Y); //orientación
	else
	{
		o = 1;
		p = 4;
	}

	for(i = 0; i < 4; i++) C[i].V = 1;

	if(o) //Horizontal
	{
		C[3].X = min(Coor[0].X, Coor[Tocados-1].X) - 1;
		C[2].X = max(Coor[0].X, Coor[Tocados-1].X) + 1;
		C[3].Y = Coor[0].Y;
		C[2].Y = Coor[0].Y;
		C[2].V = 0;
		C[3].V = 0;
	}
	else
	{
		C[1].X = Coor[0].X;
		C[0].X = Coor[0].X;
		C[1].Y = min(Coor[0].Y, Coor[Tocados-1].Y) - 1;
		C[0].Y = max(Coor[0].Y, Coor[Tocados-1].Y) + 1;
		C[0].V = 0;
		C[1].V = 0;
	}

	if(p == 4)
	{
		C[1].X = Coor[0].X;
		C[0].X = Coor[0].X;
		C[1].Y = Coor[0].Y - 1;
		C[0].Y = Coor[0].Y + 1;
		C[1].V = 0;
		C[0].V = 0;
	}

	// Descartar los vistos:
	for(i = 0; i < 4; i++)
	{
		 // Elimina bordes
		 if(C[i].X < 0 || C[i].X > 9 || C[i].Y < 0 || C[i].Y > 9)
			C[i].V = 1;
		 else
		 // elimina vistos
		 if(Trabajo2->Visto[C[i].X][C[i].Y])
			C[i].V = 1;
	}

	// Asignar prioridades a los que quedan:
	// averiguar el barco de mayor tamaño que nos cabe:
	Xa = 0;
	Xb = 0;
	if(!C[2].V)
		while(!Trabajo2->Visto[C[2].X+Xa][C[2].Y])
		{
			Xa++;
			if(C[2].X+Xa > 9) break;
		}
	if(!C[3].V)
		while(!Trabajo2->Visto[C[3].X-Xb][C[3].Y])
		{
			Xb++;
			if(C[3].X-Xb < 0) break;
		}
	Ya = 0;
	Yb = 0;
	if(!C[0].V)
		while(!Trabajo2->Visto[C[0].X][C[0].Y+Ya])
		{
			Ya++;
			if(C[0].Y+Ya > 9) break;
		}
	if(!C[1].V)
		while(!Trabajo2->Visto[C[1].X][C[1].Y-Yb])
		{
			Yb++;
			if(C[1].Y-Yb < 0) break;
		}

	if(o) // Horizontal
	{
		x = Xa+Xb+Tocados;
		y = 1;
	}
	else
	{
		x = 1;
		y = Ya+Yb+Tocados;
	}
	if(Tocados == 1) y = Ya+Yb+Tocados;

	// a) Criba eliminar casillas donde no es posible que quepa
	//    el barco mayor que queda por hundir:
	for(lm = 3; lm >= 0; lm--)
		if(Barco[0][lm] < 4-lm) break;

	lm = min(max(x,y), lm);
	// Si hemos llegado aquí, significa que queda al menos un barco
	// de dos o mayor que cabe en el hueco alrededor de los tocados

	for(x = 0; x < 10; x++)
		for(y = 0; y < 10; y++)
		{
			Mapa->Contenido[x][y] = 0;
			Mapa->Visto[x][y] = Jugador->Visto[x][y];
		}
	for(x = 0; x < Tocados; x++)
		Mapa->Visto[Coor[x].X][Coor[x].Y] = 0;

	for(x = 0; x < 10; x++)
		for(y = 0; y < 10; y++)
		{
			if(!Mapa->Visto[x][y])
			{
				// Caso horizontal:
				Xa = x;
				Xb = x;
				Ya = y;
				Yb = y;
				while(Xa > 0 && !Mapa->Visto[Xa-1][y] && x-Xa < lm) Xa--;
				while(Xb < 9 && !Mapa->Visto[Xb+1][y] && Xb-x < lm) Xb++;
				while(Ya > 0 && !Mapa->Visto[x][Ya-1] && y-Ya < lm) Ya--;
				while(Yb < 9 && !Mapa->Visto[x][Yb+1] && Yb-y < lm) Yb++;
				if(Xb-Xa >= lm) Mapa->Contenido[x][y] = (char)(Xb-Xa-lm+1);
				if(Yb-Ya >= lm) Mapa->Contenido[x][y] += (char)(Yb-Ya-lm+1);
			}
		}
	// Mapa hecho, filtrar coordenadas posibles
	for(x = 0; x < 10; x++)
		for(y = 0; y < 10; y++)
			Mapa->Visto[x][y] = 1;
	for(x = 0; x < 4; x++)
		if(!C[x].V) Mapa->Visto[C[x].X][C[x].Y] = 0;

	// optimización: elegir el que tenga más esquinas libres:
	n = 0;
	for(x = 0; x < 10; x++)
		for(y = 0; y < 10; y++)
		{
			if(!Mapa->Visto[x][y])
			{
				// Contar esquinas:
				if(x > 0 && y > 0) if(!Jugador->Visto[x-1][y-1]) Mapa->Contenido[x][y]++;
				if(x < 9 && y > 0) if(!Jugador->Visto[x+1][y-1]) Mapa->Contenido[x][y]++;
				if(x > 0 && y < 9) if(!Jugador->Visto[x-1][y+1]) Mapa->Contenido[x][y]++;
				if(x < 9 && y < 9) if(!Jugador->Visto[x+1][y+1]) Mapa->Contenido[x][y]++;
				n = max((char)n, Mapa->Contenido[x][y]);
			}
			//else Mapa[x][y].Contenido = 0;
		}
	// contar
	m = n;
	n = 0;
	for(x = 0; x < 10; x++)
		for(y = 0; y < 10; y++)
		{
			if(!Mapa->Visto[x][y]) Mapa->Visto[x][y] = !(Mapa->Contenido[x][y] == m);
			if(!Mapa->Visto[x][y]) n++;
		}

	Suerte(n);
	EstadoOrdenador++;
}

void TPanel::DispararJugador(int x, int y)
{
	int n, m;

	// Verificación:
	if(Ordenador->Contenido[x][y] >= NEGRO)
	{  // Tocado
		for(n = -1; n < 2; n += 2)
			for(m = -1; m < 2; m += 2)
				if(x+n >= 0 && x+n < 10 && y+m >= 0 && y+m < 10)
					if(!Ordenador->Contenido[x+n][y+m])
					{
						Trabajo->Contenido[x+n][y+m] = AZUL;
						Trabajo->Visto[x+n][y+m] = 1;
						Ordenador->Visto[x+n][y+m] = 1;
						PintaCasilla(FALSE, x+n, y+m);
					}
		Ordenador->Visto[x][y] = 1;
		Trabajo->Contenido[x][y] = ROJO;
		Trabajo->Visto[x][y] = 1;
		PintaCasilla(FALSE, x, y);
		Mensaje("TOCADO");
		Turno = JUGADOR;
		if(Hundido(Ordenador, Trabajo, FALSE ,x, y)) // verificar fin de partida
		{
			Mensaje("TOCADO Y HUNDIDO");
			// Lista de barcos
			n = (Ordenador->Contenido[x][y] >> 1)-2;
			Barco[1][n]++;
			Contador();
			if(Barco[1][0]+Barco[1][1]+Barco[1][2]+Barco[1][3] == 10)
			{
				MessageBox("Tú ganas.", "FIN DEL JUEGO", MB_ICONINFORMATION | MB_OK);
				Turno = NADIE;
				VerResto();
			}
		}
	}
	else
	{  // Agua
		Ordenador->Visto[x][y] = 1;
		Trabajo->Contenido[x][y] = AZUL;
		Trabajo->Visto[x][y] = 1;
		PintaCasilla(FALSE, x, y);
		Turno = ORDENADOR;
	}
}

void TPanel::DispararOrdenador(int x, int y)
{
	int m, n;

	Resto--;
	if(Jugador->Contenido[x][y]) // tocado
	{
		for(n = -1; n < 2; n += 2)
			for(m = -1; m < 2; m += 2)
				if(x+n >= 0 && x+n < 10 && y+m >= 0 && y+m < 10)
					if(!Jugador->Contenido[x+n][y+m])
					{
						if(!Jugador->Visto[x+n][y+m]) Resto--;
						Trabajo2->Contenido[x+n][y+m] = AZUL;
						Trabajo2->Visto[x+n][y+m] = 1;
						Jugador->Visto[x+n][y+m] = 1;
						PintaCasilla(TRUE, x+n, y+m);
					}
		Trabajo2->Contenido[x][y] = ROJO;
		Trabajo2->Visto[x][y] = 1;
		Jugador->Visto[x][y] = 1;
		PintaCasilla(TRUE, x, y);
		Mensaje("TOCADO");
		// Verificar Hundido: submarino
		if(!Hundido(Jugador, Trabajo2, TRUE ,x, y))
		{
			Estrategia = HUNDIR;
			Coor[Tocados].X = x;
			Coor[Tocados].Y = y;
			Tocados++;
		}
		else
		{
			Mensaje("TOCADO Y HUNDIDO");
			Estrategia = BUSCA;
			// Lista de barcos
			Barco[0][Tocados]++;  // Otro submarino hundido.
			Contador();
			Tocados = 0;
			// Fin del juego:
			if(Barco[0][0] + Barco[0][1] + Barco[0][2] + Barco[0][3] == 10)
			{
				MessageBox("Yo gano.", "FIN DEL JUEGO", MB_ICONINFORMATION | MB_OK);
				Turno = NADIE;
				// Ver localizazión de barcos no hundidos
				VerResto();
				return;
			}
		}
		Turno = ORDENADOR;
	}
	else
	{  // Agua
		Trabajo2->Contenido[x][y] = AZUL;
		Trabajo2->Visto[x][y] = 1;
		Jugador->Visto[x][y] = 1;
		PintaCasilla(TRUE, x, y);
		Mensaje("AGUA");
		Turno = JUGADOR;
	}
}


// OWL Aplicación:
class TMyApp : public TApplication {

  public:

  protected:
	 void InitMainWindow();
};

void TMyApp::InitMainWindow()
{
  char		 Nombre[120];

  LoadString(Titulo, Nombre, 120);

  MainWindow = new TFrameWindow(NULL, Nombre, new TPanel(), TRUE);
  MainWindow->SetIcon(this, Icono);
  MainWindow->Attr.Style &= ~(WS_MAXIMIZEBOX | WS_THICKFRAME);
  randomize();
}

int OwlMain(int /*argc*/, char* /*argv*/ [])
{
	return TMyApp().Run();
}

