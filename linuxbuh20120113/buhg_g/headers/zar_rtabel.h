/*$Id: zar_rtabel.h,v 1.2 2011-01-13 13:49:56 sasa Exp $*/
/*14.12.2006	14.12.2006	Белых А.И.	zar_rtabel.h
Реквизиты для распечатки заготовки табеля
*/
class zar_rtabel_rek
 {
  public:
   class iceb_u_str podr;
   
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   zar_rtabel_rek()
    {
     clear_data();
    }
  
  void clear_data()
   {
    podr.new_plus("");
   }
 };
