/*$Id: ukrshkz.h,v 1.2 2011-01-13 13:49:54 sasa Exp $*/
/*28.02.2008	28.02.2008	Белых А.И.	ukrshkz.h
Реквизиты для расчёта отчёта по контрагентам
*/

class ukrshkz_data
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str shet;
   class iceb_u_str kod_zat;
   class iceb_u_str kod_gr_zat;

   class iceb_u_spisok imaf;
   class iceb_u_spisok naim;
   
   ukrshkz_data()
    {
     clear();
    }   

   void clear()
    {
     kod_zat.new_plus("");
     shet.new_plus("");
     datan.new_plus("");
     datak.new_plus("");
     kod_gr_zat.new_plus("");
    }

 };
