/*$Id: zartarroz1w.c,v 1.10 2011-02-21 07:36:00 sasa Exp $*/
/*09.12.2010	28.11.2008	Белых А.И.	zartarroz1w.c
Работа с тарифной сеткой
*/

#include <errno.h>
#include "buhg_g.h"
#include <unistd.h>

class zartarroz1_data
 {
  public:
   class iceb_u_str kof;
   class iceb_u_int roz;
   class iceb_u_double baza;
   
   void clear()
    {
     kof.new_plus("");
     roz.free_class();
     baza.free_class();
    }
 
 };

extern char *organ;
class zartarroz1_data zar_tark;
extern SQL_baza bd;

int zartarroz1w(GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

zar_tark.clear();


sprintf(strsql,"select str from Alx where fil='zartarroz1.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  return(1);
 }


class iceb_u_str roz;
class iceb_u_str baza;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(iceb_u_polen(row_alx[0],&roz,1,'|') != 0)
   continue;
  if(iceb_u_polen(row_alx[0],&baza,2,'|') != 0)
   continue;

  if(iceb_u_SRAV("Коэффициенты для разрядов",roz.ravno(),0) == 0)
   {
    zar_tark.kof.new_plus(baza.ravno());
    continue;
   }


  if(roz.ravno_atoi() == 0)
   continue;

  zar_tark.roz.plus(roz.ravno_atoi());  
  zar_tark.baza.plus(baza.ravno_atof(),-1);  
 }
return(0);


}
/******************************/
/*Найти нужный тариф*/
/******************************/
double zartarroz1w(int roz,int kof,FILE *ff)
{
double suma=0.;
int nomer_roz=0;
double znah_kof=0;
if(zar_tark.roz.kolih() == 0)
 {
  if(ff != NULL)
   fprintf(ff,"zartarroz1-количество номеров тарифной сетки равно нолю!\n");
  return(0.);
 }  
if(iceb_u_pole2(zar_tark.kof.ravno(),',') == 0)
 {
  if(ff != NULL)
   fprintf(ff,"zartarroz1-количество коэффициентов разрядов равно нолю!\n");
  return(0.);
 }  

if(iceb_u_polen(zar_tark.kof.ravno(),&znah_kof,kof,',') != 0)
 {
  if(ff != NULL)
   fprintf(ff,"zartarroz1-Не найден коэффициент %d в списке коэффициентов!\n",kof);
  return(0.);
 }

if((nomer_roz=zar_tark.roz.find(roz)) < 0)
 {
  if(ff != NULL)
   fprintf(ff,"zartarroz1-Не найден номер тарифа %d списке !\n",roz);
  return(0.);
 }

double suma1=zar_tark.baza.ravno(nomer_roz)*znah_kof;
suma=iceb_u_okrug(suma1,0.01);
if(ff != NULL)
 fprintf(ff,"Вычисляем тариф %f*%f=%f Округляем = %.2f\n",zar_tark.baza.ravno(nomer_roz),znah_kof,suma1,suma);
return(suma);
}
/***************************/
/*Распечатка таблицы*/
/************************/

int zartarroz1w(class iceb_u_str *imaf_ot,GtkWidget *wpredok)
{
zartarroz1w(wpredok); /*Читаем файл настройки*/
char imaf[56];
sprintf(imaf,"zart1%d.lst",getpid());
FILE *ff;

if((ff=fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return(1);
 }
iceb_u_zagolov(gettext("Тарифная таблица"),0,0,0,0,0,0,organ,ff);

int kolih_kof=iceb_u_pole2(zar_tark.kof.ravno(),',');
int kolih_roz=zar_tark.roz.kolih();
class iceb_u_str liniq;

liniq.plus("---------------");
for(int kof=0; kof < kolih_kof; kof++)
 liniq.plus("---------");

fprintf(ff,"%s\n",liniq.ravno());
double dkof=0.;

fprintf(ff,"Номер| База   |");
for(int kof=0; kof < kolih_kof; kof++)
 {
  fprintf(ff,"%-8d|",kof+1);
 } 
fprintf(ff,"\n");
fprintf(ff,"тариф|        |");
for(int kof=0; kof < kolih_kof; kof++)
 {
  iceb_u_polen(zar_tark.kof.ravno(),&dkof,kof+1,',');
  fprintf(ff,"%-8.2f|",dkof);
 } 
fprintf(ff,"\n");
fprintf(ff,"%s\n",liniq.ravno());

double suma=0.;
for(int roz=0; roz < kolih_roz; roz++)
 {
  fprintf(ff,"%5d|%8.2f|",zar_tark.roz.ravno(roz),zar_tark.baza.ravno(roz));
  
  for(int kof=0; kof < kolih_kof; kof++)
   {
    iceb_u_polen(zar_tark.kof.ravno(),&dkof,kof+1,',');
    suma=zar_tark.baza.ravno(roz)*dkof;
    suma=iceb_u_okrug(suma,0.01);
    fprintf(ff,"%8.2f|",suma);    
   }   

  fprintf(ff,"\n");
 }
fprintf(ff,"%s\n",liniq.ravno());
iceb_podpis(ff,wpredok);
fclose(ff);

imaf_ot->new_plus(imaf);

iceb_ustpeh(imaf,0,wpredok);

return(0);
}




