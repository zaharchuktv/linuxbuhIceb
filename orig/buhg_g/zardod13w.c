/* $Id: zardod13w.c,v 1.7 2011-02-21 07:36:00 sasa Exp $ */
/*20.11.2009	23.10.1999	Белых А.И.	zardod13w.c
Подпрограмма распечатки приложения 13 для пенсионного фонда
*/
#include        <errno.h>
#include        "buhg_g.h"


void zardod13w(const char *inn,/*Индивидуальный налоговый номер*/
const char *fio, /*Фамилия, имя, отчество*/
char kodor[], /*Индетификационный код организации*/
char naimor[], /*Наименование организации*/
char rukov[], /*Руководитель*/
int *kol,     /*Количество записаных в страницу*/
int *nomst,   /*Номер страницы*/
short dd,short md,short gd,/*дата документа*/
short dr,short mr,short gr,//День рождения
short pol_slug, //0-мужчина 1-женщина
FILE *ff13r,FILE *ff13w,FILE *ffvv)
{
short		nomstr;
char		fam[512];
char		ima[32];
char		oth[512];
char		str[1024];
char		bros[512];
char		pol[16];

/*
printw("zardod13 kol=%d nomst=%d\n",*kol,*nomst);
refresh();
*/
if(fio[0] == '\0' && *kol == 0)
  return;

if(fio[0] == '\0' && *kol > 0)
  goto vp;

if(*kol == 0)
 {
  for(nomstr=1; nomstr <= 25; nomstr++)
   {
    memset(str,'\0',sizeof(str));
    fgets(str,sizeof(str),ff13r);
    switch(nomstr)
     {
      case 16:
        iceb_u_bstab(str,kodor,1,21,2);
        break;
      case 19:
        iceb_u_bstab(str,naimor,14,68,1);
        break;
      default:
        break;
     }
    fprintf(ff13w,"%s",str);
   }  
 }

*kol=*kol+1;

memset(fam,'\0',sizeof(fam));
memset(ima,'\0',sizeof(ima));
memset(oth,'\0',sizeof(oth));

iceb_u_pole3(fio,fam,1,' ');
iceb_u_pole3(fio,ima,2,' ');
iceb_u_pole3(fio,oth,3,' ');

memset(str,'\0',sizeof(str));
fgets(str,sizeof(str),ff13r);
iceb_u_bstab(str,inn,4,24,2);

iceb_u_bstab(str,fam,24,45,1);
iceb_u_bstab(str,ima,45,66,1);
iceb_u_bstab(str,oth,66,87,1);
fprintf(ff13w,"%s",str);

fgets(str,sizeof(str),ff13r);
fprintf(ff13w,"%s",str);
memset(pol,'\0',sizeof(pol));
/*********
if(pol_slug == 1)
  pol[0]='Ж';
if(pol_slug == 0)
  pol[0]='Ч';
***********/
if(pol_slug == 1)
  strcpy(pol,"Ж");
if(pol_slug == 0)
  strcpy(pol,"Ч");

fprintf(ffvv,"\"%s\",\"%s\",\"%s\",\"%s\",%d%02d%02d,\"%s\",%d%02d%02d,\"%s\",\"\",\
\"\",\"\",\"\"\n",inn,fam,ima,oth,gr,mr,dr,pol,gd,md,dd,kodor);

if(*kol == 25)
 {
vp:;
  for(nomstr=76-(25-*kol)*2; fgets(str,sizeof(str),ff13r) != NULL ; nomstr++)
   {
    switch(nomstr)
     {
      case 78:
        sprintf(bros,"%4d",*kol);
        iceb_u_bstab(str,bros,1,9,2);
        break;

      case 80:
        sprintf(bros,"%4d",*nomst);
        iceb_u_bstab(str,bros,1,9,2);
        break;

      case 83:
        iceb_u_bstab(str,rukov,58,96,1);
        break;

      case 85:
        sprintf(bros,"%02d %02d %d",dd,md,gd);
        iceb_u_bstab(str,bros,45,65,2);
        break;

      default:
        break;
     }
    fprintf(ff13w,"%s",str);
   }
  *nomst=*nomst+1;
  *kol=0;
  rewind(ff13r);
 }

}
