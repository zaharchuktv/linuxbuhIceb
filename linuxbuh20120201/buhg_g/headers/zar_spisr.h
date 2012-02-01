/*$Id: zar_spisr.h,v 1.4 2011-01-13 13:49:56 sasa Exp $*/
/*08.05.2009	22.11.2006	Белых А.И.	zar_spisr.h
Реквизиты для распечатки списка работников
*/
class zar_spisr_rek
 {
  public:
   class iceb_u_str podr;
   class iceb_u_str tabnom;
   class iceb_u_str kod_zvan;
   class iceb_u_str kod_kateg;
   class iceb_u_str shetu;
         
   class iceb_u_spisok imaf;
   class iceb_u_spisok naimf;
   
   short metka_ras; //0-список всех работающих 1-всех уволенных 2-совместителей
   short metka_sort; //0-по табельным номерам 1-по фамилиям
   zar_spisr_rek()
    {
     clear_data();
     metka_ras=0;
     metka_sort=0;
    }
  
  void clear_data()
   {
    podr.new_plus("");
    tabnom.new_plus("");
    kod_zvan.new_plus("");
    kod_kateg.new_plus("");
    shetu.new_plus("");
   }
 };
