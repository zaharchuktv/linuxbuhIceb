/* $Id: readpdokw.c,v 1.11 2011-02-21 07:35:57 sasa Exp $ */
/*26.05.2010	07.05.1998	Белых А.И.	readpdokw.c
Чтение реквизитов платежного документа
Вернули 0- если нашли
        1- не найден документ
        2- не найдены свои реквизиты
        3- не найдены чужие реквизиты
        4- не прочтён комментарий
*/
#include <stdlib.h>
#include        <errno.h>
#include        "buhg_g.h"
#include        "dok4w.h"
#include <unistd.h>

extern class REC rec;
extern char     *imafkomtmp; //Имя файла куда выгружается комментарий для редактирования
extern SQL_baza bd;

int readpdokw(const char *tabl,short god,const char *npp,GtkWidget *wpredok)
{
//FILE		*ff;
long		kolstr;
SQL_str         row;
SQLCURSOR cur;
char		strsql[512];

//чистим реквизиты
rec.clear_data();

if(iceb_u_SRAV(tabl,"Pltp",0) == 0 || iceb_u_SRAV(tabl,"Tpltp",0) == 0)
  rec.tipz=2;
if(iceb_u_SRAV(tabl,"Pltt",0) == 0 || iceb_u_SRAV(tabl,"Tpltt",0) == 0)
  rec.tipz=1;

/*Читаем платежку*/
if(iceb_u_SRAV(tabl,"Pltp",0) == 0 || iceb_u_SRAV(tabl,"Pltt",0) == 0)
 {
   sprintf(strsql,"select * from %s where datd >= '%04d-01-01' and \
datd <= '%04d-12-31' and nomd='%s'",
    tabl,god,god,npp);    
 }
else
 {
  sprintf(strsql,"select * from %s where nomd='%s'",
  tabl,npp);    
 }

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найден документ"));
  repl.plus("! ");
  repl.plus(npp);
  repl.plus(" ");
  repl.plus(god);
  repl.plus(" ");
  repl.plus(tabl);
  iceb_menu_soob(&repl,wpredok);  
  return(1);   
 }
iceb_u_rsdat(&rec.dd,&rec.md,&rec.gd,row[0],2);
rec.ddi=rec.dd;
rec.mdi=rec.md;
rec.gdi=rec.gd;

rec.nomdk.new_plus(row[1]);
rec.nomdk_i.new_plus(row[1]);
//memset(rec.nomdk,'\0',sizeof(rec.nomdk));
//strncpy(rec.nomdk,row[1],sizeof(rec.nomdk)-1);

//memset(rec.kodop,'\0',sizeof(rec.kodop));
//strncpy(rec.kodop,row[2],sizeof(rec.kodop)-1);
rec.kodop.new_plus(row[2]);
rec.kodop_i.new_plus(row[2]);

rec.sumd=atof(row[3]);
rec.nds=atof(row[4]);
rec.uslb=atof(row[5]);

//memset(rec.shbzu,'\0',sizeof(rec.shbzu));
//strncpy(rec.shbzu,row[6],sizeof(rec.shbzu)-1);
rec.shbzu.new_plus(row[6]);

if(row[14][0] != '\0')
 iceb_u_rsdat(&rec.dv,&rec.mv,&rec.gv,row[14],2);

//strncpy(rec.kodnr,row[15],sizeof(rec.kodnr)-1);
rec.kodnr.new_plus(row[15]);

/*Читаем реквизиты свои*/
iceb_u_pole(row[7],&rec.kodor,1,'#');

iceb_u_pole(row[7],&rec.naior,2,'#');
iceb_u_pole(row[7],&rec.gorod,3,'#');
iceb_u_pole(row[7],&rec.kod,4,'#');
iceb_u_pole(row[7],&rec.naiban,5,'#');
iceb_u_pole(row[7],&rec.mfo,6,'#');
iceb_u_pole(row[7],&rec.nsh,7,'#');

/*Читаем реквизиты чужие*/
iceb_u_pole(row[8],&rec.kodor1,1,'#');
iceb_u_pole(row[8],&rec.naior1,2,'#');
iceb_u_pole(row[8],&rec.gorod1,3,'#');
iceb_u_pole(row[8],&rec.kod1,4,'#');
iceb_u_pole(row[8],&rec.naiban1,5,'#');
iceb_u_pole(row[8],&rec.mfo1,6,'#');
iceb_u_pole(row[8],&rec.nsh1,7,'#');

rec.podt=atoi(row[9]);
rec.prov=atoi(row[10]);

/*********************
memset(rec.vidpl,'\0',sizeof(rec.vidpl));
strncpy(rec.vidpl,row[11],sizeof(rec.vidpl)-1);
***************/
rec.ktoz=atoi(row[12]);
rec.vremz=atol(row[13]);

/*Пишем комментарий*/
/************************
if((ff = fopen(imafkomtmp,"w")) == NULL)
 {
  iceb_er_op_fil(imafkomtmp,"",errno,wpredok);
  return(1);
 }
************************/
sprintf(strsql,"select zapis from %sz where datd='%04d-%02d-%02d' and \
nomd='%s' and mz=0 order by nz asc",tabl,rec.gd,rec.md,rec.dd,rec.nomdk.ravno());
/*printw("\n%s",strsql);*/
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(4);
 }

//short i=0;
rec.naz_plat.new_plus("");
if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
/*  printw("%s\n",row[0]);*/
//  i++;
//  fprintf(ff,"%s\n",row[0]);
  rec.naz_plat.plus(row[0]);
 }

/*****************
for(; i < 5 ; i++)
    fprintf(ff,"\n");

fprintf(ff,"\
#----------------------------------------------------------------------------\n\
#%s\n",
gettext("Введите назначение платежа и другую необходимую информацию"));

fclose(ff);
*******************/

return(0);   

}
