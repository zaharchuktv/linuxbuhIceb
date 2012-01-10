/*$Id: l_taxisk.h,v 1.2 2011-01-13 13:50:09 sasa Exp $*/
/*29.05.2009	29.05.2009	Белых А.И.	l_taxisk.h
Реквизиты для работы со списком сальдо по клиетам
*/
class l_taxisk_rek
 {
  public:
   class iceb_u_str kodkl;
   class iceb_u_str fio;
   class iceb_u_str saldo;   
   class iceb_u_str adres;
   class iceb_u_str telef;
   int pol; /*для ввода 0-мужчина 1-женщина   для поиска 0-не искать по полу 1-искать мужчин 2-искать женщин*/
   class iceb_u_str denrog;
   class iceb_u_str koment;

   short metka_poi; /*0-без поиска 1- с поиском*/   

   l_taxisk_rek()
    {
     clear_data();
     metka_poi=0;
    }
  void clear_data()
   {
    kodkl.new_plus("");
    fio.new_plus("");
    saldo.new_plus("");
    adres.new_plus("");
    telef.new_plus("");
    pol=0;
    denrog.new_plus("");
    koment.new_plus("");
   }
 
 };
