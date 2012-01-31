/*$Id: glkni.h,v 1.10 2011-01-13 13:49:50 sasa Exp $*/
/*02.03.2010	22.01.2004	Белых А.И.	glkni.h
Реквизиты расчета главной книги
*/

class glkni_rr
 {
  public:
  class iceb_u_str datan;
  class iceb_u_str datak;
  class iceb_u_str shet;
  class iceb_u_str kod_subbal;
  short metka_r;  //0-расчет по субсчетам 1-по счетам
  short metka_bd; //0-хозрасчёт 1-бюджет 2-сводный баланс

  class iceb_u_spisok imaf;
  class iceb_u_spisok naimf;

  glkni_rr()
   {
    clear_data();
   }
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    shet.new_plus("");
    kod_subbal.new_plus("");
    metka_r=0;
    metka_bd=0;
   }
 };

