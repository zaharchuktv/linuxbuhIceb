/*$Id: ukrspdokw.h,v 1.2 2011-01-13 13:49:54 sasa Exp $*/
/*29.02.2008	29.02.2008	Белых А.И.	ukrspdokw.h
Реквизиты для распечатки списка документов
*/

class ukrspdokw_data
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str kontr;
   class iceb_u_str vidkom;
   class iceb_u_str kod_zat;

   class iceb_u_spisok imaf;
   class iceb_u_spisok naim;
   
   ukrspdokw_data()
    {
     clear();
    }   

   void clear()
    {
     kod_zat.new_plus("");
     kontr.new_plus("");
     vidkom.new_plus("");
     datan.new_plus("");
     datak.new_plus("");
    }

 };
