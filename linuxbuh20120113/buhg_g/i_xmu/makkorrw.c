/* $Id: makkorrw.c,v 1.5 2011-01-13 13:49:51 sasa Exp $ */
/*14.08.2004	21.01.1999	Белых А.И.	makkorrw.c
Вычисление сумы корректировки документа
Возвращаем сумму корректировки
*/
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze

extern double	okrg1;

double		makkorrw(double sum, //Сумма по документа
iceb_u_double *maskor)  //Массив с диапазонами и процентами корректировки
{
short		i;
double		sumk;
short		ns;

sumk=0.;

ns=(short)maskor->ravno(0);

if(maskor->ravno(0) == 1)
 {

  if(maskor->ravno(2) > 0.)
    sumk=sum*maskor->ravno(2)/100.;
  if(maskor->ravno(2) < 0.)
    sumk=sum*maskor->ravno(2)/(100+ maskor->ravno(2)*-1);
  sumk=iceb_u_okrug(sumk,okrg1);
/************
  printw("\n00makkorr-Вернули %.2f Процент %.2f С суммы:%.2f\n",
  sumk,maskor->ravno(2),sum);
  OSTANOV();
***********/
  return(sumk);  
 }

for(i=1; i < ns+1 ;i++)
 {
/*
  printw("ns=%d i=%d sum=%.2f maskor->ravno[i]=%.2f\n",ns,i,sum,maskor->ravno[i]);
*/
  if(i == ns)
   {
    if(maskor->ravno(i*2) > 0.)
      sumk=sum*maskor->ravno(i*2)/100.;
    if(maskor->ravno(i*2) < 0.)
      sumk=sum*maskor->ravno(i*2)/(100.+maskor->ravno(i*2)*-1);
    sumk=iceb_u_okrug(sumk,okrg1);
/*
    printw("\n1Вернули %.2f Процент %.2f\n",sumk,maskor->ravno[2*i]);
    OSTANOV();
*/  
    return(sumk);
    
   }

  if(sum >= maskor->ravno(i) && sum < maskor->ravno(i+1))
   {
    if(maskor->ravno(i+ns) > 0.)
      sumk=sum*maskor->ravno(i+ns)/100.;
    if(maskor->ravno(i+ns) < 0.)
      sumk=sum*maskor->ravno(i+ns)/(100.+maskor->ravno(i+ns)*-1);
    sumk=iceb_u_okrug(sumk,okrg1);
/*  
    printw("\n2Вернули %.2f Процент %.2f i=%d sum=%.2f ns=%d\n\
элемент массива %d\n",
    sumk,maskor->ravno(i+ns),i,sum,ns,i+ns);
    OSTANOV();
*/

    return(sumk);
   }
 }
return(sumk);
}
