/*$Id: ukreksim_endw.c,v 1.8 2011-01-13 13:49:54 sasa Exp $*/
/*30.10.2009	25.01.2006	Белых А.И.	ukreksim_endw.c
Концовка формирования распечатки и дискеты для Укрэксимбанка
*/
#include <stdlib.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include        <unistd.h>


void ukreksim_endw(int nomer,const char *imaftmp,char *imafdbf,
double sumai,const char *rukov,const char *glavbuh,FILE *fftmp,FILE *ffr,
GtkWidget *wpredok)
{
char strsql[512];

fputc(26, fftmp);
fprintf(ffr,"\
-----------------------------------------------------------------------\n");
fprintf(ffr,"%*s:%10.2f\n",iceb_u_kolbait(29,"Разом"),"Разом",sumai);
memset(strsql,'\0',sizeof(strsql));

iceb_u_preobr(sumai,strsql,0);

fprintf(ffr,"\nСумма прописом:\n%s\n",strsql);

fprintf(ffr,"\n\
Керівник           _____________________ %s\n\n\
Головний бухгалтер _____________________ %s\n",rukov,glavbuh);
iceb_podpis(ffr,wpredok); 
fclose(fftmp);
fclose(ffr);


sprintf(imafdbf,"OPER.DBF");

ukreksim_hw(imafdbf,nomer);

/*Сливаем два файла*/
iceb_cat(imafdbf,imaftmp,NULL);
unlink(imaftmp);
}
