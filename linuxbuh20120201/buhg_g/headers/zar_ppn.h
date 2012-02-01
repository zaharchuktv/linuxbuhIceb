/*$Id: zar_ppn.h,v 1.2 2011-01-13 13:49:56 sasa Exp $*/
/*30.11.2006	30.11.2006	Белых А.И.	zar_ppn.h
Реквизиты для перерасчёта подоходного налога
*/
class zar_ppn_rek
 {
  public:
   class iceb_u_str mesn;
   class iceb_u_str mesk;
   class iceb_u_str god;
   class iceb_u_str tabnom;
   class iceb_u_str podr;
   
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   zar_ppn_rek()
    {
     clear_data();
     
     short dt,mt,gt;
     iceb_u_poltekdat(&dt,&mt,&gt);
     god.new_plus(gt-1);
     mesn.new_plus("1");
     mesk.new_plus("12");
    }
  
  void clear_data()
   {
    mesn.new_plus("");
    mesk.new_plus("");
    podr.new_plus("");
    tabnom.new_plus("");
    god.new_plus("");
   }
 };
