/*$Id: rest_oth.h,v 1.6 2011-01-13 13:50:09 sasa Exp $*/
/*24.08.2006	04.07.2004	Белых А.И.	rest_oth.h
Реквизиты поиска для отчета
*/
class rest_oth_data
 {
  public:
  static class iceb_u_str datan;
  static class iceb_u_str datak;
  static class iceb_u_str vremn;
  static class iceb_u_str vremk;
  static class iceb_u_str podr;
  static class iceb_u_str skl;
  static class iceb_u_str kod_op; //код оператора
  iceb_u_spisok imaf;
  iceb_u_spisok naim;

  rest_oth_data()
   {
    clear();
   }

  void clear()
   {
    datan.new_plus("");
    datak.new_plus("");
    vremn.new_plus("");
    vremk.new_plus("");
    podr.new_plus("");
    skl.new_plus("");
    kod_op.new_plus("");  
   }
 
 };
