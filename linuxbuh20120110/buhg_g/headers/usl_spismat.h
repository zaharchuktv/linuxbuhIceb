/*$Id: usl_spismat.h,v 1.6 2011-01-13 13:49:55 sasa Exp $*/
/*23.11.2005	23.11.2005	Белых А.И.	usl_spismat.h
Реквизиты для списания материаллов в материальном учёте
*/
class usl_spismat_rek
 {
  public:
  //Реквизиты документа в учёте услуг
  short dd,md,gd;
  iceb_u_str nomdoku;
  int podr;
  int tipz;
  iceb_u_str kontr;
  int lnds;
  float pnds;
  
  //Реквизиты списания
  iceb_u_str datas;
  iceb_u_str sklad;
  iceb_u_str nomdok;
  iceb_u_str kodop;
    
  usl_spismat_rek()
   {
    clear_data();
   }
  void clear_data()
   {
    datas.new_plus("");
    sklad.new_plus("");
    nomdok.new_plus("");
    kodop.new_plus("");
   }
 };
