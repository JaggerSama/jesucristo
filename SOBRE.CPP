

#include <owl\owlpch.h>
#include <owl\dialog.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dos.h>
#include "sobre.h"

// Dialogo de Presentación

DEFINE_RESPONSE_TABLE1(TSobre, TDialog)
	 EV_WM_PAINT,
	 EV_COMMAND(IDOK, cmOk),
END_RESPONSE_TABLE;

TSobre::TSobre(TWindow *Parent): TDialog(Parent, DialogoSobre)
{
	Padre = Parent;
}

void TSobre::cmOk()
{
	Destroy(TRUE);
}

void TSobre::EvPaint()
{
  TPaintDC	dc(*this);
  HFONT hFont;
  char Recurso[80];
  int y;

  hFont = CreateFont(18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		"Times New Roman");

  dc.SetBkMode(TRANSPARENT);
  dc.SelectObject(hFont);
  for(y = LINEA1; y <= LINEA7; y++)
  {
	  GetApplication()->LoadString(y, Recurso, 50);
	  dc.TextOut( 30, 130+16*(y-LINEA1), Recurso);
  }
  DeleteObject(hFont);
}

