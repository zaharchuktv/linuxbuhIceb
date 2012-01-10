/*$Id: l_srestdok.h,v 1.12 2011-01-13 13:50:09 sasa Exp $*/
/*09.12.2004	25.02.2004	Белых А.И.	l_srestdok.h

*/

 
class  srestdok_r_data
 {
  public:

  //реквизиты поиска 
  iceb_u_str datan; //дата начала поиска
  iceb_u_str datak; //дата конца поиска
  iceb_u_str nomdokp;
  iceb_u_str nomstol;
  iceb_u_str kodkl;
  iceb_u_str fio;
  iceb_u_str koment;   
  iceb_u_str podr;
  short  metkapoi;
  void clear_data()
   {
    metkapoi=0;
    datan.new_plus("");
    datak.new_plus("");
    nomdokp.new_plus("");
    nomstol.new_plus("");
    kodkl.new_plus("");
    fio.new_plus("");
    koment.new_plus("");
    podr.new_plus("");
   }
   
 };

