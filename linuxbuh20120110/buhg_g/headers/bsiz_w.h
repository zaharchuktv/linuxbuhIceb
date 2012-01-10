/*$Id: bsiz_w.h,v 1.2 2011-01-13 13:49:49 sasa Exp $*/
/*08.01.2008	03.10.2004	Белых А.И.	bsiz_w.h
Класс для расчёта балансовой стоимости основных средств
*/


class bsiz_per
 {
  public:

   double bs_nah_per;              //Балансовая стоимость на начало периода
   double izmen_iznosa_nah_per;    //Изменение износа на начало периода
   double amort_nah_per;           //Амортизация на начало периода
   
   double bs_kon_per;              //Балансовая стоимость на конец периода
   double izmen_iznosa_kon_per;    //Изменение износа на конец периода
   double amort_kon_per;           //Амортизация на конец периода

   double bs_prih_za_per;          //Приход за период балансовой стоимости
   double bs_rash_za_per;          //Расход за период балансовой стоимости

   double iznos_prih_za_per;        //Приход износа за период
   double iznos_rash_za_per;        //Расход износа за период

   double bs_raznica;              //Разница балансовой стоимости 
   double izmen_iznosa_raznica;    //Разница изменения износа
   double amort_raznica;           //Разница амортизации

   bsiz_per()
    {
     clear_data();
    }

   void clear_data()
    {
     bs_nah_per=izmen_iznosa_nah_per=amort_nah_per=0.;
     bs_kon_per=izmen_iznosa_kon_per=amort_kon_per=0.;
     bs_raznica=izmen_iznosa_raznica=amort_raznica=0.;
     bs_prih_za_per=bs_rash_za_per=0.;
     iznos_prih_za_per=iznos_rash_za_per=0.;
    }

  void plus(class bsiz_per *data)
   {
     bs_nah_per+=data->bs_nah_per;              
     izmen_iznosa_nah_per+=data->izmen_iznosa_nah_per;
     amort_nah_per+=data->amort_nah_per;
     
     bs_kon_per+=data->bs_kon_per;
     izmen_iznosa_kon_per+=data->izmen_iznosa_kon_per;
     amort_kon_per+=data->amort_kon_per;
     
     bs_prih_za_per+=data->bs_prih_za_per;
     bs_rash_za_per+=data->bs_rash_za_per;
     
     iznos_prih_za_per+=data->iznos_prih_za_per;
     iznos_rash_za_per+=data->iznos_rash_za_per;
     
     bs_raznica+=data->bs_raznica;
     izmen_iznosa_raznica+=data->izmen_iznosa_raznica;
     amort_raznica+=data->amort_raznica;
   }
 };

class bsiz_data
 {
  public:

 //Налоговый учёт
   class bsiz_per nu;

//Бухгалтерский учёт
   class bsiz_per bu;

   bsiz_data()
    {
     clear_data();
    }

   void clear_data()
    {
     nu.clear_data();
     bu.clear_data();     
    }

  void plus(class bsiz_data *data)
   {
    nu.plus(&data->nu);
    bu.plus(&data->bu);
   }   
 };
