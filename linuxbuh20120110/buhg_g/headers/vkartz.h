/*$Id: vkartz.h,v 1.4 2011-01-13 13:49:55 sasa Exp $*/
/*09.09.2009	23.08.2006	Белых А.И.	vkartz.h
Реквизиты карточки работника
*/
class vkartz_vs  //реквизиты карточки
 {
  public:
   //Реквизиты меню
   class iceb_u_str tabnom;
   class iceb_u_str fio;
   class iceb_u_str dolgn;
   class iceb_u_str data_pnr; //Дата приёма на работу 
   class iceb_u_str data_usr; //Дата увольнения с работы
   class iceb_u_str kod_lg;    
   class iceb_u_str zvanie;    
   class iceb_u_str shet;    
   class iceb_u_str podr;    
   class iceb_u_str kateg;    
   short blok;  //Код оператора заблокировавшего карточку или 0-незаблокирована
   short uvol;  //0-работает 1-уволен
   short sovm;                 //0-не совместитель 1-совместитель
   short mp,gp; //Дата просмотра карточки  
   short mtd;  /*0-дата не заблокирована 1-заблокирована*/
   short flagrk; //0-с каточкой нико не работает 1-работает другой оператор
   
   //Данные второй страницы 
   class iceb_u_str adres;     //Адрес
   class iceb_u_str nomer_pasp;//Номер паспорта
   class iceb_u_str kem_vidan; //Кем выдан паспорт
   class iceb_u_str data_vp;   //Дата выдачи паспорта
   class iceb_u_str telefon;   //Телефон
   class iceb_u_str har_rab;   //Характер работы
   class iceb_u_str gorod_ni;  //Город налоговой инспекции
   class iceb_u_str in;        //Индетификационный номер
   class iceb_u_str kart_shet; //Карт-счёт
   class iceb_u_str tip_kk;    //Тип кредитной карты
   class iceb_u_str data_rog;  //Дата рождения
   short pol;                  //Пол 0-мужчина 1-женщина
   class iceb_u_str data_ppz;  //Дата последнего повышения зарплаты
   int kodbank;                /*Код банка для перечисления зарплаты на карт счета*/   
   class iceb_u_str kodss;       /*Код основания учёта спецстажа*/
   vkartz_vs()
    {
      tabnom.new_plus("");
      fio.new_plus("");
      dolgn.new_plus("");
      data_pnr.new_plus("");
      data_usr.new_plus("");
      kod_lg.new_plus("");
      zvanie.new_plus("");
      shet.new_plus("");
      podr.new_plus("");
      kateg.new_plus("");
      blok=0;
      uvol=0;
      sovm=0;
     mtd=0;
      flagrk=0;
     adres.new_plus("");
     nomer_pasp.new_plus("");
     kem_vidan.new_plus("");
     data_vp.new_plus("");
     telefon.new_plus("");
     har_rab.new_plus("");
     gorod_ni.new_plus("");
     in.new_plus("");
     kart_shet.new_plus("");
     tip_kk.new_plus("");
     data_rog.new_plus("");
     pol=0;
     data_ppz.new_plus("");
     kodbank=0;
     kodss.new_plus("");
    }
 };



