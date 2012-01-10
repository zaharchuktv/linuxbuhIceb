/*$Id: l_uslkart.h,v 1.4 2011-01-13 13:49:51 sasa Exp $*/
/*16.11.2005	15.11.2005	Белых А.И.	l_uslkart.h
Реквизиты поиска подтверждённых записей услуг
*/

class  uslkart_poi //класс для поиска записей
 {
  public:
   iceb_u_str datan;
   iceb_u_str datak;
   iceb_u_str nomd;
   iceb_u_str kontr;
   iceb_u_str kodop;
   iceb_u_str podr;
   iceb_u_str pr_ras;
   int metka_poi;  //0-без поиска 1-с поиском

  uslkart_poi()
   {
    clear_data();
   }

  void clear_data()
   {
    metka_poi=0;
    datan.new_plus("");
    datak.new_plus("");
    nomd.new_plus("");
    kontr.new_plus("");
    podr.new_plus("");
    kodop.new_plus("");
    pr_ras.new_plus("");
   }
 };
