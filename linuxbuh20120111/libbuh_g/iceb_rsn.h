/*$Id: iceb_rsn.h,v 1.3 2011-01-13 13:49:59 sasa Exp $*/
/*10.04.2004	09.04.2004	Белых А.И.	iceb_rsn.h
Структура системных настроек
*/

class SYSTEM_NAST
 {
  public:
  
  iceb_u_str redaktor;  //программа редактор текстов
  iceb_u_str prog_peh_sys; //программа печати на системном принтере
  iceb_u_str clear_peh;    //программа снятия очереди на печать
  iceb_u_str prosop;         //программа просмотра очереди на печать
  iceb_u_str prosmotr;         //программа просмотра файла

  SYSTEM_NAST()
   {
    redaktor.new_plus("");
    prog_peh_sys.new_plus("");
    clear_peh.new_plus("");
    prosop.new_plus("");
    prosmotr.new_plus("");
   }
 };
