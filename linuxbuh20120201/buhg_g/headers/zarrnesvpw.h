/*$Id: zarrnesvpw.h,v 1.2 2011-05-06 08:37:47 sasa Exp $*/
/*28.04.2011	08.04.2011	Белых А.И.	zarrnesvpw.h
Чтение процентов для расчёта удержаний с работника
*/

class zarrnesvp_rek
 {
  public:
   float proc_esv; /*процент единого социального взноса с работника*/
   float proc_esv_bol; /*процент единого социального взноса с работника*/
   float proc_esv_inv; /*процент единого социального взноса с работника*/
   float proc_esv_dog; /*процент единого социального взноса с работника*/
   float proc_esv_vs;  
  zarrnesvp_rek()
   {
    clear();
   }

  void clear()
   {
    proc_esv=0.; /*процент единого социального взноса с работника*/
    proc_esv_bol=0.; /*процент единого социального взноса с работника*/
    proc_esv_inv=0.; /*процент единого социального взноса с работника*/
    proc_esv_dog=0.; /*процент единого социального взноса с работника*/
    proc_esv_vs=0.;
   }
 };