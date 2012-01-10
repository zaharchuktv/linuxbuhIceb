/*$Id: xrnn_uzzp.h,v 1.2 2011-01-13 13:49:55 sasa Exp $*/
/*07.05.2008	07.05.2008	Белых А.И.	xrnn_uzzp.h
Реквизиты для поиска записей для удаления в реестре налоговых накладных
*/
class xrnn_uzzp_rek
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   
   xrnn_uzzp_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
   }
 
 };
