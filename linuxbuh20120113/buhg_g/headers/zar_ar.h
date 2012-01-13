/*$Id: zar_ar.h,v 1.8 2011-01-13 13:49:55 sasa Exp $*/
/*22.12.2010	19.09.2006	Белых А.И.	zar_ar.h
Реквизиты расчёта зарплаты по всем карточкам
*/

class zar_ar_rek
 {
  public:
   class iceb_u_str datar;
   class iceb_u_str podr;
   class iceb_u_str tabnom;
   class iceb_u_str nah_only;
   class iceb_u_str uder_only;
   short metka_r;   //1-начисления 2-удержания 3-начисления и удержания 4-соц отчисления 5-проводки
   
   class iceb_u_spisok imafr;
   class iceb_u_spisok naimf;
   FILE *ff;   
   short metka_ff; //0-протокол не делать 1-делать
   
   zar_ar_rek()
    {
     clear_rek();
     metka_r=0;
     ff=NULL;
     metka_ff=0;
    }

   void clear_rek()
    {
     datar.new_plus("");
     podr.new_plus("");
     tabnom.new_plus("");
     nah_only.new_plus("");
     uder_only.new_plus("");
    }
 };
