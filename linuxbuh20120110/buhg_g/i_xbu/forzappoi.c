/*$Id: forzappoi.c,v 1.12 2011-01-13 13:49:50 sasa Exp $*/
/*18.01.2010	11.01.2004	Белых А.И.	forzappoi.c
Формирование запроса для поиска проводок
*/
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "prov_poi.h"

void	forzap(iceb_u_str*,const char *,const char *,const char *);

void forzappoi(class prov_poi_data *data)
{
short dn=0,mn=0,gn=0;
short dk=0,mk=0,gk=0;
short dnz=0,mnz=0,gnz=0;
short dkz=0,mkz=0,gkz=0;
char strsql[512];

iceb_u_rsdat(&dn,&mn,&gn,data->datan.ravno(),1);
iceb_u_rsdat(&dk,&mk,&gk,data->datak.ravno(),1);

iceb_u_rsdat(&dnz,&mnz,&gnz,data->datanz.ravno(),1);
iceb_u_rsdat(&dkz,&mkz,&gkz,data->datakz.ravno(),1);

if(dk == 0)
 {
  dk=dn;
  mk=mn;
  gk=gn;
  iceb_u_dpm(&dk,&mk,&gk,5); //Получаем последний день месяца
 }

if(dkz == 0)
 {
  dkz=dnz;
  mkz=mnz;
  gkz=gnz;
  iceb_u_dpm(&dkz,&mkz,&gkz,5); //Получаем последний день месяца
 }

data->zapros.new_plus("select * from Prov where ");

if(dk == 0)
 sprintf(strsql," datp >= '%04d-%02d-%02d'",gn,mn,dn);
else
 sprintf(strsql," datp >= '%04d-%02d-%02d' and datp <= '%04d-%02d-%02d'",
 gn,mn,dn,gk,mk,dk);

data->zapros.plus(strsql);

forzap(&data->zapros,data->shet.ravno_filtr(),data->shetk.ravno_filtr(),data->kontr.ravno_filtr());

  

time_t vrem=0;

vrem=iceb_u_dat_to_sec(dnz,mnz,gnz);

sprintf(strsql," and vrem >= %ld",vrem);

data->zapros.plus(strsql);
if(dkz != 0)
 {
  struct tm bf;
  bf.tm_mday=(int)dkz;
  bf.tm_mon=(int)(mkz-1);
  bf.tm_year=(int)(gkz-1900);
  bf.tm_hour=(int)23;

  bf.tm_min=(int)59;
  bf.tm_sec=59;

  time_t vremk=0;

  vremk=mktime(&bf);
  
  sprintf(strsql," and vrem <= %ld",vremk);

  data->zapros.plus(strsql);
 }    


forzap(&data->zapros,data->shet.ravno_filtr(),data->shetk.ravno_filtr(),data->kontr.ravno_filtr());

data->zapros.plus(" order by datp,nomd asc");

//printf("forzappoi-%s\n",data->zapros.ravno());


}

/***********************************/
/*Формирование запроса             */
/***********************************/
void forzap(iceb_u_str *strsql,const char *sh,const char *shk,const char *kor)
{
char		bros[312];
char		bros1[312];
int		kolshet=0;

if(sh[0] != '\0')
 {
  kolshet=iceb_u_pole2(sh,',');

  if(kolshet == 0)
   {
    sprintf(bros," and sh like '%s%%'",sh);
    strsql->plus(bros);
   }
  else
   {
    for(int i1=1; i1 <= kolshet;i1++)
     if(iceb_u_polen(sh,bros,sizeof(bros),i1,',') == 0)
      {
       if(bros[0] == '\0')
        break;
       if(i1 == 1)
        {
         if(sh[0] != '-')
          sprintf(bros1," and (sh like '%s%%'",bros);
         else
          sprintf(bros1," and (sh not like '%s%%'",bros);
        }
       else          
        {
         if(sh[0] != '-')
           sprintf(bros1," or sh like '%s%%'",bros);
         else
           sprintf(bros1," and sh not like '%s%%'",bros);
        }
       strsql->plus(bros1);
      }        
    strsql->plus(")");
   }
 }

if(shk[0] != '\0')
 {
  kolshet=iceb_u_pole2(shk,',');

  if(kolshet == 0)
   {
    sprintf(bros," and shk like '%s%%'",shk);
    strsql->plus(bros);
   }
  else
   {
    for(int i1=1; i1 <= kolshet;i1++)
     if(iceb_u_polen(shk,bros,sizeof(bros),i1,',') == 0)
      {
       if(bros[0] == '\0')
        break;
       if(i1 == 1)
        {
         if(shk[0] != '-')
          sprintf(bros1," and (shk like '%s%%'",bros);
         else
          sprintf(bros1," and (shk not like '%s%%'",bros);
        }
       else          
        {
         if(shk[0] != '-')
          sprintf(bros1," or shk like '%s%%'",bros);
         else
          sprintf(bros1," and shk not like '%s%%'",bros);
        }
       strsql->plus(bros1);
      }        
    strsql->plus(")");
   }
 }

if(kor[0] != '\0')
 {
  kolshet=iceb_u_pole2(kor,',');

  if(kolshet == 0)
   {
    sprintf(bros," and kodkon='%s'",kor);
    strsql->plus(bros);
   }
  else
   {
    for(int i1=1; i1 <= kolshet;i1++)
     if(iceb_u_polen(kor,bros,sizeof(bros),i1,',') == 0)
      {
       if(bros[0] == '\0')
        break;
       if(i1 == 1)
        {
         if(kor[0] != '-')
          sprintf(bros1," and (kodkon='%s'",bros);
         else
          sprintf(bros1," and (kodkon <> '%s'",bros);
        }
       else          
        {
         if(kor[0] != '-')
          sprintf(bros1," or kodkon='%s'",bros);
         else
          sprintf(bros1," and kodkon <> '%s'",bros);
        }
       strsql->plus(bros1);
      }        
    strsql->plus(")");
   }
 }
}
