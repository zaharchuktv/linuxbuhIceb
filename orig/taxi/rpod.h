/*$Id: rpod.h,v 1.5 2011-01-13 13:50:09 sasa Exp $*/
/*29.04.2004	29.04.2004	Белых А.И.	rpod.h
Класс для работы со списком сальдо по бухгалтерским счетам
*/

enum
{
  FK2,
  FK3,
  FK4,
  FK5,
  FK6,
  FK10,
  SFK2,
  KOL_F_KL
};

enum
 {
  PFK2,
  PFK4,
  PFK10,
  KOL_PFK
 };

enum
 {
  E_KOD,
  E_NAIM,
  KOLENTER  
 };

class  rpod_rek
 {
  public:
  iceb_u_str kod;
  iceb_u_str naim;

  void clear_zero()      
   {
    kod.new_plus("");  
    naim.new_plus("");  
   }
 };
 
class  rpod_data
 {
  public:

  rpod_rek vvod;
  rpod_rek poisk;

  iceb_u_str kodv;
  iceb_u_str naimv;
  int        nomervsp; //Номер записи в списке

  GtkWidget *label_kolstr;
  GtkWidget *label_poisk;
  GtkWidget *sw;
  GtkWidget *entry[KOLENTER];
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *vwindow;
  GtkWidget *pwindow;
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_pv[KOL_PFK];
  short     kl_shift; //0-отжата 1-нажата  
  short     metkazapisi; //0-новая запись 1-корректировка
  short     snanomer;   //номер записи на которую надостать или -2
  int       kolzap;     //Количество записей
  short     metka_poi;  //0-без поиска 1-с поиском
  int       metka_voz;  //0-выбрали 1-нет  
  short     metka_rr;   //0-работа со списком 1-выбор
  //Конструктор
  rpod_data()
   {
    snanomer=-2;
    metka_rr=metka_voz=kl_shift=metkazapisi=metka_poi=0;
    vwindow=pwindow=window=treeview=NULL;
   }      
 };

extern nl_catd	fils; /*Указатель на базу сообщений*/
extern SQL_baza	bd;
extern char *name_system;

void rpod_create_list (class rpod_data *data);
