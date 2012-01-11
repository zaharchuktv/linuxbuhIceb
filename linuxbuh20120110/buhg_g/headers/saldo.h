/*$Id: saldo.h,v 1.2 2011-01-13 13:49:53 sasa Exp $*/
/*12.10.2010	12.10.2010	Белых А.И.	saldo.h
Реквизиты для работы со списком категорий
*/
class saldo_m_rek
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   
   saldo_m_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
   }
 
 };
