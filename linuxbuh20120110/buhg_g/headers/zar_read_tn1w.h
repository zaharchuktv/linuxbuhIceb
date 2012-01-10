/*$Id: zar_read_tn1w.h,v 1.1 2011-04-06 09:49:17 sasa Exp $*/

class zar_read_tn1h
 {
  public:
    double minzar; /*Минимальная зарплата*/		
    double minzar_ng; /*Минимальная зарплата на начало года*/		
    float  prog_min_ng; //Прожиточный минимум на начало года
    float  prog_min_tek; //Прожиточный минимум текущий
    float  max_sum_for_soc; //Максимальная сумма с которой начисляются соц.отчисления

  zar_read_tn1h()
   {
    clear();
   }

  void clear()
   {
    minzar=0.; /*Минимальная зарплата*/		
    minzar_ng=0.; /*Минимальная зарплата на начало года*/		
    prog_min_ng=0.; //Прожиточный минимум на начало года
    prog_min_tek=0.; //Прожиточный минимум текущий
    max_sum_for_soc=0.; //Максимальная сумма с которой начисляются соц.отчисления
   }
 };