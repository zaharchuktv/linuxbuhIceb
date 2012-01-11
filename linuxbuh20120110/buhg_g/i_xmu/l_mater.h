/*$Id: l_mater.h,v 1.13 2011-01-13 13:49:50 sasa Exp $*/
/*11.09.2006	05.05.2004	Белых А.И.	l_mater.h
Реквизиты для ввода и корректировки записи и для поиска 
*/

class  mater_rek
 {
  public:
  iceb_u_str kod;
  iceb_u_str grupa;
  iceb_u_str naim;
  iceb_u_str cena;
  iceb_u_str ei;
  iceb_u_str krat;
  iceb_u_str kodtar;
  iceb_u_str fas;
  iceb_u_str htrix;
  int       mcena;
  iceb_u_str artik;
  iceb_u_str kriost;
  short metka_poi;     //0-без поиска 1-с поиском
  short metka_mat_s_ost;
  double kolih; 
  short dt,mt,gt;
    
  mater_rek()
   {
    iceb_u_poltekdat(&dt,&mt,&gt);
   }

  void clear_zero()      
   {
    metka_poi=0;
    kod.new_plus("");
    grupa.new_plus("");
    naim.new_plus("");  
    cena.new_plus("");
    ei.new_plus("");
    krat.new_plus("");
    kodtar.new_plus("");
    fas.new_plus("");
    htrix.new_plus("");
    artik.new_plus("");
    kriost.new_plus("");
    mcena=0;
    metka_mat_s_ost=0;
    kolih=0.;
    
   }
 };

extern nl_catd	fils; /*Указатель на базу сообщений*/
extern SQL_baza	bd;
extern char *name_system;

void            mater_get_pm0(GtkWidget*,int*);
void            mater_get_pm1(GtkWidget*,int*);
void            mater_get_pm2(GtkWidget*,int*);


