/*$Id: vrshet_nomdok.c,v 1.5 2011-02-21 07:36:21 sasa Exp $*/
/*03.02.2006	12.12.2005	Белых А.И. 	vrshet_nomdok.c
Получение номера документа для выписки счетов 
*/
#include <stdlib.h>
#include <libsql.h>
#include <iceb_util.h>

void t_msql_error(SQL_baza *bd,const char *zapros);

extern SQL_baza bd;

void  vrshet_nomdok(short god,iceb_u_str *nomdok)
{
char strsql[300];
SQLCURSOR cur;
int       kolstr=0;
SQL_str   row;
int       inomdok=0;

sprintf(strsql,"select nomd from Restdok where god=%d",god);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  t_msql_error(&bd,strsql);
  return;
 }
//printf("%s // %d\n",strsql,kolstr);

while(cur.read_cursor(&row) != 0)
 {
//  printf("*vrshet_nomdok %s\n",row[0]);
  
  if(atoi(row[0]) > inomdok)
   inomdok=atoi(row[0]);
 }

inomdok++;
sprintf(strsql,"%d",inomdok);

//printf("vrshet_nomdok %s\n",strsql);

nomdok->new_plus(strsql);
 
}
