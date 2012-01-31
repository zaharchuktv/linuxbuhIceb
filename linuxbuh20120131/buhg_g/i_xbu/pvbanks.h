/*$Id: pvbanks.h,v 1.7 2011-01-13 13:49:52 sasa Exp $*/
/*06.12.2010	08.04.2004	Белых А.И.	pvbanks.h
Реквизиты расчета 
*/

class pvbanks_rr
 {
  public:
  class iceb_u_str datan;  // дата сальдо
  class iceb_u_str shet;   //счет
  class iceb_u_str kontr; //код контрагента
  class iceb_u_str grupk; /*коды групп контрагентов*/
  class iceb_u_str shet_kor; /*Счёт кореспондент для выполнения проводок*/
  class iceb_u_str datap; /*дата выполнения проводок*/
  short       metka_r;    //0-дебет 1-кредит
  iceb_u_str   naimshet;
  short        vds;       //0-счет 1-субсчет
  short        stat;      //0-балансовый 1-не балансовый
  short        saldo;     //0-свернутое 3-развернутое

  int kod_banka; /*код банка из списка банков*/
  class iceb_u_str kod00; /*Код контрагента у которого нужно брать номер расчётного счёта и МФО*/
  int kod_banka_tabl; /*код банка из таблицы Zarsb*/

  pvbanks_rr()
   {
    kod_banka_tabl=0;
    kod_banka=0;
    kod00.plus("00");
    clear_data();
   }
  void clear_data()
   {
    datan.new_plus("");
    shet.new_plus("");
    kontr.new_plus("");
    naimshet.new_plus("");
    metka_r=0;
    vds=0;
    stat=0;
    saldo=0;
    grupk.new_plus("");
    shet_kor.new_plus("");
    datap.new_plus("");
   }
 };
