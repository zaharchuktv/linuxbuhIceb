/*$Id: uosprs.h,v 1.3 2011-04-06 09:49:17 sasa Exp $*/
/*05.04.2011	26.12.2007	Белых А.И.	uosprs.h
Реквизиты для расчёта пропорционального распределения сумм
*/

class uosprs_data
 {
  public:
   class iceb_u_str datar;
   class iceb_u_str suma;
   class iceb_u_str innom;
   class iceb_u_str datad;
   class iceb_u_str grup;
   class iceb_u_str shetuh;
   int baza_r;   /*0-Остаточная балансовая стоимость 1-Начальная балансовая стоимость*/
   int sposob_r; /*0-пообъектно 1-по группе*/
   
   class iceb_u_spisok imaf;
   class iceb_u_spisok naim;
   class iceb_u_str kodop;
   float dpnds; 
      
   uosprs_data()
    {
     clear();
    }

   void clear()
    {
     datar.new_plus("");
     suma.new_plus("");
     innom.new_plus("");
     datad.new_plus("");
     baza_r=sposob_r=0;
     grup.new_plus("");
     kodop.new_plus("");
     shetuh.new_plus("");
     dpnds=0.;
    }
 };
