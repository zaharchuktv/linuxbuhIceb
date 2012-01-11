/* $Id: zarp1.h,v 1.3 2011-01-13 13:49:56 sasa Exp $ */
/*04.09.2006    25.04.1996      Белых А.И. zarp1.h
Массивы для запоминания всех начислений и удержаний
на конкретного челевека за месяц
*/
#define         razm 120
struct ZAR
 {
  short	 denz[razm]; //день записи
  short  mes[razm];  /*месяц*/
  short  god[razm]; /*год*/
  short  knu[razm]; /*Коды начислений, удержаний*/
  short  prnu[razm]; /*1-начисления 2-удержания*/
  double sm[razm];   /*Сумма*/
  short	 kolbd[razm]; /*Количество больничных дней*/
  char	 sheta[razm][10]; /*Счета */
 };
