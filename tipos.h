#define BLANCO 0
#define ROJO	1
#define AZUL	2
#define VERDE  3
#define NEGRO	4
#define IMPACTO	5
#define CHOFF		6


// valores de Turno
#define ORDENADOR 0
#define JUGADOR	1
#define NADIE		2

// valores de Estrategia
#define HUNDIR		1
#define BUSCA		0

// Valores para Estados de espera:
#define AVISO		0
#define ANALISIS  1
#define PREPARADO 2
#define DISPARO	3

// valores posibles de Raton:
#define EDICION 0
#define JUEGO 	 1
#define ESPERA	 2

// Orientaciones de los barcos:
#define HORIZONTAL	true
#define VERTICAL		false

// posible librería de gráficos:
//static TModule  ResourceGraficos("barcos.dll");   // DLL to be loaded.

typedef struct
{
	int Contenido;
	int Visto;
} TipoCasilla;

typedef struct
{
	char Contenido[10][10];
	char Visto[10][10];
} TipoTablero;

typedef struct
{
	int X, Y, V;
} TipoCoor;

typedef TipoCasilla TipoLinea[10];
typedef TipoLinea   TipoMapa[10];
