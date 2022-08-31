/****************************************************************************


sobre.h




*****************************************************************************/

#define LINEA1	800
#define LINEA2	801
#define LINEA4	803
#define LINEA5	804
#define LINEA6	805
#define LINEA7	806
#define LINEA3	802
#define Logotipo	1123
#define DialogoSobre	2001

class TSobre : public TDialog {
  public:
	 TSobre(TWindow *Parent);

  private:
	 TWindow		*Padre;

  protected:
	 void EvPaint();
	 void cmOk();
  public:

  DECLARE_RESPONSE_TABLE(TSobre);
};

