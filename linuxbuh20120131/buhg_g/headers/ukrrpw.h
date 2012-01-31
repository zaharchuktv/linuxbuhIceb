/*$Id: ukrrpw.h,v 1.2 2011-01-13 13:49:54 sasa Exp $*/
/*27.02.2008	27.02.2008	Белых А.И.	ukrrpw.h
Реквизиты для расчёта реестра проводок по видам операций
*/

class ukrrpw_data
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str kontr;
   class iceb_u_str vidkom;
   class iceb_u_str shetu;

   class iceb_u_spisok imaf;
   class iceb_u_spisok naim;
   
   ukrrpw_data()
    {
     clear();
    }   

   void clear()
    {
     shetu.new_plus("");
     kontr.new_plus("");
     vidkom.new_plus("");
     datan.new_plus("");
     datak.new_plus("");
    }

 };
