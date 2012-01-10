/*$Id: l_grkl.h,v 1.3 2011-01-13 13:50:09 sasa Exp $*/
/*31.08.2004	31.08.2004	Белых А.И.	l_grkl.h
*/

class  grkl_rek
 {
  public:
  iceb_u_str kod;
  iceb_u_str naim;
  void clear_zero()      
   {
    kod.new_plus("");  
    naim.new_plus("");  
   }
 };

extern nl_catd	fils; /*Указатель на базу сообщений*/
extern SQL_baza	bd;
extern char *name_system;
