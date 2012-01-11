/*$Id: dvtmcf3_r.h,v 1.5 2011-01-13 13:49:50 sasa Exp $*/
/*05.12.2004	03.12.2004	Белых А.И.	dvtmcf3_r.h
*/
#include "dvtmcf3.h"

class dvtmcf3_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;

  class dvtmcf3_rr *rk;
  
  short kon_ras; //0-расчет завершен 1-нет
  int voz;


  iceb_u_spisok spoprp;/*Массив с перечнем операций приходов*/
  iceb_u_spisok spoprr;/*Массив с перечнем операций расходов*/
  iceb_u_int    mko;  /*Массив кодов материалов*/
  iceb_u_double mkm1p; /*Массив количества материала для прихода*/
  iceb_u_double mkm2p; /*Массив веса материала для прихода*/
  iceb_u_double mkm3p; /*Массив цены материала для прихода*/
  iceb_u_double mkm1r; /*Массив количества материала для расхода*/
  iceb_u_double mkm2r; /*Массив веса материала для расхода*/
  iceb_u_double mkm3r; /*Массив цены материала для расхода*/

  iceb_u_double mko1r; /*Массив количества отхода материала для расхода*/
  iceb_u_double mko2r; /*Массив веса отхода материала для расхода*/
  iceb_u_double mko3r; /*Массив цены отхода материала для расхода*/

  iceb_u_int    mkg;  /*Массив кодов групп материалов*/
  iceb_u_double mkg1p; /*Массив количества материала по группе для прихода*/
  iceb_u_double mkg2p; /*Массив веса материала по группе для прихода*/
  iceb_u_double mkg3p; /*Массив цены материала по группе для прихода*/
  iceb_u_double mkg1r; /*Массив количества материала по группе для прихода*/
  iceb_u_double mkg2r; /*Массив веса материала по группе для прихода*/
  iceb_u_double mkg3r; /*Массив цены материала по группе*/

  iceb_u_double mkdp; /*Массив динамики приходов*/
  iceb_u_double mkdr; /*Массив динамики расходов*/

  dvtmcf3_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
