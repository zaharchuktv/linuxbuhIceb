/* $Id: raspropw.c,v 1.16 2011-08-29 07:13:43 sasa Exp $ */
/*18.12.2009	13.06.2000	Белых А.И.	raspropw.c
Распечатка пропуска для вывоза полученного товара-материалла
*/
#include        <stdlib.h>
#include        <errno.h>
#include	"buhg_g.h"
#include        <unistd.h>

extern double   okrg1; /*Округление*/
extern double	okrcn;  /*Округление цены*/
extern SQL_baza  bd;

void raspropw(short dd,short md,short gd,int skl,const char *nomdok,GtkWidget *wpredok)
{
SQL_str         row;
int		kolstr;
char		strsql[512];
FILE            *ff,*f1;
char		kontr[32];
int		tipz;
char		uspr[512];
char		datop[16];
short		metz;
short		d,m,g;
char		bros[512];
char		naiskl[512];
char		imaf[32],imaf1[32];
char		str[512];
int		kodm;
double		kratn;
char		ein[32];
double		kolih,kolihi;
int		nomerz;
double		mest,mesti;
char		naim[512];
time_t vrem_zap=0;
SQLCURSOR cur;

/*Читаем шапку документа*/
sprintf(strsql,"select * from Dokummat \
where datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",
gd,md,dd,skl,nomdok);

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl;
  sprintf(strsql,"rasdpropw-%s",gettext("Не найден документ !")),
  repl.plus(strsql);
  repl.ps_plus(strsql);
  
  sprintf(strsql,"%s %d.%d.%d %s %s %s %d",
  gettext("Дата"),dd,md,gd,
  gettext("Документ"),nomdok,
  gettext("Слад"),skl);
  
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,wpredok);
  
  return;
 }

strcpy(kontr,row[3]);
tipz=atoi(row[0]);
vrem_zap=atol(row[10]);

sprintf(strsql,"select nomerz,sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d",
gd,nomdok,skl);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

memset(uspr,'\0',sizeof(uspr));
memset(datop,'\0',sizeof(datop));


if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  metz=atoi(row[0]);
  if(metz == 7)
    strncpy(uspr,row[1],sizeof(uspr)-1);
  if(metz == 9)
   {
    iceb_u_rsdat(&d,&m,&g,row[1],0);
    if(d != 0 && iceb_u_sravmydat(d,m,g,dd,md,gd) < 0)
     sprintf(datop,"%02d.%02d.%d",d,m,g);
   }
 }

//iceb_u_imafl(nomdok,strsql);

memset(imaf,'\0',sizeof(imaf));
sprintf(imaf,"prp%ld.lst",vrem_zap);

if((f1 = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }
int kolstrok=0;
rashapw(tipz,&kolstrok,"00",kontr,0,0,f1,wpredok);


/*Читаем наименование склада*/
sprintf(strsql,"select naik from Sklad where kod=%d",skl);
naiskl[0]='\0';

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl; 
  repl.plus(gettext("Не найден склад"));
  repl.plus(" ");
  repl.plus(skl);
  repl.plus(" !");
  iceb_menu_soob(&repl,wpredok);
 }
else
  strncpy(naiskl,row[0],sizeof(naiskl)-1);

fprintf(f1,"\n%s\n\n",naiskl);

fprintf(f1,"         %s N%s\n",
gettext("ПРОПУСК к расходной накладной"),nomdok);

memset(strsql,'\0',sizeof(strsql));
iceb_mesc(md,1,strsql);
fprintf(f1,"\
                      %d %s %d%s\n",
dd,strsql,gd,gettext("г."));

/*Читаем доверенность*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gd,nomdok,skl,1);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  fprintf(f1,gettext("Доверенность"));
  fprintf(f1," N");
  iceb_u_pole(row[0],bros,1,'#');
  fprintf(f1," %s",bros);
  iceb_u_pole(row[0],bros,2,'#');
  fprintf(f1," %s %s",gettext("от"),bros);
 }
fprintf(f1,"\n");


/*Читаем через кого*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gd,nomdok,skl,2);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
  fprintf(f1,"Через кого: %s\n",row[0]);

fprintf(f1,"\x0F"); /*Включение ужатого режима печати*/

fprintf(f1,"\
--------------------------------------------------------------------------------\n");
fprintf(f1,gettext(" N |       Наименование продукции                 |Ед. |Количество| Кол. |Крат-|\n"));
fprintf(f1,gettext("   |              (товару)                        |изм.|          | м-ст |ность|\n"));
fprintf(f1,"\
--------------------------------------------------------------------------------\n");

/*Готовим отсортированный файл*/

sortnakw(dd,md,gd,skl,nomdok,wpredok);
sprintf(imaf1,"sn%d.tmp",getpid());
if((ff = fopen(imaf1,"r")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,wpredok);
  return;
 }
kolihi=mesti=0.;
nomerz=0;
while(fgets(str,sizeof(str),ff) != NULL)
 {
  iceb_u_polen(str,naim,sizeof(naim),2,'|');
  iceb_u_pole(str,bros,3,'|');
  kodm=atoi(bros);

  iceb_u_pole(str,bros,6,'|');
  kratn=atof(bros);

  iceb_u_pole(str,ein,7,'|');

  iceb_u_pole(str,bros,10,'|');
  kolih=atof(bros);
  kolihi+=kolih;
  if(kratn != 0.)
    mest=kolih/kratn;
  else
    mest=0.;
  mest=iceb_u_okrug(mest,0.1);
  mesti+=mest;
  fprintf(f1,"%3d %-*.*s %-*s %10.10g %6.6g %4.4g\n",
  ++nomerz,
  iceb_u_kolbait(46,naim),iceb_u_kolbait(46,naim),naim,
  iceb_u_kolbait(4,ein),ein,
  kolih,mest,kratn);

 }
fprintf(f1,"\
--------------------------------------------------------------------------------\n");
fprintf(f1,"%*s:%10.10g %6.6g\n",
iceb_u_kolbait(55,gettext("Итого")),gettext("Итого"),kolihi,mesti);


fprintf(f1,"\n\%s%s\n",gettext("\
Руководитель__________________        Главный  бухгалтер_______________\n\n"),
gettext("\
Отпустил______________________        Принял__________________________"));

fprintf(f1,"\x12"); /*Отмена ужатого режима печати*/
fprintf(f1,"\x1B\x50"); /*10-знаков*/
fprintf(f1,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
fprintf(f1,"\x1b\x6C%c",1); /*Установка левого поля*/
iceb_podpis(f1,wpredok);
fclose(f1);
fclose(ff);
unlink(imaf1);

iceb_u_spisok imafil;
iceb_u_spisok naimf;
imafil.plus(imaf);
naimf.plus(gettext("Пропуск для вывоза материаллов со склада"));
iceb_ustpeh(imaf,1,wpredok);

iceb_rabfil(&imafil,&naimf,"",0,wpredok);

}
