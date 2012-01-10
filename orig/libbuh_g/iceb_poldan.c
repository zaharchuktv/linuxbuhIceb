/*$Id: iceb_poldan.c,v 1.6 2011-02-21 07:36:07 sasa Exp $*/
/*10.09.2009	27.09.2009	Белых А.И. 	iceb_poldan.c
Поиск настройки в файле и получение значения принадлежащее этой настройке
*/
#include <stdlib.h>
#include "iceb_libbuh.h"






int iceb_poldan(const char *strpoi,class iceb_u_str *find_dat,const char *imaf,GtkWidget *wpredok)
{
char strsql[1024];
SQL_str row;
char prom[4096];
class SQLCURSOR cur;
find_dat->new_plus("");
sprintf(strsql,"select ns,str from Alx where fil='%s' and str like '%s|%%'",imaf,strpoi);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
/**************
  class iceb_u_str repl(strsql);  
  sprintf(strsql,"%s\n%s\n%s",gettext("Ненайдена настройка!"),strpoi,imaf);
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,wpredok);
**************/
  return(1);  
 }
memset(prom,'\0',sizeof(prom));
if(iceb_u_polen(row[1],prom,sizeof(prom),2,'|') == 0)
 {
  if(iceb_u_polen(prom,find_dat,1,'\\') != 0)
   {
    find_dat->new_plus(prom);
    return(0);
   }
  
  int ns=atoi(row[0]);
  while(prom[strlen(prom)-1] == '\\')
   {
    sprintf(strsql,"select ns,str from Alx where fil='%s' and ns=%d",imaf,++ns);

    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
      return(0);    

    if(row[1][0] == '#')
     continue;

    memset(prom,'\0',sizeof(prom));
    strncpy(prom,row[1],sizeof(prom)-1);

    class iceb_u_str prom1("");

    if(iceb_u_polen(prom,&prom1,1,'\\') != 0)
     {
      int dlin=strlen(prom)-1;
      if(prom[dlin] == '\n')
       prom[dlin]='\0';
      find_dat->plus(prom);
      return(0);
     }
    find_dat->plus(prom1.ravno());
   }
  return(0);
 }


return(1);

//return(iceb_u_polen(row[0],find_dat,2,'|'));

}
/********************************/

int iceb_poldan(const char *strpoi,char *find_dat,const char *imaf,GtkWidget *wpredok)
{
class iceb_u_str str("");
find_dat[0]='\0';

if(iceb_poldan(strpoi,&str,imaf,wpredok) != 0)
 return(1);

strcpy(find_dat,str.ravno());
return(0);
}
/********************************/

int iceb_poldan(const char *strpoi,int *find_dat,const char *imaf,GtkWidget *wpredok)
{
class iceb_u_str str("");
*find_dat=0;

if(iceb_poldan(strpoi,&str,imaf,wpredok) != 0)
 return(1);

*find_dat=str.ravno_atoi();
return(0);
}

