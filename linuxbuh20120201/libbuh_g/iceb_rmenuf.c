/*$Id: iceb_rmenuf.c,v 1.3 2011-01-13 13:49:59 sasa Exp $*/
/*12.04.2004	12.04.2004	Белых А.И. 	iceb_u_rmenuf.c
Чтение из файла меню для выбора
*/
#include        <errno.h>
#include        "iceb_libbuh.h"


int iceb_rmenuf(char *imafil,
iceb_u_spisok *kol1,
iceb_u_spisok *kol2,
GtkWidget *wpredok)
{
FILE		*ff;
char		stroka[1000];
char		bros[512];

errno=0;
//printw("menufil1 - %s\n",imafil);
//refresh();
if((ff = fopen(imafil,"r")) == NULL)
 if(errno != ENOENT)
  {
   iceb_er_op_fil(imafil,"",errno,wpredok);
   return(-1);
  }

if(errno == ENOENT) /*Файл не существует*/
 return(-2);

while(fgets(stroka,sizeof(stroka),ff) != NULL)
 {
//  printw("%s\n",stroka);
//  refresh();
  if(stroka[0] == '#')
    continue;

  if(iceb_u_polen(stroka,bros,sizeof(bros),1,'|') != 0)
    continue;

  kol1->plus(bros);

  iceb_u_polen(stroka,bros,sizeof(bros),2,'|');

  kol2->plus(bros);
  
 }

fclose(ff);

return(0);

}
