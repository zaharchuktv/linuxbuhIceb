/*$Id: l_vod.h,v 1.2 2011-01-13 13:50:09 sasa Exp $*/
/*30.05.2009	30.05.2009	Белых А.И.	l_vod.h
Реквизиты для работы со списком категорий
*/
class l_vod_rek
 {
  public:
   class iceb_u_str kodvod;
   class iceb_u_str fio;
   class iceb_u_str adres;
   class iceb_u_str telef;
   class iceb_u_str gosn;
   
   short metka_poi;   /*0-без поиска 1-поиск*/
   
   l_vod_rek()
    {
     clear_data();
     metka_poi=0;
    }
  void clear_data()
   {
    kodvod.new_plus("");
    fio.new_plus("");
    adres.new_plus("");
    telef.new_plus("");
    gosn.new_plus("");
   }
 
 };
