/*$Id: l_zar_dok.h,v 1.3 2011-01-13 13:49:51 sasa Exp $*/
/*25.10.2006	24.10.2006	Белых А.И.	l_zar_dok.h
Реквизиты для поиска документов в зароботной плате
*/
class l_zar_dok_rek
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str podr;
   class iceb_u_str koment;
   class iceb_u_str nomdok;
   short metka_poi;
   
   l_zar_dok_rek()
    {
     clear_data();
    }

  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    podr.new_plus("");
    koment.new_plus("");
    nomdok.new_plus("");
    metka_poi=0;
   }
 
 };
