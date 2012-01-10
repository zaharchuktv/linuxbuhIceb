/*$Id: l_klientr.h,v 1.17 2011-01-13 13:50:09 sasa Exp $*/
/*02.11.2007
*/

#include "klient.h"
enum
{
  FK2,
  FK3,
  FK4,
  FK5,
  FK6,
  FK7,
  FK8,
  FK9,
  FK10,
  SFK2,
  KOL_F_KL
};

enum
{
 COL_KOD,
 COL_MB,
 COL_FIO,
 COL_POL,
 COL_PROCENT,
 COL_GRUPA, 
 COL_SP_PODR,
 COL_KOMENT,
 COL_ADRES,
 COL_TELEFON,
 COL_KVV,
 COL_DVK,
 COL_DEN_ROG,
 COL_VREM,
 COL_KTO,
 NUM_COLUMNS
};


class  klientr_data
 {
  public:
  
  class klient_rek_data poi;
  
  iceb_u_str  kodv;
  int         nomervsp; //Номер записи в списке

  GtkWidget *label_kolstr;
  GtkWidget *labelpoi;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *radiobutton[3];
  short     metka_rr;
  short     metkapoi;    //0-без поиска 1-с поиском
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать или -2
  int       kolzap;     //Количество записей
  int       voz;  
  int metka_bl; //0-разблокированные 1-заблокированные 2-все  
  iceb_u_str kodkl;
  
  //Конструктор
  klientr_data()
   {
    metka_bl=0;
    metkapoi=0; //0-новая запись 1-корректировка
    voz=0;
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
   }      
 };

void klientr_create_list (class klientr_data *data);
