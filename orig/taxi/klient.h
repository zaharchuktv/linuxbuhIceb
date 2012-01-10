/*$Id: klient.h,v 1.12 2011-01-13 13:50:09 sasa Exp $*/
/*26.08.2007	31.08.2004	Белых А.И.	klient.h
*/

class  klient_rek_data
 {
  public:
  iceb_u_str kod;
  iceb_u_str fio;
  iceb_u_str adres;
  iceb_u_str telef;
  iceb_u_str kodgr;    //Код группы
  iceb_u_str pr_sk;    //процент скидки
  iceb_u_str sp_podr;  //список подразделений доступ в которые запрещён
  int  metka_kvv;     //Метка контроля входа-выхода 0-включено 1-выключено
  int  pol;            //Пол 0-мужчина 1-женщина
  iceb_u_str denrog;  //Дата рождения
  class iceb_u_str koment;
  int metka_bl;       //Метка блокировки 0-активная 1-пассивная
  klient_rek_data()
   {
    clear_data();
   }

  void clear_data()
   {
    koment.new_plus("");
    kod.new_plus("");
    fio.new_plus("");
    adres.new_plus("");
    telef.new_plus("");
    kodgr.new_plus("");
    pr_sk.new_plus("");
    sp_podr.new_plus("");      
    metka_kvv=0;
    pol=0;
    metka_bl=0;
    denrog.new_plus("");
   } 
  
 };

