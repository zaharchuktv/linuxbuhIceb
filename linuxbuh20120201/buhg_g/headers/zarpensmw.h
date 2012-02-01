/*$Id: zarpensmw.h,v 1.2 2011-01-13 13:49:56 sasa Exp $*/
/*09.09.2008	10.03.2006	Белых А.И.	zarpensmw.h

*/

class zarpensmw_rr
 {
  public:
  class iceb_u_str datar;
  class iceb_u_str tabnom;
  
  class iceb_u_spisok imaf;
  class iceb_u_spisok naimf;
    
  zarpensmw_rr()
   {
    clear_data();
   }

  void clear_data()
   {
    datar.new_plus("");
    tabnom.new_plus("");  
   }
 };
