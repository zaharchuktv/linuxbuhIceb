/*$Id: uosrbsw.h,v 1.2 2011-01-13 13:49:55 sasa Exp $*/
/*15.01.2008	15.01.2008	Белых А.И.	uosrbsw.h
Реквизиты для расчёта балансовой стоимости по мат.ответственным и счетам учёта
*/

class uosrbsw_data
 {
  public:
   class iceb_u_str datak;
   class iceb_u_str mat_ot;
   class iceb_u_str shetu;

   class iceb_u_spisok imaf;
   class iceb_u_spisok naim;
   
   uosrbsw_data()
    {
     clear();
    }   

   void clear()
    {
     datak.new_plus("");
     mat_ot.new_plus("");
     shetu.new_plus("");
    }

 };
