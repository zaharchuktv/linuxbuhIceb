/*$Id: iceb_l_sklad.h,v 1.5 2011-01-13 13:49:59 sasa Exp $*/
/*06.02.2007	05.05.2004	Белых А.И.	iceb_l_sklad.h
*/


class  sklad_rek
 {
  public:
  class iceb_u_str kod;
  class iceb_u_str naim;
  class iceb_u_str fmol;// фамилия материально-ответственного
  class iceb_u_str dolg; // должность
  int metka_isp_skl;  //0-используется 1-не используется   
  
  void clear_zero()      
   {
    kod.new_plus("");  
    naim.new_plus("");  
    fmol.new_plus("");
    dolg.new_plus("");
    metka_isp_skl=0;    
   }
 };
 

extern nl_catd	fils; /*Указатель на базу сообщений*/
extern SQL_baza	bd;
extern char *name_system;

void sklad_create_list (class sklad_data *data);
