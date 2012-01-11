/*$Id: pvbank.h,v 1.6 2011-01-13 13:49:52 sasa Exp $*/
/*17.06.2006	07.04.2004	Белых А.И.	pvbank.h
Реквизиты расчета 
*/

class pvbank_rr
 {
  public:

  class iceb_u_str datan;  // дата начала
  class iceb_u_str datak;  //дата конца
  class iceb_u_str shet;   //счет
  class iceb_u_str kontr; //код контрагента
  short       metka_r;
  iceb_u_str   naimshet;
  short        vds;       //0-счет 1-субсчет
  short        stat;      //0-балансовый 1-не балансовый
  short        saldo;     //0-свернутое 3-развернутое

  pvbank_rr()
   {
    clear_data();
   }
   
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    shet.new_plus("");
    kontr.new_plus("");
    naimshet.new_plus("");
    metka_r=0;
    vds=0;
    stat=0;
    saldo=0;
   }
 };
