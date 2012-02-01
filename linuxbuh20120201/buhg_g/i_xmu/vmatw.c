/*$Id: vmatw.c,v 1.24 2011-02-21 07:35:58 sasa Exp $*/
/*11.11.2008	09.07.2004	Белых А.И.	vmatw.c
Ввод материалла в документ
*/
#include <ctype.h>
#include <stdlib.h>
#include "../headers/buhg_g.h"

extern SQL_baza  bd;

int vkartm(short dd,short md,short gd,const char *nomdok,int tipz,int skl,int kodm,double,double,GtkWidget *wpredok);

int vmatw(int tipz, //1-приход 2-расход
short dd,short md,short gd, //Дата документа
const char *nomdok, //Номер документа
int skl,
int *kodmat_v, //введенный код материалла
float pnds,
GtkWidget *wpredok)
{
iceb_u_str repl;
iceb_u_str kodmat;
iceb_u_str kodmatv;
iceb_u_str naim;
int skan=0;

repl.plus(gettext("Введите код или наименование материалла"));
if((skan=iceb_menu_vvod1(&repl,&kodmat,40,wpredok)) != 0)
 return(1);

//printf("vmatw-kodmat=%s\n",kodmat.ravno());
kodmatv.new_plus(kodmat.ravno());

if(kodmat.getdlinna() <= 1 || isdigit(kodmat.ravno()[0]) == 0)
 {
  kodmatv.new_plus("");
  char bros[512];
  memset(bros,'\0',sizeof(bros)); 
  if(iceb_u_pole(kodmat.ravno(),bros,2,'+') != 0)
    strcpy(bros,kodmat.ravno());
  naim.new_plus(bros);
//  printf("vmatw-%s\n",naim.ravno());
  int i=0;
  int metka_rr=0;
  if(tipz == 2)
   metka_rr=1;
  if((i=l_mater(1,&kodmatv,&naim,skl,metka_rr,wpredok)) != 0)
   {
//    printf("vmatw-Выбрано1=%s %s\n",kodmatv.ravno(),naim.ravno());
    
    if( i == GDK_KP_0)
     {
    
      i=vbrpw(dd,md,gd,skl,nomdok,kodmatv.ravno_atoi(),tipz,wpredok);
    
      return(i);
     }     
    return(1);
   }
//  printf("vmatw-Выбрано=%s %s\n",kodmatv.ravno(),naim.ravno());
 }
 
char strsql[512];
SQL_str row;
SQLCURSOR cur;

/*Читаем материал*/
if(skan != -1) 
  sprintf(strsql,"select kodm from Material where kodm=%d",kodmatv.ravno_atoi());
else
  sprintf(strsql,"select kodm from Material where strihkod=%s",kodmatv.ravno());

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  
  if(skan != -1) 
    sprintf(strsql,"%s %d !",gettext("Не найден код материалла"),kodmat.ravno_atoi());
  else
    sprintf(strsql,"%s %s !",gettext("Не найден штрих код"),kodmat.ravno());
  
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

int kodm=atoi(row[0]);
*kodmat_v=kodm;
iceb_u_str nomz;
nomz.plus("");
int n_kart=0;
int voz=0;

if((voz=l_kartmatv(tipz,dd,md,gd,nomdok,0.,&nomz,kodm,skl,&n_kart,0.,0,pnds,wpredok)) == 0)
  return(0); //карточка выбрана или введена новая для прихода

if(voz == 2) //ввод материалла без привязки к карточке
   return(vbrpw(dd,md,gd,skl,nomdok,kodmatv.ravno_atoi(),tipz,wpredok));

if(voz == -1) //ввод новой карточки
 if(vkartm(dd,md,gd,nomdok,tipz,skl,kodm,0.,0.,wpredok) == 0)
   return(0);

return(1);
}
