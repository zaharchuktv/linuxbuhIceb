/*$Id: rasspdok.h,v 1.8 2011-01-13 13:49:52 sasa Exp $*/
/*23.02.2009	31.01.2005	Белых А.И.	rasspdok.h
Класс для получения отчёта - список документов в материальном учёте
*/

class rasspdok_rr
 {
  public:
  static iceb_u_str datan;
  static iceb_u_str datak;
  static iceb_u_str kodop;
  static iceb_u_str kontr;
  static iceb_u_str sklad;
  static iceb_u_str grupk;
  static short metka_pr;   //0-всё 1-только приходы 2-только расходы
  static short metka_sort; //0-сортировка по датам 1-по номерам документов
  static short metka_dok; //0-только подтверждённые 1-неподтверждённые 2-все

  iceb_u_spisok imaf;
  iceb_u_spisok naimf;
  
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    kodop.new_plus("");
    kontr.new_plus("");
    sklad.new_plus("");
    grupk.new_plus("");
    metka_pr=0;
    metka_sort=0;
    metka_dok=0;
   }
 };
