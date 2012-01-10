/*$Id: l_f1df.h,v 1.2 2011-01-13 13:49:50 sasa Exp $*/
/*21.12.2006	21.12.2006	Белых А.И.	l_f1df.h
Реквизиты для воода шапки формы 1ДФ
*/
class l_f1df_rek
 {
  public:
   class iceb_u_str god;
   class iceb_u_str kvrt;
   class iceb_u_str nomdok;
   class iceb_u_str datd;
   short metka_vd; //0-отчётный 1-новый отчётный 2-уточняющий
   class iceb_u_str nomdok_dpa;
   class iceb_u_str datd_dpa;
      
   l_f1df_rek()
    {
     clear_data();
    }
  
  void clear_data()
   {
    god.new_plus("");
    kvrt.new_plus("");
    nomdok.new_plus("");
    datd.new_plus("");
    metka_vd=0;
    nomdok_dpa.new_plus("");
    datd_dpa.new_plus("");
   }
 };
