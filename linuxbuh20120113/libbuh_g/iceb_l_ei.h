/*$Id: iceb_l_ei.h,v 1.4 2011-01-13 13:49:59 sasa Exp $*/
/*06.09.2010	05.05.2004	Белых А.И.	iceb_l_ei.h
*/
enum
{
  FK2,
  FK3,
  FK4,
  FK5,
  FK10,
  SFK2,
  KOL_F_KL
};

enum
 {
  PFK2,
  PFK4,
  PFK10,
  KOL_PFK
 };

enum
 {
  E_KOD,
  E_NAIM,
  KOLENTER  
 };

class iceb_l_ei_rek
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

