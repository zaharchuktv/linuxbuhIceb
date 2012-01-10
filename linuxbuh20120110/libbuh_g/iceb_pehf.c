/*$Id: iceb_pehf.c,v 1.11 2011-02-21 07:36:07 sasa Exp $*/
/*09.10.2010	14.10.1997	Белых А.И.	iceb_pehf.c
Печать файла на системный или приэкранный принтер
*/
//#include        <stdlib.h>
#include	"iceb_libbuh.h"



void	iceb_pehf(const char *imaf, //Имя файла
short kk,  //0-запрос на количесто >0 введено количество
GtkWidget *wpredok)
{


iceb_print_operation(GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG,imaf,wpredok);

/******************
char print[512];  //Программа печати
short		i2;


if(iceb_poldan("Системный принтер",print,"nastsys.alx",wpredok) != 0)
  printf("iceb_pehf-Не найден \"Системный принтер\" !!!\n");

i2=kk;
if(kk == 0)
 {      
     
  iceb_u_str repl;
  iceb_u_str kolkop;
  repl.plus(gettext("Введите количество копий"));
  if(iceb_menu_vvod1(&repl,&kolkop,10,wpredok) != 0)
   return;
     

  i2=kolkop.ravno_atoi();
  
 }

if(i2 == 0)
 return;
 
iceb_print(imaf,i2,"","",wpredok);
********************/
}

