/*$Id: zarsf1dfw.c,v 1.1 2011-03-28 06:55:46 sasa Exp $*/
/*24.03.2011	24.03.2011	Белых А.И.	zarsf1dfw.c
Печать концовки формы 1ДФ
*/

#include "buhg_g.h"

extern SQL_baza bd;

void zarsf1dfw(int kolih_str,int kolih_fiz_os,int kolih_list,int TYP,FILE *ff1,GtkWidget *wpredok)
{
int nomstr=0;

char stroka[2048];
const char *imaf_alx={"zarsf1df_1e.alx"};

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,wpredok);
  return;
 }
class iceb_fioruk_rk rukov;
class iceb_fioruk_rk glavbuh;

iceb_fioruk(1,&rukov,wpredok);
iceb_fioruk(2,&glavbuh,wpredok);

short dt=0,mt=0,gt=0;
iceb_u_poltekdat(&dt,&mt,&gt);
char data_pod[64];
sprintf(data_pod,"%02d.%02d.%04d",dt,mt,gt);
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  nomstr++;
  memset(stroka,'\0',sizeof(stroka)-1);
  strncpy(stroka,row_alx[0],sizeof(stroka)-1);
  switch(nomstr)
   {
    case 2:
     iceb_u_bstab(stroka,kolih_str,19,28,1);
     iceb_u_bstab(stroka,kolih_fiz_os,53,63,1);
     iceb_u_bstab(stroka,kolih_list,80,90,1);
     iceb_u_bstab(stroka,data_pod,101,123,1);
     break;

    case 5: /*Руководитель*/
     if(TYP == 0)
      {
       iceb_u_bstab(stroka,rukov.inn.ravno(),21,322,1);
       iceb_u_bstab(stroka,rukov.famil_inic.ravno(),59,87,1);
       iceb_u_bstab(stroka,rukov.telef.ravno(),89,100,1);
      }
     break;    

    case 10: /*главный бухгалтер*/
     iceb_u_bstab(stroka,glavbuh.inn.ravno(),21,322,1);
     iceb_u_bstab(stroka,glavbuh.famil_inic.ravno(),59,87,1);
     iceb_u_bstab(stroka,glavbuh.telef.ravno(),89,100,1);
     break;    

    case 15: /*самозанятая особа*/
     if(TYP == 1)
      {
       iceb_u_bstab(stroka,rukov.inn.ravno(),21,322,1);
       iceb_u_bstab(stroka,rukov.famil_inic.ravno(),59,87,1);
       iceb_u_bstab(stroka,rukov.telef.ravno(),89,100,1);
      }
     break;    

   }
  fprintf(ff1,"%s",stroka);  
 
 }
 


}
