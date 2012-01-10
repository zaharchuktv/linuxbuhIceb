/*$Id: iceb_u_propis.c,v 1.1 2011-07-06 08:12:32 sasa Exp $*/
/*05.07.2011	05.07.2011	Белых А.И.	iceb_u_propis.c
получение целой части числа прописью
Подпрограмма преобразования числа в текстовую строку
				      |       |       |
			     12 11 10 | 9 8 7 | 6 5 4 | 3  2  1
				      |       |       |
*/
#include <glib.h>
#include <ctype.h>
#include	"iceb_util.h"

const char *iceb_u_propis(double hs,int metka_1s) /*0-первая буква маленькая 1-большая*/
{
static char str[2048];

char            s[512];
char            s1[512];
int             kls;
int             i;
short           md;  /*Метка десятка*/
short           mt;  /*Метка тысячи*/
short           mm;  /*Метка миллиона*/
short           mml; /*Метка миллиарда*/
char		simv;

memset(str,'\0',sizeof(str)); 
sprintf(s,"%.2f",hs);
iceb_u_polen(s,s1,sizeof(s1),1,'.');

mml=mm=mt=md=0;

kls=strlen(s1);
for(i=kls; i> 0 ;  i--)
 {
  switch (i)
   {
    case 1 :
     if(s1[kls-i] == '0')
      {
       if(md == 1)
	{
         strcat(str,gettext(" десять"));
	}
       md=0;
       continue;
      }
     if(s1[kls-i] == '1')
      {
       if(md == 1)
	{
         strcat(str,gettext(" одиннадцать"));
	}
       else
	{
	 strcat(str,gettext(" одна"));
	}
       md=0;
       continue;
      }
     if(s1[kls-i] == '2')
      {
       if(md == 1)
	{
         strcat(str,gettext(" двенадцать"));
	}
       else
	{
         strcat(str,gettext(" две"));
	}
       md=0;
       continue;
      }
     if(s1[kls-i] == '3')
      {
       if(md == 1)
	{
         strcat(str,gettext(" тринадцать"));
	}
       else
	  strcat(str,gettext(" три"));
       md=0;
       continue;
      }
     if(s1[kls-i] == '4')
      {
       if(md == 1)
	{
         strcat(str,gettext(" четырнадцать"));
	}
       else
	{
	   strcat(str,gettext(" четыре"));
	}
       md=0;
       continue;

      }
     if(s1[kls-i] == '5')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" пятнадцать"));
	}
       else
	{
	   strcat(str,gettext(" пять"));
	}
       md=0;
       continue;
      }
     if(s1[kls-i] == '6')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" шестнадцать"));
	}
       else
	{
	   strcat(str,gettext(" шесть"));
	}
       md=0;
       continue;
      }
     if(s1[kls-i] == '7')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" семнадцать"));
	}
       else
	{
	   strcat(str,gettext(" семь"));
	}
       md=0;
       continue;
      }
     if(s1[kls-i] == '8')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" восемнадцать"));
	}
       else
	{
	   strcat(str,gettext(" восемь"));
	}
       md=0;
       continue;
      }
     if(s1[kls-i] == '9')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" девятнадцать"));
	}
       else
	{
	   strcat(str,gettext(" девять"));
	}
       md=0;
       continue;
      }

     break;
/*******************************/
    case 2 :
    case 5 :
    case 8 :
    case 11 :
     if(i == 5)
      if(s1[kls-i] == '0')
	mt++;
     if(i == 8)
      if(s1[kls-i] == '0')
	mm++;
     if(i == 11)
      if(s1[kls-i] == '0')
	mml++;

     if(s1[kls-i] == '1')
       md++;;
     if(s1[kls-i] == '2')
      {
	 strcat(str,gettext(" двадцать"));
       continue;
      }
     if(s1[kls-i] == '3')
      {
	 strcat(str,gettext(" тридцать"));
       continue;
      }
     if(s1[kls-i] == '4')
      {
       strcat(str,gettext(" сорок"));
       continue;
      }
     if(s1[kls-i] == '5')
      {
	  strcat(str,gettext(" пятьдесят"));
       continue;
      }
     if(s1[kls-i] == '6')
      {
	 strcat(str,gettext(" шестьдесят"));
       continue;
      }
     if(s1[kls-i] == '7')
      {
	 strcat(str,gettext(" семьдесят"));
       continue;
      }
     if(s1[kls-i] == '8')
      {
	 strcat(str,gettext(" восемьдесят"));
       continue;
      }
     if(s1[kls-i] == '9')
      {
	 strcat(str,gettext(" девяносто"));
       continue;
      }
     break;
/******************************/
    case 3 :
    case 6 :
    case 9 :
    case 12 :
    if(i == 6)
     if(s1[kls-i] == '0')
       mt++;
     if(i == 9)
      if(s1[kls-i] == '0')
	mm++;
     if(i == 12)
      if(s1[kls-i] == '0')
	mml++;
     /*
     if(s1[kls-i] == '0')
	mt++;
       */

     if(s1[kls-i] == '1')
       strcat(str,gettext(" сто"));
     if(s1[kls-i] == '2')
      {
	 strcat(str,gettext(" двести"));
       continue;
      }
     if(s1[kls-i] == '3')
      {
       strcat(str,gettext(" триста"));
       continue;
      }
     if(s1[kls-i] == '4')
      {
	 strcat(str,gettext(" четыреста"));
       continue;
      }
     if(s1[kls-i] == '5')
      {
	 strcat(str,gettext(" пятьсот"));
       continue;
      }
     if(s1[kls-i] == '6')
      {
	 strcat(str,gettext(" шестьсот"));
       continue;
      }
     if(s1[kls-i] == '7')
      {
	 strcat(str,gettext(" семьсот"));
       continue;
      }
     if(s1[kls-i] == '8')
      {
	 strcat(str,gettext(" восемьсот"));
       continue;
      }
     if(s1[kls-i] == '9')
      {
	 strcat(str,gettext(" девятьсот"));
       continue;
      }
     break;
/******************************/
    case 4 :
     if(s1[kls-i] == '0')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" десять тысяч"));
	}
       else
	 if(mt < 2)
	  {
	     strcat(str,gettext(" тысяч"));
	  }
       md=0;
       continue;
      }
     if(s1[kls-i] == '1')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" одиннадцать тысяч"));
	}
       else
	{
	   strcat(str,gettext(" одна тысяча"));
	}
       md=0;
       continue;
      }
     if(s1[kls-i] == '2')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" двенадцать тысяч"));
	}
       else
	{
	   strcat(str,gettext(" две тысячи"));
	}
       md=0;
       continue;
      }
     if(s1[kls-i] == '3')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" тринадцать тысяч"));
	}
       else
	{
	  strcat(str,gettext(" три тысячи"));
	}
       md=0;
       continue;
      }
     if(s1[kls-i] == '4')
      {
       if(md == 1)
	{
	  strcat(str,gettext(" четырнадцать тысяч"));
	}
       else
	{
	   strcat(str,gettext(" четыре тысячи"));
	}
       md=0;
       continue;

      }
     if(s1[kls-i] == '5')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" пятнадцать тысяч"));
	}
       else
	{
	   strcat(str,gettext(" пять тысяч"));
	}
       md=0;
       continue;
      }
     if(s1[kls-i] == '6')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" шестнадцать тысяч"));
	}
       else
	{
	   strcat(str,gettext(" шесть тысяч"));
	}
       md=0;
       continue;
      }
     if(s1[kls-i] == '7')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" семнадцать тысяч"));
	}
       else
	{
	   strcat(str,gettext(" семь тысяч"));
	}
       md=0;
       continue;
      }
     if(s1[kls-i] == '8')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" восемнадцать тысяч"));
	}
       else
	{
	   strcat(str,gettext(" восемь тысяч"));
	}
       md=0;
       continue;
      }
     if(s1[kls-i] == '9')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" девятнадцать тысяч"));
	}
       else
	{
	   strcat(str,gettext(" девять тысяч"));
	}
       md=0;
       continue;
      }
     break;

/****************************/
    case 7 :
     if(s1[kls-i] == '0')
      {
       if(md == 1)
	{
	  strcat(str,gettext(" десять миллионов"));
	}
       else
	 if(mm < 2)
	  {
	     strcat(str,gettext(" миллионов"));
	  }
       md=0;
       continue;
      }
     if(s1[kls-i] == '1')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" одиннадцать миллионов"));
	}
       else
	{
	   strcat(str,gettext(" один миллион"));
	}
       md=0;
       continue;
      }
     if(s1[kls-i] == '2')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" двенадцать миллионов"));
	}
       else
	{
	   strcat(str,gettext(" два миллиона"));
	}
       md=0;
       continue;
      }
     if(s1[kls-i] == '3')
      {
       if(md == 1)
	{
	  strcat(str,gettext(" тринадцать миллионов"));
	}
       else
	{
	  strcat(str,gettext(" три миллиона"));
	}
       md=0;
       continue;
      }
     if(s1[kls-i] == '4')
      {
       if(md == 1)
	{
	  strcat(str,gettext(" четырнадцать миллионов"));
	}
       else
	{
	   strcat(str,gettext(" четыре миллиона"));
	}
       md=0;
       continue;

      }
     if(s1[kls-i] == '5')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" пятнадцать миллионов"));
	}
       else
	{
	   strcat(str,gettext(" пять миллионов"));
	}
       md=0;
       continue;
      }
     if(s1[kls-i] == '6')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" шестнадцать миллионов"));
	}
       else
	{
	   strcat(str,gettext(" шесть миллионов"));
	}
       md=0;
       continue;
      }
     if(s1[kls-i] == '7')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" семнадцать миллионов"));
	}
       else
	{
	   strcat(str,gettext(" семь миллионов"));
	}
       md=0;
       continue;
      }
     if(s1[kls-i] == '8')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" восемнадцать миллионов"));
	}
       else
	{
	   strcat(str,gettext(" восемь миллионов"));
	}
       md=0;
       continue;
      }
     if(s1[kls-i] == '9')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" девятнадцать миллионов"));
	}
       else
	{
	   strcat(str,gettext(" девять миллионов"));
	}
       md=0;
       continue;
      }
     break;
/********************************/
    case 10 :
     if(s1[kls-i] == '0')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" десять миллиардов"));
	}
       else
	 if(mml < 2 )
	  {
	     strcat(str,gettext(" миллиард"));
	  }
       md=0;
       continue;
      }
     if(s1[kls-i] == '1')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" одиннадцать миллиардов"));
	}
       else
	{
	   strcat(str,gettext(" один миллиард"));
	}
       md=0;
       continue;
      }
     if(s1[kls-i] == '2')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" двенадцать миллиардов"));
	}
       else
	{
	   strcat(str,gettext(" два миллиарда"));
	}
       md=0;
       continue;
      }
     if(s1[kls-i] == '3')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" тринадцать миллиардов"));
	}
       else
	{
	   strcat(str,gettext(" три миллиарда"));
	}
       md=0;
       continue;
      }
     if(s1[kls-i] == '4')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" четырнадцать миллиардов"));
	}
       else
	{
	   strcat(str,gettext(" четыре миллиарда"));
	}
       md=0;
       continue;

      }
     if(s1[kls-i] == '5')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" пятнадцать миллиардов"));
	}
       else
	{
	   strcat(str,gettext(" пять миллиардов"));
	}
       md=0;
       continue;
      }
     if(s1[kls-i] == '6')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" шестнадцать миллиардов"));
	}
       else
	{
	   strcat(str,gettext(" шесть миллиардов"));
	}
       md=0;
       continue;
      }
     if(s1[kls-i] == '7')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" семнадцать миллиардов"));
	}
       else
	{
	   strcat(str,gettext(" семь миллиардов"));
	}
       md=0;
       continue;
      }
     if(s1[kls-i] == '8')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" восемнадцать миллиардов"));
	}
       else
	{
	   strcat(str,gettext(" восемь миллиардов"));
	}
       md=0;
       continue;
      }
     if(s1[kls-i] == '9')
      {
       if(md == 1)
	{
	   strcat(str,gettext(" девятнадцать миллиардов"));
	}
       else
	{
	   strcat(str,gettext(" девять миллиардов"));
	}
       md=0;
       continue;
      }
     break;
   }
 }

if(str[0] == '\0')
  sprintf(str," %s",gettext("ноль"));

if(metka_1s == 0)
 return(&str[1]); /*первый символ всегда пробел*/

/*Преобразование первой буквы из малой в большую*/
 
simv=str[1];
if(g_utf8_validate(str,-1,NULL) == FALSE)
  str[1]=toupper(simv);
else
 {
  char ddd[16];
  memset(ddd,'\0',sizeof(ddd));
  g_utf8_strncpy(ddd,str,2);

  strcpy(s1,g_utf8_strup(ddd,-1));
  gchar *strout=g_strdup_printf("%s%s",s1,iceb_u_adrsimv(2,str));
  strcpy(str,strout);
  g_free(strout);  
 }

return(&str[1]); /*первый символ всегда пробел*/
}
