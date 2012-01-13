/*$Id: iceb_dikont.c,v 1.12 2011-02-21 07:36:06 sasa Exp $*/
/*11.11.2008	24.11.2000	Белых А.И.	iceb_dikont.c
Ввод дополнительной информации к коду
*/
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <errno.h>
#include	"iceb_libbuh.h"
#include        <unistd.h>

extern SQL_baza	bd;

int iceb_dikont(const char mr, //+ загрузить - выгрузить
const char *imaf, //имя файла
const char *kod,
const char *tabl) //Имя таблицы
{
int		nomz;
char		strok[1024];
char		strsql[512];
SQL_str  	row;
FILE		*ff;
int		kolstr,i;
/*
printw("\ndikont-%c %s %s %s\n",mr,imaf,kod,tabl);
refresh();
*/
if(mr == '+')
 {

  if((ff = fopen(imaf,"r")) == NULL)
   {
     if(errno == ENOENT)
      return(0);
     else
      {
       iceb_er_op_fil(imaf,"iceb_dikont-Ошибка открытия файла",errno,NULL);
       return(1);
      }
   }

  sprintf(strsql,"LOCK TABLES %s WRITE",tabl);
  if(sql_zap(&bd,strsql) != 0)
   {
    iceb_u_str rep;

    sprintf(strsql,"dikont-%s\n%s\n",
    gettext("Ошибка блокировки таблицы !"),
    sql_error(&bd));
    rep.plus(strsql);
    iceb_menu_soob(&rep,NULL);
   }

  if(iceb_u_SRAV(tabl,"Kontragent1",0) == 0)
    sprintf(strsql,"delete from %s where kodkon='%s'",
    tabl,kod);

  if(iceb_u_SRAV(tabl,"Uosin1",0) == 0)
    sprintf(strsql,"delete from %s where innom=%s",
    tabl,kod);
  if(iceb_u_SRAV(tabl,"Kartb1",0) == 0)
    sprintf(strsql,"delete from %s where tabn=%s",
    tabl,kod);

  if(sql_zap(&bd,strsql) != 0)
   {
    sprintf(strok,"dikont-%s",gettext("Ошибка удаления записи !"));
    iceb_msql_error(&bd,strok,strsql,NULL);
   }
  nomz=0;
  while(fgets(strok,sizeof(strok),ff) != NULL)
   {
    i=strlen(strok);
    if(strok[i-1] == '\n')
      strok[i-1]='\0';
    
    sqlfiltr(strok,sizeof(strok));
              
    if(iceb_u_SRAV(tabl,"Kontragent1",0) == 0)
      sprintf(strsql,"insert into %s \
values ('%s',%d,'%s')",tabl,kod,++nomz,strok);

    if(iceb_u_SRAV(tabl,"Uosin1",0) == 0)
      sprintf(strsql,"insert into %s \
values (%s,%d,'%s')",tabl,kod,++nomz,strok);

    if(iceb_u_SRAV(tabl,"Kartb1",0) == 0)
      sprintf(strsql,"insert into %s \
values (%s,%d,'%s')",tabl,kod,++nomz,strok);
    
    if(sql_zap(&bd,strsql) != 0)
     {
      sprintf(strok,"dikont-%s",gettext("Ошибка записи !"));
      iceb_msql_error(&bd,strok,strsql,NULL);
     }
   }
  sprintf(strsql,"UNLOCK TABLES");
  if(sql_zap(&bd,strsql) != 0)
   {
    sprintf(strok,"dikont-%s",
    gettext("Ошибка разблокировки таблицы !"));
    iceb_msql_error(&bd,strok,strsql,NULL);
   }

  fclose(ff);
  unlink(imaf);
  return(0);
 }

//Выгрузка из базы в файл

if(mr == '-')
 {
  if((ff = fopen(imaf,"w")) == NULL)
   {
    iceb_er_op_fil(imaf,"iceb_dikont-Ошибка открытия файла",errno,NULL);
    return(1);
   }  

  if(iceb_u_SRAV(tabl,"Kontragent1",0) == 0)
    sprintf(strsql,"select zapis from %s where kodkon='%s'",
    tabl,kod);

  if(iceb_u_SRAV(tabl,"Uosin1",0) == 0)
    sprintf(strsql,"select zapis from %s where innom=%s",
    tabl,kod);

  if(iceb_u_SRAV(tabl,"Kartb1",0) == 0)
    sprintf(strsql,"select zapis from %s where tabn=%s",
    tabl,kod);

//  printw("\n%s\n",strsql);
//  refresh();
  SQLCURSOR cur;  
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);

  if(kolstr == 0)
   {
/*********************
    if(iceb_u_SRAV("Kontragent1",tabl,0) == 0 && metka[0] != '\0')
     {
//      fprintf(ff,"Идентификационный номер|\n");
      fprintf(ff,"Номер паспорта|\n");
      fprintf(ff,"Паспорт выдан|\n");
//      fprintf(ff,"Адрес|\n");
      fprintf(ff,"Код города налоговой инспекции|\n");
     }    
*******************/
    fclose(ff);
    return(1);
   }  


  while(cur.read_cursor(&row) != 0)
    fprintf(ff,"%s\n",row[0]);


  fclose(ff);
  return(0);
 }

return(1);

}
