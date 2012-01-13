/*$Id: admin_user.h,v 1.2 2011-01-13 13:49:49 sasa Exp $*/
/*14.11.2010	14.11.2010	Белых А.И.	admin_user.h
Реквизиты поиска
*/

class admin_user_poi
 {
  public:
   class iceb_u_str login;
   class iceb_u_str imabaz;
   class iceb_u_str host;
   short metka_poi;
   
   admin_user_poi()
    {
     metka_poi=0;
     clear();
    }
  void clear()
   {
    login.new_plus("");
    imabaz.new_plus("");
    host.new_plus("");
   } 
 };
 