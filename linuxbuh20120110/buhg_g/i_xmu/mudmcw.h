/*$Id: mudmcw.h,v 1.2 2011-01-13 13:49:51 sasa Exp $*/
/*23.04.2009	23.04.2009	Белых А.И.	mudmcw.h
Реквизиты поиска для получения отчёта движения по кодам материалов
*/

class mudmcw_poi
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str shet;
   class iceb_u_str sklad;
   class iceb_u_str grupa;
   class iceb_u_str kodmat;
   class iceb_u_str naim;
   
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
  mudmcw_poi()
   {
    clear();
   }
  void clear()
   {
    datan.new_plus("");
    datak.new_plus("");
    shet.new_plus("");
    sklad.new_plus("");
    grupa.new_plus("");
    kodmat.new_plus("");
    naim.new_plus("");
   }
 
 };
 
