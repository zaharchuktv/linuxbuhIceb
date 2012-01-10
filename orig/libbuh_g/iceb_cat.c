/*$Id: iceb_cat.c,v 1.2 2011-01-13 13:49:58 sasa Exp $*/
/*24.09.2009	23.09.2009	Белых А.И.	iceb_cat.c
Объединение двух файлов в один
*/
#include <errno.h>
#include "iceb_libbuh.h"

int iceb_cat(const char *imaf1,const char *imaf2,GtkWidget *wpredok)
{
FILE *ff_1;
FILE *ff_2;
if((ff_1 = fopen(imaf1,"a")) == NULL)
 {
  iceb_er_op_fil(imaf1,"iceb_cat",errno,wpredok);
  return(1);
 }

if((ff_2 = fopen(imaf2,"r")) == NULL)
 {
  iceb_er_op_fil(imaf2,"iceb_cat",errno,wpredok);
  return(1);
 }

int i=0;
while((i=fgetc(ff_2)) != EOF)
  fputc(i, ff_1);  

fclose(ff_2);

fclose(ff_1);
return(0);

}
