/*$Id: l_zar_dok_zap.h,v 1.4 2011-01-13 13:49:51 sasa Exp $*/
/*30.10.2006	30.10.2006	Белых А.И.	l_zar_dok_zap.h
Реквизиты для поиска записей в документе подсистемы "Заработная плата"
*/
class l_zar_dok_zap_rek
 {
  public:
   class iceb_u_str tabnom;
   class iceb_u_str kod_nah;
   class iceb_u_str fio;
   class iceb_u_str shet;
   class iceb_u_str koment;
   short metka_poi;
      
   l_zar_dok_zap_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    tabnom.new_plus("");
    kod_nah.new_plus("");
    fio.new_plus("");
    shet.new_plus("");
    koment.new_plus("");
    metka_poi=0;
   }
 
 };
