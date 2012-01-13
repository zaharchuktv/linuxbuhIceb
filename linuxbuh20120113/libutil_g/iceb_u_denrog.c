/*$Id: iceb_u_denrog.c,v 1.3 2011-01-13 13:50:05 sasa Exp $*/
/*18.02.2009    08.01.2000      Белых А.И.      iceb_u_denrog.c
Вычисление дня рождения
*/

void iceb_u_denrog(short *d,short *m,short *g, //Возвращаемый день родждения
int kold) //Количество дней с 1.1.1900 до дня рождения
{
int		i;
int		koldg;
int		sumd;

*d=0;
*m=1;
*g=1900;
sumd=0;

for(i=0;;i++)
 {
  koldg=365;
  if(*g % 4 == 0 && *g % 100 != 0)
   koldg=366;
  if(*g % 400 == 0)
   koldg=366;

  if(sumd+koldg >= kold)
   break;
  sumd+=koldg;
  *g=*g+1;
 }

for(i=sumd; i < kold; i++)
 {
   *d=*d+1;
   if( *m == 1 || *m == 3 || *m == 5 || *m == 7 || *m == 8 || *m == 10 || *m == 12)
    if(*d > 31)
     {
      *d=1; *m=*m+1;
     }

   if( *m == 4 || *m == 6 || *m == 9 || *m == 11)
    if(*d > 30)
     {
      *d=1; *m=*m+1;
     }

   if(*m == 2)
    {
     int metka_vis_god=0;
     if(*g % 4 == 0 && *g % 100 != 0)
      metka_vis_god=1;
     if(*g % 400 == 0)
      metka_vis_god=1;

     if(metka_vis_god == 1)
       {
        if(*d > 29)   /*Год високосный*/
         {
          *d=1; *m=*m+1;
         }
       }
      else
       {
        if(*d > 28)   /*Год не високосный*/
         {
          *d=1; *m=*m+1;
         }
       }
   }
 }


}
