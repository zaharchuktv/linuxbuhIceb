/* $Id: matpom1w.c,v 1.6 2011-04-14 16:09:35 sasa Exp $ */
/*12.04.2011	31.5.2001	Белых А.И.	matpom1w.c
Определение суммы необлагаемой налогом материальной помощи
*/
#include <stdlib.h>
#include        <errno.h>
#include        "buhg_g.h"
#include        "zarp1.h"

extern class ZAR zar;
extern double   snmpz; /*Сумма необлагаемой благотворительной помощи заданная*/
extern char	*shetb; /*Бюджетные счета начислений*/
extern float  kof_prog_min; //Коэффициент прожиточного минимума
extern short  *kn_bsl; //Коды начислений на которые не распространяется льгота по подоходному налогу
extern SQL_baza bd;

double matpom1w(int tbn,short mp,short gp,short kodp[],
FILE *fff,
int metka, //0- в массиве 1-в базе
double *matpomb, //Не облагаемая налогом бюджетная благотворительная помощь
int metkar, //0-не облагается только 1-я, 1-до полной суммы
double *matp, //Сумма благотворительной помощи начисленная в расчетном месяце
double *matpb, //Сумма благотворительной помощи начисленная в расчетном месяце бюджетная
GtkWidget *wpredok)
{
int		i,i1,m;
char		strsql[512];
int		kolstr=0;
SQL_str		row;
char		bros[512];
double          dohod=0.;

*matp=0.; //Сумма благотворительной помощи начисленная в расчетном месяце
*matpb=0.; //Сумма благотворительной помощи начисленная в расчетном месяце бюджетная

//if(fff != NULL)
// fprintf(fff,"Проверяем благотворительную помощь %d %d.%d\n",tbn,mp,gp);

*matpomb=0.;

/*Опpеделение матеpиальной помощи в массиве*/
if(metka == 0)
for(i=0; i < razm; i++)
 {
  if( zar.prnu[i] == 0 )
   break;
  if(zar.prnu[i] == 1)
   {
    if(provkodw(kn_bsl,zar.knu[i]) < 0)
      dohod+=zar.sm[i];
    if(provkodw(kodp,zar.knu[i]) >= 0)
     {
      if(iceb_u_sravmydat(zar.denz[i],mp,gp,19,5,2001) < 0)
        continue;
      *matp+=zar.sm[i];
      if(iceb_u_proverka(shetb,zar.sheta[i],0,1) == 0) 
        *matpb+=zar.sm[i];
     }
   }
 }

SQLCURSOR cur;

if(metka == 1)
 {
  sprintf(strsql,"select knah,suma,shet from Zarp where \
datz >= '%d-%02d-01' and datz <= '%d-%02d-31' and tabn=%d and \
prn='%d' and suma <> 0.",
  gp,mp,gp,mp,tbn,1);

//  if(fff != NULL)
//   fprintf(fff,"%s\n",strsql);
   
//  printw("\n%s\n",strsql);
//  refresh();
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    sprintf(bros,"matpom1/1-%s",gettext("Ошибка создания курсора !"));
    iceb_msql_error(&bd,bros,strsql,wpredok);
   }

  while(cur.read_cursor(&row) != 0)
   {
    if(provkodw(kn_bsl,atoi(row[0])) < 0)
      dohod+=atof(row[1]);
//    printw("%s %s\n",row[0],row[1]);
    if(fff != NULL)
     fprintf(fff,"%s %s %s\n",row[0],row[1],row[2]);    
    if(provkodw(kodp,atoi(row[0])) >= 0)
     {
      *matp+=atof(row[1]);
      if(iceb_u_proverka(shetb,row[2],0,1) == 0)
        *matpb+=atof(row[1]);
     }
   }
 }


//printw("matp=%f\n",matp);
//OSTANOV();

if(*matp == 0.)
  return(0.);

class zar_read_tn1h nastr;
zar_read_tn1w(1,mp,gp,&nastr,NULL,wpredok);

double maxsum=nastr.prog_min_ng*kof_prog_min;
maxsum=iceb_u_okrug(maxsum,10.);

if(dohod-*matp > maxsum)
 {
  if(fff != NULL)
   fprintf(fff,"Доход %.2f-%.2f=%.2f > %.2f благотворительная помощ облагается !\n",
   dohod,*matp,dohod-*matp,maxsum);
  return(0.);   
   
 }

if(fff != NULL)
  fprintf(fff,"%s:%s/%.2f\n","Благотворительная помощь",iceb_u_prnbr(*matp),*matpb);

double matpo=0.;
for(m=1; m < mp ; m++)
for(i1=1; i1 <= kodp[0]; i1++)
 {
  sprintf(strsql,"select suma,datz from Zarp where \
datz >= '%d-%02d-01' and datz <= '%d-%02d-31' and tabn=%d and \
prn='%d' and knah=%d and suma <> 0.",
  gp,m,gp,m,tbn,1,kodp[i1]);

//  printw("%s\n",strsql);
//  refresh();
/*

  if(fff != NULL)
   fprintf(fff,"%s\n",strsql);
*/   
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    sprintf(bros,"matpom1/2-%s",gettext("Ошибка создания курсора !"));
    iceb_msql_error(&bd,bros,strsql,wpredok);
    continue;
   }
  if(kolstr == 0)
    continue;

  while(cur.read_cursor(&row) != 0)
   {
//    if(fff != NULL)
//     fprintf(fff,"matpom-%s %s\n",row[1],row[0]) ;
    matpo+=atof(row[0]);
   }

 }

if(metkar == 0) //Первая не облагается остальные облагаются
 {
  if(matpo != 0.)//Была уже значит, облагается
   {
    if(fff != NULL)
     fprintf(fff,"Благотворительной помощь была раньше, поэтому вся облагается налогом.\n");
    return(0.);
   }

 if(*matp > snmpz)
  {
   if(fff != NULL)
    fprintf(fff,"Сумма не облагаемой благотворительной помощи:%.2f\n",snmpz);
   *matpomb=snmpz*(*matpb)/(*matp);
   return(snmpz);
  }
 else
  {
   if(fff != NULL)
    fprintf(fff,"Сумма не облагаемой благотворительной помощи:%.2f\n",*matp);
   *matpomb=*matpb;
   return(*matp);
  }
 }

if(metkar == 1) 
 {
    if(fff != NULL)
       fprintf(fff,"%.2f %.2f %.2f\n",*matp,matpo,snmpz);
  if(*matp+matpo > snmpz)
   {

    if(matpo > snmpz)
     {
      if(fff != NULL)
       fprintf(fff,"Благотворительной помощь облагается налогом.\n");
      return(0.);
     }
    else
     {
      double snmp=snmpz-matpo;
      if(fff != NULL)
       fprintf(fff,"1Сумма не облагаемой благотворительной помощи:%.2f\n",snmp);
      *matpomb=snmp*(*matpb)/(*matp);
      return(snmp);
      
     }
   }
  else
   {
    if(fff != NULL)
     fprintf(fff,"2Сумма не облагаемой благотворительной помощи:%.2f\n",*matp);

    *matpomb=*matpb;
    return(*matp);
    
   }   
 }
return(0.); 
}
