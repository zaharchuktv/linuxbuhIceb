/*$Id: iceb_l_gkontr.h,v 1.6 2011-01-13 13:49:59 sasa Exp $*/
/*05.05.2010	30.12.2003	Белых А.И.	iceb_l_gkontr.h
*/

class  gkontr_rek
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

