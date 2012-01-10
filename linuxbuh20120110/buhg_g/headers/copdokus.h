/*$Id: copdokus.h,v 1.3 2011-01-13 13:49:49 sasa Exp $*/
/*17.06.2006	22.11.2005	Белых А.И.	copdokus.h
Структура с реквизитами поиска нужных документов для копирования.
*/

class copdokus_data
 {
  public:
  class iceb_u_str datan;
  class iceb_u_str datak;
  class iceb_u_str kodop;
  class iceb_u_str kontr;
  class iceb_u_str pr_ras;
  class iceb_u_str nomdok;
  class iceb_u_str podr;
  
  //Реквизиты документа куда копируем
  short dd,md,gd;
  int    tipz; //1-приход 2-расход
  iceb_u_str nomdok_c;
  int    pod;
  
  copdokus_data()
   {
    clear_data();
   }

  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    kodop.new_plus("");
    kontr.new_plus("");
    pr_ras.new_plus("");
    nomdok.new_plus("");
    podr.new_plus("");

   }
 };
