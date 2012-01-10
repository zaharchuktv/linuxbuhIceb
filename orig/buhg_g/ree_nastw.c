/*$Id: ree_nastw.c,v 1.9 2011-08-29 07:13:44 sasa Exp $*/
/*23.07.2011	30.08.2005	Белых А.И.	ree_nastw.c
Чтение необходимых настроек для работы реестра налоговых накладных
*/
#include        <errno.h>
#include <unistd.h>
#include	"buhg_g.h"



double		okrcn=0.;  /*Округление цены*/
double		okrg1=0.;  /*Округление 1*/
short           startgodb=0; //Стартовый год для подсистемы "Главная книга"
short		vplsh; /*0-двух уровневый план счетов 1-многоуровневый*/
extern SQL_baza bd;
class iceb_rnfxml_data rek_zag_nn; /*настройки для создания xml файлов*/

short dp=0,mp=0,gp=0; //Дата начала просмотра реестров

int ree_nastw()
{
char st1[1024];

okrg1=okrcn=0.;

memset(st1,'\0',sizeof(st1));
startgodb=0;
if(iceb_poldan("Стартовый год",st1,"nastrb.alx",NULL) == 0)
 {
  startgodb=(short)iceb_u_atof(st1);
 }

vplsh=0;
memset(st1,'\0',sizeof(st1));
iceb_poldan("Многопорядковый план счетов",st1,"nastrb.alx",NULL);
if(iceb_u_SRAV(st1,"Вкл",1) == 0)
 vplsh=1;

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='matnast.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"matnast.alx");
  iceb_menu_soob(strsql,NULL);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  iceb_u_polen(row_alx[0],st1,sizeof(st1),1,'|');

  if(iceb_u_SRAV(st1,"Округление 1",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     okrg1=iceb_u_atof(st1);
     continue;
    }
  if(iceb_u_SRAV(st1,"Округление цены",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     okrcn=iceb_u_atof(st1);
     continue;
    }
 }

sprintf(strsql,"select str from Alx where fil='rnn_nast.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"rnn_nast.alx");
  iceb_menu_soob(strsql,NULL);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  iceb_u_polen(row_alx[0],st1,sizeof(st1),1,'|');

  if(iceb_u_SRAV(st1,"Дата начала просмотра реестра",0) == 0)
    {
     iceb_u_polen(row_alx[0],st1,sizeof(st1),2,'|');
     iceb_u_rsdat(&dp,&mp,&gp,st1,1);
     continue;
    }
 }
iceb_rnfxml(&rek_zag_nn,NULL);
return(0);

}
