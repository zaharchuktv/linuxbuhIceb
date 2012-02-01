/*$Id: zvb_ukrpromw.h,v 1.2 2011-01-13 13:49:56 sasa Exp $*/
/*19.10.2006	19.10.2006	Белых А.И.	zvb_ukrpromw.h
Реквизиты для выгрузки данных
*/

class zvb_ukrpromw_rek
 {
  public:
   class iceb_u_str nomer_fil;
   class iceb_u_str kod_or;
   class iceb_u_str ima_mah;
   class iceb_u_str ima_set_disk;
   class iceb_u_str parol;
      
  zvb_ukrpromw_rek()
   {
    clear_rek();
   }

  void clear_rek()
   {
    nomer_fil.new_plus("");
    kod_or.new_plus("");
    ima_mah.new_plus("");
    ima_set_disk.new_plus("");
    parol.new_plus("");
    
   }
 };
