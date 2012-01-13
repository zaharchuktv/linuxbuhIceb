/*$Id: ukrspdok1w.h,v 1.2 2011-01-13 13:49:54 sasa Exp $*/
/*09.08.2010	09.08.2010	Белых А.И.	ukrspdok1w.h
Реквизиты для распечатки списка документов по датам приказов
*/

class ukrspdok1w_data
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str kontr;
   class iceb_u_str vidkom;

   class iceb_u_spisok imaf;
   class iceb_u_spisok naim;
   
   ukrspdok1w_data()
    {
     clear();
    }   

   void clear()
    {
     kontr.new_plus("");
     vidkom.new_plus("");
     datan.new_plus("");
     datak.new_plus("");
    }

 };
