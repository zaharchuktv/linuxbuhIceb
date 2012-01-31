/*$Id: kasothkcn.h,v 1.3 2011-01-13 13:49:50 sasa Exp $*/
/*01.10.2006	01.10.2006	Белых А.И.	kasothkcn.h
Класс для получения отчёта в подсистеме "Учёт кассовых ордеров"
*/

class kasothkcn_rr
 {
  public:
  class iceb_u_str datan;
  class iceb_u_str datak;
  class iceb_u_str kodop;
  class iceb_u_str kassa;
  class iceb_u_str shet;
  class iceb_u_str kod_cn;
  short metka_pr;   //0-всё 1-только приходы 2-только расходы

  iceb_u_spisok imaf;
  iceb_u_spisok naimf;
    
  kasothkcn_rr()
   {
    clear_data();
   }

  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    kodop.new_plus("");
    kassa.new_plus("");
    shet.new_plus("");
    kod_cn.new_plus("");
    metka_pr=0;
   }
 };
