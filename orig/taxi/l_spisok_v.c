/*$Id: l_spisok_v.c,v 1.9 2011-01-13 13:50:09 sasa Exp $*/
/*10.01.2006	26.02.2004	Белых А.И.	l_spisok_v.c
Выбор товара для выписки счета в ресторане
Если выбрали возвращаем 0
                        1-не выбрали

*/
#include "i_rest.h"

int l_vmat(int *kodv,int kodgr,int,GtkWidget *wpredok);

int  l_spisok_v(int *kodvbr,//код выбранного товара
int *kodvskl, //код выбранного склада
int metka, //0-выбрать надо товар 1-услугу
int kod, // Код склада или подразделения уже выбранный
GtkWidget *wpredok)
{
iceb_u_str kodskl;

iceb_u_str kodv;
kodv.plus("");
kodskl.plus("");
*kodvbr=0;

if(metka == 0)
 {
  if(kod == 0)
   {
    if(l_spisok(0,0,&kodskl,0,wpredok) != 0)
     return(1);
   }
  else 
   kodskl.new_plus(kod);
   
  if(l_spisok(1,kodskl.ravno_atoi(),&kodv,0,wpredok) != 0)
    return(1);
 }

if(metka == 1)
 {
  if(kod == 0)
   {
    if(l_spisok(2,0,&kodskl,0,wpredok) != 0)
      return(1);
   }
  else
   kodskl.new_plus(kod);
  
  if(l_spisok(3,kodskl.ravno_atoi(),&kodv,0,wpredok) != 0)
    return(1);
 }

int kodvv=0;

if(l_vmat(&kodvv,kodv.ravno_atoi(),metka,wpredok) != 0)
  return(1);

*kodvbr=kodvv;
*kodvskl=kodskl.ravno_atoi();

return(0);

}
