/*$Id: l_vhvi.h,v 1.3 2011-01-13 13:50:09 sasa Exp $*/
/*02.09.2006	13.12.2005	Белых А.И.	l_vhvi.c
Реквизиты для просмотра записей входов-выходов
*/

class l_vhvi_rp
 {
  public:
  
  iceb_u_str datan;
  iceb_u_str datak;
  iceb_u_str vremn;
  iceb_u_str vremk;
  iceb_u_str podr;
  iceb_u_str kodkart;
  class iceb_u_str tipz;  
  l_vhvi_rp()
   {
    clear_data();
   }
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    vremn.new_plus("");
    vremk.new_plus("");
    podr.new_plus("");
    kodkart.new_plus("");
    tipz.new_plus("");
   }
 };
