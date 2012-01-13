/*$Id: dvuslf2w.h,v 1.3 2011-01-13 13:49:50 sasa Exp $*/
/*18.06.2010	16.12.2005	Белых А.И.	dvuslf2w.h
Структура с реквизитами поиска нужных данных
*/
class dvuslf2w_rr
 {
  public:
  
  static iceb_u_str datan;
  static iceb_u_str datak;
  static iceb_u_str shet;
  static iceb_u_str podr;
  static iceb_u_str grupa;
  static iceb_u_str kodusl;
  static iceb_u_str kontr;
  static iceb_u_str kodop;
  static int prr; /*0-все 1-только приходы 2-только расходы*/
    
  iceb_u_spisok imaf;
  iceb_u_spisok naimf;
  
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    shet.new_plus("");
    podr.new_plus("");
    grupa.new_plus("");
    kodusl.new_plus("");
    kontr.new_plus("");
    kodop.new_plus("");
    prr=0;
   }
  
 };
