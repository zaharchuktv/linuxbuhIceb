/*$Id: mumow.h,v 1.2 2011-01-13 13:49:51 sasa Exp $*/
/*17.12.2009	17.12.2009	Белых А.И.	mumow.h
Класс для получения отчёта 
*/

class mumow_rr
 {
  public:
  class iceb_u_str datan;
  class iceb_u_str datak;
  class iceb_u_str sklad;
  class iceb_u_str grupm;
  class iceb_u_str kodmat;
  class iceb_u_str shet;

  class iceb_u_spisok imaf;
  class iceb_u_spisok naimf;
    
  mumow_rr()
   {
    clear_data();
   }

  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    sklad.new_plus("");
    grupm.new_plus("");
    shet.new_plus("");
    kodmat.new_plus("");
   }
 };
