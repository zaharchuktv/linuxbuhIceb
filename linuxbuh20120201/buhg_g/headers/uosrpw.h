/*$Id: uosrpw.h,v 1.2 2011-01-13 13:49:55 sasa Exp $*/
/*02.01.2008	31.12.2007	Белых А.И.	uosrpw.h
Реквизиты для расчёта реестра проводок по видам операзий
*/

class uosrpw_data
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str kontr;
   class iceb_u_str kod_op;
   class iceb_u_str podr;
   class iceb_u_str shetu;
   int prih_rash; /*0-всё 1-приход 2-расход*/

   class iceb_u_spisok imaf;
   class iceb_u_spisok naim;
   
   uosrpw_data()
    {
     clear();
    }   

   void clear()
    {
     podr.new_plus("");
     shetu.new_plus("");
     kontr.new_plus("");
     kod_op.new_plus("");
     datan.new_plus("");
     datak.new_plus("");
     prih_rash=0;
    }

 };
