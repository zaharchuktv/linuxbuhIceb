/*$Id: iceb_salorok.h,v 1.2 2011-01-13 13:49:59 sasa Exp $*/
/*27.02.2008	27.02.2008	Белых А.И.	iceb_salorok.h
Реквизиты для расчёта реестра проводок по видам операций
*/

class iceb_salorok_data
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str kontr;
   class iceb_u_str shet;
   int metka_prov; /*0-все проводки 1-без взаимоно кореспондерующих*/
   
   class iceb_u_spisok imaf;
   class iceb_u_spisok naim;
   
   iceb_salorok_data()
    {
     clear();
    }   

   void clear()
    {
     metka_prov=0;
     shet.new_plus("");
     kontr.new_plus("");
     datan.new_plus("");
     datak.new_plus("");
    }

 };
