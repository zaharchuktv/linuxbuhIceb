/*$Id: ukrprik.h,v 1.2 2011-01-13 13:49:54 sasa Exp $*/
/*28.02.2008	28.02.2008	Белых А.И.	ukrprik.h
Реквизиты для распечатки приказов на командировку
*/

class ukrprik_data
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;

   class iceb_u_spisok imaf;
   class iceb_u_spisok naim;
   
   ukrprik_data()
    {
     clear();
    }   

   void clear()
    {
     datan.new_plus("");
     datak.new_plus("");
    }

 };
