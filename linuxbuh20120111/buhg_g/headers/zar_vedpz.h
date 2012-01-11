/*$Id: zar_vedpz.h,v 1.2 2011-01-13 13:49:56 sasa Exp $*/
/*08.12.2006	08.12.2006	Белых А.И.	zar_vedpz.h
Реквизиты для расчёта ведомости производственных затрат
*/
class zar_vedpz_rek
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str podr;
   class iceb_u_str kod_grup;
   class iceb_u_str shet;
   short metka_pr_ras;  //0-всё 1-начисления 2-удержания
   
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   zar_vedpz_rek()
    {
     clear_data();
    }
  
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    podr.new_plus("");
    kod_grup.new_plus("");
    shet.new_plus("");
    metka_pr_ras=0;
   }
 };
