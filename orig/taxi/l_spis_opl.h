/*$Id: l_spis_opl.h,v 1.6 2011-01-13 13:50:09 sasa Exp $*/
/*05.02.2006	25.01.2006	Белых А.И.	l_spis_opl.h
Реквизиты поиска записей оплаты
*/

class l_spis_opl_rek
 {
  public:
  static iceb_u_str datan;
  static iceb_u_str vremn;
  static iceb_u_str datak;
  static iceb_u_str vremk;

  static iceb_u_str kodkl;
  static iceb_u_str kassa;    
  static iceb_u_str suma;
  static iceb_u_str nomdok;
  static iceb_u_str datad;
  static iceb_u_str podr;
  static iceb_u_str koment;
  static short metka_nal; //0-Наличку показывать 1-не показывать    
  static short metka_bnal; //0-Без нал. показывать 1-не показывать    
  static short metka_spis; //0-Списание показывать 1-не показывать    
  
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    vremn.new_plus("");
    vremk.new_plus("");
    kodkl.new_plus("");
    kassa.new_plus("");
    suma.new_plus("");
    nomdok.new_plus("");
    datad.new_plus("");
    podr.new_plus("");
    koment.new_plus("");
    metka_nal=metka_bnal=metka_spis=0;
   }
 
 };
