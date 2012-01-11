/*$Id: rasoskr.h,v 1.11 2011-01-13 13:49:52 sasa Exp $*/
/*26.11.2010	22.10.2004	Белых А.И.	rasoskr.h
Данные для получения остатков по карточкам
*/

class rasoskr_data
 {
  public:
   class iceb_u_str sklad;
   class iceb_u_str grupa;
   class iceb_u_str shet;
   class iceb_u_str kodmat;
   class iceb_u_str data_ost;
   class iceb_u_str nds;   
   class iceb_u_str naim;
   class iceb_u_str invnom;
   class iceb_u_str kon_dat_is; /*конечная дата использования*/   
   short metka_r; //метка сортировки записей в отчёте
   short metka_ost; //0-без карточек с нулевым остатком
   short metka_innom; //0-все карточки 1-карточки с инвентарными номерами

   iceb_u_spisok imaf;
   iceb_u_spisok naimf;
   
   rasoskr_data()
    {
     clear_data();
    }
   
   void clear_data()
    {
     metka_r=0;
     metka_ost=0;
     metka_innom=0;
     sklad.new_plus("");
     grupa.new_plus("");
     shet.new_plus("");
     kodmat.new_plus("");
     data_ost.new_plus("");
     nds.new_plus("");
     naim.new_plus("");
     invnom.new_plus("");
     kon_dat_is.new_plus("");
    }
 
 };
 
 
 
 
