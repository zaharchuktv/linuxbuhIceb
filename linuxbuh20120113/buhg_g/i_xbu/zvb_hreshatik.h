/*$Id: zvb_hreshatik.h,v 1.2 2011-01-13 13:49:56 sasa Exp $*/
/*16.10.2009	16.10.2009	Белых А.И.	zvb_hreshatik.h
*/

class zvb_hreshatik_rek
 {
  public:
   class iceb_u_str datpp;
   class iceb_u_str nomerpp;
   class iceb_u_str meszp;
   
   zvb_hreshatik_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    datpp.new_plus("");
    nomerpp.new_plus("");
    meszp.new_plus("");
   }
 
 };
