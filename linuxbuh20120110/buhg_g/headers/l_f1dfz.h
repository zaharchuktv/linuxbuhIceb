/*$Id: l_f1dfz.h,v 1.3 2011-01-13 13:49:50 sasa Exp $*/
/*11.01.2007	09.01.2007	Белых А.И.	l_f1dfz.h
Реквизиты поиска записей в форме 1ДФ
*/

class l_f1dfz_rek
 {
  public:
   class iceb_u_str fio;
   class iceb_u_str inn;
   class iceb_u_str kod_doh;
   short metka_poi; //0-искать 1-не искать
      
   l_f1dfz_rek()
    {
     clear_data();
    }
  
  void clear_data()
   {
    metka_poi=1;
    fio.new_plus("");
    inn.new_plus("");
    kod_doh.new_plus("");
   }
 };

