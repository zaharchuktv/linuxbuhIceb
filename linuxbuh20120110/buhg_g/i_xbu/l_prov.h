/*$Id: l_prov.h,v 1.15 2011-01-13 13:49:50 sasa Exp $*/
/*18.01.2010	07.01.2004	Белых А.И.	l_prov.h
*/
#include "prov_poi.h"

enum
{
  FK2,
  FK3,
  FK5,
  FK6,
  FK10,
  SFK3,
  KOL_F_KL
};

enum
 {
  E_SHET,
  E_SHETK,
  E_DATA,
  E_KOMENT,
  E_DEBET,
  E_KREDIT,
  E_KONTR,
  KOLENTER  
 };

class  prov_rek_data
 {
  public:
  int        val;
  class iceb_u_str datap;
  class iceb_u_str shet;
  class iceb_u_str shetk;
  class iceb_u_str kontr;
  class iceb_u_str kontrk;
  class iceb_u_str debet;  //Вводятся и символьные значения
  class iceb_u_str kredit;
  class iceb_u_str kodop;
  class iceb_u_str nomdok;
  class iceb_u_str koment;
  class iceb_u_str metka;
  class iceb_u_str kekv;
  class iceb_u_str datadok;
  time_t     vremz;
  int podr;
  int tipz;
  int        ktoz;
  
  //Конструктор
  prov_rek_data()
   {
    clear_zero();
   }

  void clear_zero()
   {
    val=0;
    shet.new_plus("");
    shetk.new_plus("");
    datap.new_plus("");
    koment.new_plus("");
    debet.new_plus("");
    kredit.new_plus("");
    kontr.new_plus("");
    kontrk.new_plus("");
    nomdok.new_plus("");
    kodop.new_plus("");
    metka.new_plus("");
    kekv.new_plus("");
    datadok.new_plus("");
    vremz=0;
    ktoz=0;
    podr=tipz=0;
   }


 };

class  prov_data
 {
  public:

  class prov_poi_data *rek_poi;
  class prov_rek_data rek_vibr;
  
  GtkWidget *label_kolstr;
  GtkWidget *sw;
  GtkWidget *entry[KOLENTER];
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *vwindow;
  GtkWidget *knopka[KOL_F_KL];
  int       kolzap;
  int       snanomer;   //номер записи на которую надостать
  short     kl_shift; //0-отжата 1-нажата  

  //Конструктор
  prov_data()
   {
    kolzap=0;
    snanomer=0;
    kl_shift=0;
    vwindow=window=treeview=NULL;
   }      


 };

extern nl_catd	fils; /*Указатель на базу сообщений*/
extern SQL_baza	bd;
extern char *name_system;

void prov_create_list (class prov_data *data);
