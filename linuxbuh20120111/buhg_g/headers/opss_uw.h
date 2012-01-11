/*$Id: opss_uw.h,v 1.5 2011-01-13 13:49:51 sasa Exp $*/
/*16.06.2006	24.12.2005	Белых А.И.	opss_uw.h
Реквизиты для получения отчёта по счетам списания услуг
*/
class opss_uw_rr
 {
  public:
  class iceb_u_str datan;
  class iceb_u_str datak;
  class iceb_u_str podr;
  class iceb_u_str kodgr;
  class iceb_u_str kodzap;
  class iceb_u_str shet_sp;
  class iceb_u_str shet_uh;
  class iceb_u_str kodop;
  class iceb_u_str kontr;

  iceb_u_spisok imaf;
  iceb_u_spisok naimf;
  short metka; //1-по счетам получения 2-по счетам списания  

  opss_uw_rr()
   {
    clear_data();
   }

  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    podr.new_plus("");
    kodgr.new_plus("");
    kodzap.new_plus("");
    shet_sp.new_plus("");
    shet_uh.new_plus("");
    kodop.new_plus("");
    kontr.new_plus("");

   }
 };
