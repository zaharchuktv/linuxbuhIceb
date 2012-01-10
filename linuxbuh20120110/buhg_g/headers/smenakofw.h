/*$Id: smenakofw.h,v 1.2 2011-01-13 13:49:53 sasa Exp $*/
/*19.10.2007	19.10.2007	Белых А.И.	smenakofw.h
Данные для смены коэффициентов в карточках основных средств
*/

class smenakofw_data
 {
  public:
  class iceb_u_str datak;
  class iceb_u_str kod_gr;
  class iceb_u_str kof;
  short metka_uh; //0-налоговый 1-бухгалтерский
  
  smenakofw_data()
   {
    metka_uh=0;
    clear_data();
   }

  void clear_data()
   {
    datak.new_plus("");
    kod_gr.new_plus("");
    kof.new_plus("");
   }
 };
