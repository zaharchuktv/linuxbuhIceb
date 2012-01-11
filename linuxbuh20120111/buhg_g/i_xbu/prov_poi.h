/*$Id: prov_poi.h,v 1.9 2011-01-13 13:49:52 sasa Exp $*/
/*26.03.2008	06.01.2004	Белых А.И.	prov_poi.h
Реквизиты поиска проводок
*/

class prov_poi_data
 {
  public:
  class iceb_u_str shet;    //Счет
  class iceb_u_str shetk;   //Счет корреспондент
  class iceb_u_str kontr;   //Код контрагента
  class iceb_u_str koment;  //Коментарий
  class iceb_u_str debet;   //Сумма по дебету
  class iceb_u_str kredit;  //Сумма по кредиту
  class iceb_u_str datan;   //Дата начала поиска по дате проводки
  class iceb_u_str datak;   //Дата конца поиска  по дате проводки
  class iceb_u_str datanz;   //Дата начала поиска по дате записи
  class iceb_u_str datakz;   //Дата конца поиска  по дате записи
  class iceb_u_str metka;   //Метка проводки - из какой подсистемы сделана проводка
  class iceb_u_str nomdok;  //Номер документа
  class iceb_u_str kodop;   //Код операции
  class iceb_u_str grupak;  //Группа контрагента
  class iceb_u_str kekv;    //Код экономической классификации затрат


  class iceb_u_str zapros;
    
  
  //Конструктор
  prov_poi_data()
   {
    clear_zero();
   }

  void clear_zero()
   {
    shet.new_plus("");
    shetk.new_plus("");
    kontr.new_plus("");
    koment.new_plus("");
    debet.new_plus("");
    kredit.new_plus("");
    datan.new_plus("");
    datak.new_plus("");
    datanz.new_plus("");
    datakz.new_plus("");
    metka.new_plus("");
    nomdok.new_plus("");
    kodop.new_plus("");
    grupak.new_plus("");
    kekv.new_plus("");
   }
 };
