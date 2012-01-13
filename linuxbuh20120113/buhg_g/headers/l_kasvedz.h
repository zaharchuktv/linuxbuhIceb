/*$Id: l_kasvedz.h,v 1.2 2011-01-13 13:49:50 sasa Exp $*/
/*09.06.2009	09.06.2009	Белых А.И.	l_kasvedz.h
Реквизиты для работы со списком ведомостей 
*/
class l_kasvedz_rek
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str nomd;
   class iceb_u_str suma;
   class iceb_u_str koment;
   int metka_poi; /*0-без поиска 1- с поиском*/
   l_kasvedz_rek()
    {
     metka_poi=0;
     clear_data();
    }
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    suma.new_plus("");
    koment.new_plus("");
    nomd.new_plus("");
   }
 
 };
