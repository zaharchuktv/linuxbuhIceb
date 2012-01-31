/* $Id: sqlfiltr.c,v 1.5 2011-01-13 13:50:03 sasa Exp $ */
/*10.11.2003	25.03.1999	Белих А.И.	sqlfiltr.c
Так как одинарная кавычка в sql запросе является знаком пунктуации,
то при использовании ее в тексте, который потом нужно пометстить
в sql запрос перед ней нужно поставить еще одну кавычку
*/
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

int sqlfiltr(char *mas,int siz)
{
char		*mas1;
int		i,i1;
int		dkl;

//printw("\nmas=%s\n",mas);

if(mas[0] == '\0')
  return(0);
dkl=strlen(mas);
if(dkl == 0)
 return(0);
 
/*Определяем количество*/
i1=0;
for(i=0; i<dkl ; i++)
 if(mas[i] == '\'')
   i1++;

if(i1 == 0)
 return(0);

if((mas1=new char[(dkl+i1+1)*sizeof(char)]) == NULL)
 {
  printf("\nsqlfiltr-Не могу выделить пямять для буффера !\n");
  exit(1);
 }
i1=0;
for(i=0; i<dkl ; i++)
 {
  switch (mas[i])
   {
    case '\'':
     mas1[i1]='\'';
     i1++;
     mas1[i1]=mas[i];
     break;

    default:
     mas1[i1]=mas[i];
     break;    
   }
  
  i1++;
 }

mas1[i1]='\0';

memset(mas,'\0',siz);
sprintf(mas,"%.*s",siz-1,mas1);
delete [] mas1;
return(0);
}
