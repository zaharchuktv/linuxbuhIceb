/* $Id: prilogw.c,v 1.6 2011-02-21 07:35:56 sasa Exp $ */
/*22.11.2005	07.11.2000	Белих А.И.	prilogw.c
Загрузка-выгрузка придожения к счету
Если вернули 0-порядок
         
*/
#include        <errno.h>
#include        <ctype.h>
#include        "buhg_g.h"
#include <unistd.h>

extern SQL_baza bd;

int prilogw(char mr, //+ загрузить - выгрузить
int podr, //подразделение
short god,  //год
short tipz, //1-приход 2-расход
const char *nomd,  //Номер документа
const char *imaf, //имя файла
GtkWidget *wpredok) 
{
char tabl[]={"Usldokum3"};
int		nomz;
char		strok[1024];
char		strsql[512];
SQL_str  	row;
FILE		*ff;
int		kolstr,i;

/*Загрузка из файла в базу данных*/

if(mr == '+')
 {

  if((ff = fopen(imaf,"r")) == NULL)
   {
     if(errno == ENOENT)
      return(0);
     else
       {
        iceb_er_op_fil(imaf,"",errno,wpredok);
        return(1);
       }
   }

  sprintf(strsql,"LOCK TABLES %s WRITE",tabl);

  iceb_sql_zapis(strsql,1,0,wpredok);

  sprintf(strsql,"delete from %s where podr=%d and god=%d and tp=%d \
and nomd='%s'",tabl,podr,god,tipz,nomd);

  iceb_sql_zapis(strsql,1,0,wpredok);

  nomz=0;
  while(fgets(strok,sizeof(strok),ff) != NULL)
   {
    i=strlen(strok);
    if(strok[i-1] == '\n')
      strok[i-1]='\0';
    sqlfiltr(strok,sizeof(strok));          
    sprintf(strsql,"insert into %s \
values (%d,%d,%d,'%s',%d,'%s')",tabl,podr,god,tipz,nomd,++nomz,strok);

    iceb_sql_zapis(strsql,1,0,wpredok);
    
   }
  sprintf(strsql,"UNLOCK TABLES");

  iceb_sql_zapis(strsql,1,0,wpredok);

  fclose(ff);
  unlink(imaf);
  return(0);
 }

//Выгрузка из базы в файл

if(mr == '-')
 {
  if((ff = fopen(imaf,"w")) == NULL)
   {
    iceb_er_op_fil(imaf,"",errno,wpredok);
    return(1);
   }
  
  sprintf(strsql,"select zapis from %s where podr=%d \
and god=%d and tp=%d and nomd='%s'",tabl,podr,god,tipz,nomd);
  /*
  printw("\n%s\n",strsql);
  refresh();
  */
  SQLCURSOR cur;
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    return(1);
   }

  if(kolstr == 0)
   {
    fclose(ff);
    return(1);
   }  


  while(cur.read_cursor(&row) != 0)
    fprintf(ff,"%s\n",row[0]);


  fclose(ff);
  return(0);
 }

return(0);
}
