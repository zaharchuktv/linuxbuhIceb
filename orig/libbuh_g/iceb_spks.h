/*$Id: iceb_spks.h,v 1.4 2011-01-13 13:49:59 sasa Exp $*/
/*29.12.2003	18.12.2003	Белых А.И.	iceb_spks.h
*/
#include <stdlib.h>

enum
 {
  E_DATAN,
  E_DATAK,
  E_SHETA,
  KOLENTER
 };

#define RAZMER_SHETS 20

class iceb_spks_data
 {
  public:
  GtkWidget *window;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  char        shet[RAZMER_SHETS];  //Сюда записывем сначала shetsrs, а потом shets
  iceb_u_str  shetsrs;  //Счет с развернутым сальдо по которому делаем расчет
  static char datn[11];
  static char datk[11];
  static char shets[RAZMER_SHETS];

  GtkWidget *entry[KOLENTER];
  GtkWidget *label;  //Ждите заменяемое на "рачет завершен"
  GtkWidget *knopka; //Кнопка для меню хода расчета
  int metka_voz;   //0-выход из меню без запуска отчета 1- с запуском отчета
  short sproh;     //Счетчик количества проходов
 
  iceb_u_str kontr; //Код контрагента для которого делаем расчет
  iceb_u_str naikontr; //Наименование контрагента для которого делаем расчет
  iceb_u_spisok imaf;
  iceb_u_spisok naim;
  FILE *ff1;
  FILE *ffact;
  double ddn;
  double kkn;
  double dd;
  double kk;
  double debm; //Месячный дебет
  double krem; //Месячный кредит
  double db;   //Общий итог по дебету
  double kr;   //Общий итог по кредиту
  short         metka_prov; //0-все проводки 1-без встречных проводок
  short         metka_proh; 
  int           metka_vozr; //Если вернули 0-расчет прошел нормально 1-нет
  short         metka_zavr; //0-расчет не завершен 1-завершен
  time_t        vrem_nr;    //Время начала расчета
  
  //Конструкор
  iceb_spks_data()
   {
    
    db=kr=debm=krem=ddn=0.;
    kkn=0.;
    dd=0.;
    kk=0.;
    metka_proh=metka_prov=metka_zavr=0;
    metka_vozr=0;
    sproh=0;
   }

  void clear_zero()
   {
    memset(datn,'\0',sizeof(datn));
    memset(datk,'\0',sizeof(datk));
    memset(shets,'\0',sizeof(shets));
   }  
 };
