/* $Id: dolhn1w.c,v 1.11 2011-02-21 07:35:51 sasa Exp $ */
/*22.10.2009	11.03.2000	Белых А.И.	dolhn1w.c
Поиск по табельному номеру нужной строки
Если вернули 0 строку нашли
             1 нет
*/

#include        <errno.h>
#include <unistd.h>
#include        "buhg_g.h"

extern SQL_baza bd;

int dolhn1w(long tabn,//Табельный номер который ищем
double *oklad, //Найденный оклад
float *has,    //Количество часов в одном рабочем дне
float *kof,    //Коэффициент оплаты ставки
int *met,      //0-оклад 1-часовая ставка 2-тарифный розряд
int *koddo,    //Код должносного оклада
int *metka,    //0-нет метки 1-есть Метка пересчета на процент выполнения плана подразделением
int *metka1,   //0-расчет выполнять по отработанным часам 1-дням
char *shet,
int *metka_no,  /*0-оклад пересчитывается на отработанное время 1-нет*/
GtkWidget *wpredok) 
{
char strsql[512];
long		tn;
char		bros[512];
char		bros1[512];

int		i;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;

*metka_no=*metka_no=*koddo=*metka1=*metka=*met=0;
*oklad=*has=0.;
*kof=1.;

sprintf(strsql,"select str from Alx where fil='zardolhn.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  return(1);
 }


while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(iceb_u_polen(row_alx[0],bros,sizeof(bros),1,'|')  == 0)
   {
    if(iceb_u_SRAV(bros,"Код должносного оклада",0) == 0)
     {
      iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
      *koddo=(int)iceb_u_atof(bros);
      continue;
     }


    tn=(long)iceb_u_atof(bros);
    if(tn == tabn)
     {
      iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
      i=strlen(bros);
      *oklad=iceb_u_atof(bros);
//      if(bros[i-1] == 'Ч' || bros[i-1] == 'ч')
      if(iceb_u_strstrm(bros,"Ч") == 1)
	*met=1;
      if(bros[i-1] == 'r' || bros[i-1] == 'R')
	*met=2;
     
      memset(bros,'\0',sizeof(bros));
      iceb_u_polen(row_alx[0],bros,sizeof(bros),3,'|');

      if(bros[0] == '*')
       *metka_no=1;
      else
       {
        if(iceb_u_polen(bros,bros1,sizeof(bros1),1,'*') != 0)
         {
          *has=(float)iceb_u_atof(bros);
          if(iceb_u_polen(bros,bros1,sizeof(bros1),2,':') == 0)
           {
            *has=(*has*60+iceb_u_atof(bros1))/60;
           }
         }
        else
         {
          *has=(float)iceb_u_atof(bros1);
          if(iceb_u_polen(bros,bros1,sizeof(bros1),2,':') == 0)
           {
            *has=(*has*60+iceb_u_atof(bros1))/60;
           }
          iceb_u_polen(bros,bros1,sizeof(bros1),2,'*');
          *kof=(float)iceb_u_atof(bros1);
         }
       }
      iceb_u_polen(row_alx[0],bros,sizeof(bros),4,'|');
      if(iceb_u_strstrm(bros,"*") == 1)
        *metka=1;
      if(iceb_u_strstrm(bros,"Д") == 1)
        *metka1=1;

      iceb_u_pole(row_alx[0],shet,5,'|');
      

      return(0);
     }
   }
 }

return(1);
}

