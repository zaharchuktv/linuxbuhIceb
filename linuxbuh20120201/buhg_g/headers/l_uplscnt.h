/*$Id: l_uplscnt.h,v 1.2 2011-01-13 13:49:51 sasa Exp $*/
/*10.03.2008	10.03.2008	Белых А.И.	l_uplscnt.h
Реквизиты для поиска записей в списке средневзвещенных цен на топливо
*/
class l_uplscnt_rek
 {
  public:
   class iceb_u_str data;
   class iceb_u_str kod_top;
   class iceb_u_str cena;
   class iceb_u_str koef;
   
   l_uplscnt_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    data.new_plus("");
    kod_top.new_plus("");
    cena.new_plus("");
    koef.new_plus("");
   }
 
 };
