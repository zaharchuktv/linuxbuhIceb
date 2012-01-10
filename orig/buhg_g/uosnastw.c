/* $Id: uosnastw.c,v 1.11 2011-02-21 07:35:58 sasa Exp $ */
/*23.04.2010    08.04.1996      Белых А.И.   	uosnastw.c
Чтение настpойки для учета основных сpедств
*/
#include        <errno.h>
#include <unistd.h>
#include        "buhg_g.h"

float           nemi; /*Необлагаемый минимум*/
short		startgoduos; /*Стартовый год*/
short		startgodb; /*Стартовый год просмотров главной книги*/
short		vplsh; /*0-двух-порядковый план счетов 1-многопорядковый*/
char		*bnds1=NULL; /*0% НДС реализация на територии Украины.*/
char            *bnds2=NULL; /*0% НДС экспорт.*/
char            *bnds3=NULL; /*0% Освобождение от НДС статья 5.*/
short		metkabo=0; //Если 1 то организация бюджетная
double okrg1=1.;
double okrcn=1.;
extern SQL_baza bd;

int uosnastw()
{
char            st1[1024];
char		bros[1024];
int		i;

metkabo=vplsh=0;
nemi=0.;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
const char *imaf_alx={"uosnast.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,NULL);
  return(1);
 }


if(bnds1 != NULL)
 {
  delete [] bnds1;
  bnds1=NULL;
 }

if(bnds2 != NULL)
 {
  delete [] bnds2;
  bnds2=NULL;
 }

if(bnds3 != NULL)
 {
  delete [] bnds3;
  bnds3=NULL;
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
   
  iceb_u_polen(row_alx[0],st1,sizeof(st1),1,'|');

  if(iceb_u_SRAV(st1,"Бюджетная организация",0) == 0)
   {
    iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
    if(iceb_u_SRAV(st1,"Вкл",1) == 0)
     metkabo=1;
    continue;
   }
  if(iceb_u_SRAV(st1,"Необлагаемый минимум",0) == 0)
   {
    iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
    nemi=(float)iceb_u_atof(st1);
    continue;
   }

  if(iceb_u_SRAV(st1,"Стартовый год",0) == 0)
   {
    iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
    startgoduos=(short)iceb_u_atof(st1);
    continue;
   }
  if(iceb_u_SRAV(st1,"0% НДС реализация на територии Украины",0) == 0)
    {
     memset(bros,'\0',sizeof(bros));
     iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
     i=strlen(bros)+1;
     bnds1=new char[i*sizeof(char)];
     strcpy(bnds1,bros);
     continue;
    }

  if(iceb_u_SRAV(st1,"0% НДС экспорт",0) == 0)
    {
     memset(bros,'\0',sizeof(bros));
     iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
     i=strlen(bros)+1;
     bnds2=new char[i*sizeof(char)];
     strcpy(bnds2,bros);
     continue;
    }

  if(iceb_u_SRAV(st1,"0% Освобождение от НДС статья 5",0) == 0)
    {
     memset(bros,'\0',sizeof(bros));
     iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|');
     i=strlen(bros)+1;
     bnds3=new char[i*sizeof(char)];
     strcpy(bnds3,bros);
     continue;
    }

 }

memset(st1,'\0',sizeof(st1));

iceb_poldan("Многопорядковый план счетов",st1,"nastrb.alx",NULL);
if(iceb_u_SRAV(st1,"Вкл",1) == 0)
 vplsh=1;

if(iceb_poldan("Округление 1",st1,"matnast.alx",NULL) == 0)
  okrg1=iceb_u_atof(st1);
if(iceb_poldan("Округление цены",st1,"matnast.alx",NULL) == 0)
  okrcn=iceb_u_atof(st1);

return(0);
}
