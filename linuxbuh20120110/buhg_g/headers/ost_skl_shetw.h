/*$Id: ost_skl_shetw.h,v 1.3 2011-01-13 13:49:52 sasa Exp $*/
/*17.11.2008	17.11.2008	Белых А.И.	ost_skl_shetw.h
Данные для получения остатков по складам и счетам учёта
*/

class ost_skl_shetw_data
 {
  public:
   class iceb_u_str sklad;
   class iceb_u_str grupa;
   class iceb_u_str shet;
   class iceb_u_str kodmat;
   class iceb_u_str data_ost;

   iceb_u_spisok imaf;
   iceb_u_spisok naimf;
   
   ost_skl_shetw_data()
    {
     clear_data();
    }
   
   void clear_data()
    {
     sklad.new_plus("");
     grupa.new_plus("");
     shet.new_plus("");
     kodmat.new_plus("");
     data_ost.new_plus("");
    }
 
 };
 
 
 
 
