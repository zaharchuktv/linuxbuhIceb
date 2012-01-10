/*$Id: iceb_fioruk.h,v 1.5 2011-06-07 08:52:29 sasa Exp $*/
/*27.02.2010	04.04.2008	Белых А.И.	iceb_fioruk.h
Реквизиты руководителя
*/

class iceb_fioruk_rk
 {
  public:
   class iceb_u_str fio;
   class iceb_u_str inn;
   class iceb_u_str telef;
   class iceb_u_str dolg;
   class iceb_u_str famil;
   class iceb_u_str ima;
   class iceb_u_str oth;
   class iceb_u_str famil_inic;
   char inic[32];

  iceb_fioruk_rk()
   {
    clear();
   }
  void clear()
   {
    fio.plus("");
    inn.plus("");
    telef.plus("");
    dolg.plus("");
    famil.new_plus("");
    ima.new_plus("");
    oth.new_plus("");
    famil_inic.plus("");    
    memset(inic,'\0',sizeof(inic));
   }
   
 };
