/*$Id: iceb_l_grupmat.h,v 1.4 2011-01-13 13:49:59 sasa Exp $*/
/*14.06.2006	30.12.2003	Белых А.И.	iceb_l_grupmat.h
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

class  grupmat_rek
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

