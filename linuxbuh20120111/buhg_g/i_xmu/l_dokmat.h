/*$Id: l_dokmat.h,v 1.7 2011-01-13 13:49:50 sasa Exp $*/
/*22.10.2009	25.06.2004	Белых А.И.	l_dokmat.h
Вся необходимая информация для работы с документом
*/

class dokmat_r_data
 {
  public:
  
  //Реквизиты для поиска документа
  short dd;
  short md;
  short gd;//Дата документа
  iceb_u_str nomdok; //Номер документа
  int skk;
  int tipz;     //1-приход 2-расход
  
  /****************************/
  
  iceb_u_str kpos;   //Контрагент
  iceb_u_str naimo;  //Наименование контрагента
  iceb_u_str kprr;   //Код операции
  iceb_u_str naimpr; //Наименование операции
  int metkaprov; //0-выполнять проводки для операции 1-нет
  iceb_u_str naiskl; //Наименование склада
  int skl1;     //Код встречного склада
  int lnds;     //Метка льгот по НДС
  short mvnp;  //0-внешняя 1 -внутреняя 2-изменение стоимости
  short mdd;   //0-обычный документ 1-двойной
  int pro;     //0-не выполнены проводки 1-выполнены
  int ktoi;    //Кто записал
  iceb_u_str nomnn; //Номер налоговой накладной
  iceb_u_str nomon; //Номер ответной накладной
  iceb_u_str sheta; //Счета по операции
  int blokpid; //Кто заблокировал документ
  int loginrash; //Кто распечатал чек
  short dpnn;
  short mpnn;
  short gpnn; //дата получения налоговой накладной
  double sumandspr; //Сумма НДС для приходного документа введенная вручную
  double sumkor; //Сумма корректировки на документ
  float pnds; //Процент НДС действовавший на момент создания документа.
  class iceb_u_str osnovanie;
  time_t vrem_zap;
    
  dokmat_r_data()
   {
    dd=md=gd=0;
    nomdok.new_plus("");
    skk=tipz=0;
    clear();
   }

  void clear()
   {
    osnovanie.new_plus("");
    kpos.new_plus("");
    naimo.new_plus("");
    kprr.new_plus("");
    naimpr.new_plus("");
    metkaprov=skl1=lnds=0;
    naiskl.new_plus("");
    mvnp=mdd=0;
    pro=ktoi=0;
    nomnn.new_plus("");
    nomon.new_plus("");
    sheta.new_plus("");
    blokpid=loginrash=0;
    dpnn=mpnn=gpnn=0;
    sumandspr=0.;
    sumkor=0.;
    pnds=0.;
    vrem_zap=0;
   }

 };



