/* $Id: sumdokwu.c,v 1.9 2011-02-21 07:35:57 sasa Exp $ */
/*17.11.2008	05.02.1999	Белых А.И.	sumdokwu.c
Вычисление данных по документа
*/
#include        <stdlib.h>
#include	"../headers/buhg_g.h"

extern double   okrg1;
extern SQL_baza bd;
extern double	okrcn;


int sumdokwu(short d,short m,short g,
int skl,
const char *nn,
double *suma, //Сумма по документа
GtkWidget *wpredok)
{
int kolstr;
SQLCURSOR cur;
SQL_str row;

/*Читаем услуги если они есть*/
char strsql[512];
memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"select kolih,cena from Dokummat3 where \
datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",g,m,d,skl,nn);
/*printw("\nstrsql=%s\n",strsql);*/
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

if(kolstr <= 0)
  return(0);
double cena;
double kolih;
double bb;
while(cur.read_cursor(&row) != 0)
 {
  kolih=atof(row[0]);
  cena=atof(row[1]);
  cena=iceb_u_okrug(cena,okrcn);
  
  if(kolih > 0)
    bb=kolih*cena;
  else
    bb=cena;
  bb=iceb_u_okrug(bb,okrg1);
  *suma+=bb;

 } 
return(kolstr);
}
