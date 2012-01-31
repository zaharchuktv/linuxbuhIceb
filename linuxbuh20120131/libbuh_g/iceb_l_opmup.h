/*$Id: iceb_l_opmup.h,v 1.4 2011-01-13 13:49:59 sasa Exp $*/
/*08.06.2006	05.05.2004	Белых А.И.	iceb_l_opmup.h
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

class  opmup_rek
 {
  public:
  iceb_u_str kod;
  iceb_u_str naim;
  int        vidop; //0-внешняя 1-внутрунняя 2-изменения стоимости
  int        prov; //0-проводки нужно делать 1-нет  

  void clear_zero()      
   {
    kod.new_plus("");  
    naim.new_plus("");  
    vidop=prov=0;
   }
 };
 

extern nl_catd	fils; /*Указатель на базу сообщений*/
extern SQL_baza	bd;
extern char *name_system;

