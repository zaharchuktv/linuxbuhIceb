/*$Id: iceb_avp.h,v 1.4 2011-09-05 08:18:29 sasa Exp $*/
/*31.08.2011	14.04.2009	Белых А.И.	iceb_avp.h
Класс для автоматического выполнения проводок
*/
class iceb_avp
 {
  public:
   double suma_s_nds;
   double suma_bez_nds;
   double suma_nds;
   double suma_uheta;
   double suma_iznosa;
   
   double suma_nds_vv; /*CУмма НДС введённая в ручную*/   
   int metka_snds; /*Метка выполнения проводки на сумму НДС введённую в ручную*/

   class iceb_u_str kodop;
   class iceb_u_str shetuh;
   class iceb_u_str shetsp;   
   class iceb_u_str kontr_v_dok;
   class iceb_u_str nomer_dok;
   
   /*Дата подтвержения*/
   short dp;
   short mp;
   short gp;

   class iceb_u_spisok vib_shets; /*список настроек счетов для выбора из меню*/
   class iceb_u_spisok vib_shet;  /*выбранный счёт*/
   
  iceb_avp()
   {
    clear();
    kodop.new_plus("");
    suma_nds_vv=0.;
    metka_snds=0;
    kontr_v_dok.plus("");
    nomer_dok.plus("");
   }
  void clear()
   {
    suma_iznosa=0.;
    suma_s_nds=0.;
    suma_bez_nds=0.;
    suma_nds=0.;
    suma_uheta=0.;
    shetuh.new_plus("");
    shetsp.new_plus("");
    dp=mp=gp=0;
   }
 };
