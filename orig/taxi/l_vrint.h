/*$Id: l_vrint.h,v 1.6 2011-01-13 13:50:09 sasa Exp $*/
/*02.09.2006	30.11.2005	Белых А.И.	l_vrint.h
Структура записи поиска в таблице временных интервалов
*/

class l_vrint_zap
 {
  public:
   iceb_u_str datan;
   iceb_u_str datak;
   iceb_u_str vremn;
   iceb_u_str vremk;
   iceb_u_str koment;
   l_vrint_zap()
   {
    clear_data();
   }
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    vremn.new_plus("");
    vremk.new_plus("");
    koment.new_plus("");
   }
 };
