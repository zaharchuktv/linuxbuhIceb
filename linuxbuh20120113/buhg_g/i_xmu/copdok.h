/*$Id: copdok.h,v 1.5 2011-01-13 13:49:49 sasa Exp $*/
/*17.06.2006	30.09.2004	Белых А.И.	copdok.h
Структура с реквизитами поиска нужных документов для копирования.
*/

class copdok_data
 {
  public:
  class iceb_u_str datan;
  class iceb_u_str datak;
  class iceb_u_str kodop;
  class iceb_u_str kontr;
  class iceb_u_str pr_ras;
  class iceb_u_str nomdok;
  class iceb_u_str sklad;
  
  //Реквизиты документа куда копируем
  short dd,md,gd;
  int    tipz; //1-приход 2-расход
  class iceb_u_str nomdok_c;
  int    skl;
  
  copdok_data()
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
    sklad.new_plus("");

   }
 };
