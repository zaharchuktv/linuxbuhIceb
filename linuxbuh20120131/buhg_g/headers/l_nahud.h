/*$Id: l_nahud.h,v 1.7 2011-01-13 13:49:50 sasa Exp $*/
/*12.11.2009	27.09.2006	Белых А.И.	l_nahud.h
Реквизиты для поиска записей в списке начислений/удержаний
*/

class l_nahud_rek
 {
  public:
   class iceb_u_str podr;
   class iceb_u_str mes_god_pros;
   class iceb_u_str den_n_pros;
   class iceb_u_str den_k_pros;
   class iceb_u_str tabnom;
   class iceb_u_str fio;
   class iceb_u_str kateg;
   class iceb_u_str shet;
   class iceb_u_str vkm;
   class iceb_u_str koment;
   class iceb_u_str grup_podr;
   class iceb_u_str mnks;  //Метка наличия карт-счёта "+" "-" " "
   class iceb_u_str kodbanka; 
   short metka_poi; //0-не проверять 1-проверять

   l_nahud_rek()
    {
     clear_rek();
    }
   void clear_rek()
    {
     podr.new_plus("");
     mes_god_pros.new_plus("");
     den_n_pros.new_plus("");
     den_k_pros.new_plus("");
     tabnom.new_plus("");
     fio.new_plus("");
     kateg.new_plus("");
     shet.new_plus("");
     vkm.new_plus("");
     grup_podr.new_plus("");
     mnks.new_plus("");
     koment.new_plus("");
     metka_poi=0;
     kodbanka.new_plus("");
    } 
 
 
 
 };
int l_nahud_prov_str(SQL_str row,class l_nahud_rek *data,GtkWidget*);
