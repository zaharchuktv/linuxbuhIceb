/*$Id:*/
/*17.05.2006	16.05.2006	Белых А.И.	xdkgur.h
реквизиты для расчёта журнала платёжных документов
*/

class xdkgur_rr
 {
  public:

  static class iceb_u_str datan;  // дата начала
  static class iceb_u_str datak;  //дата конца
  static class iceb_u_str kontr;  //код контрагента
  static class iceb_u_str kodop;  //код операции
  char tablica[50];
  
  class iceb_u_spisok imaf;
  class iceb_u_spisok naimf;
  
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    kontr.new_plus("");
    kodop.new_plus("");
   }
 };
