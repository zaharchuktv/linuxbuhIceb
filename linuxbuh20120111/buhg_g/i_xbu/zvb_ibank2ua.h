/*$Id: zvb_ibank2ua.h,v 1.2 2011-01-13 13:49:56 sasa Exp $*/
/*16.10.2009	16.10.2009	Белых А.И.	zvb_ibank2ua.h
*/

class zvb_ibank2ua_rek
 {
  public:
   class iceb_u_str datpp;
   class iceb_u_str nomerpp;
   class iceb_u_str meszp;
   class iceb_u_str koment;
      
   zvb_ibank2ua_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    datpp.new_plus("");
    nomerpp.new_plus("");
    meszp.new_plus("");
    koment.new_plus("");
   }
 
 };
