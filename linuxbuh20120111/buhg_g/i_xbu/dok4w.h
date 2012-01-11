/*$Id: dok4w.h,v 1.5 2011-01-13 13:49:49 sasa Exp $*/
/*26.05.2010	07.05.1998	Белых А.И.	dok4.h
Набор реквизитов для получения платежного поручения
*/
class REC
{
 public:
  /*Свои реквизиты*/
  iceb_u_str kodor; /*Код организации*/
  iceb_u_str naior; /*Наименование организации*/
  iceb_u_str gorod; /*Город*/
  iceb_u_str kod;   /*Код*/
  iceb_u_str naiban;/*Наименование банка*/
  iceb_u_str mfo;   /*МФО*/
  iceb_u_str nsh;   /*Номер счета в банке*/
  iceb_u_str rsnds;  //спец счёт для НДС

  /*Чужие реквизиты*/
  iceb_u_str kodor1; /*Код организации*/
  iceb_u_str naior1; /*Наименование организации*/
  iceb_u_str gorod1; /*Город*/
  iceb_u_str kod1;   /*Код*/
  iceb_u_str naiban1;/*Наименование банка*/
  iceb_u_str mfo1;   /*МФО*/
  iceb_u_str nsh1;   /*Номер счета в банке*/
  iceb_u_str rsnds1;  //спец счёт для НДС

  double     nds;
  iceb_u_str shbzu; /*Счет банка по которому платят за услуги*/
  iceb_u_str nomdk; /*Номер документа*/
  double     sumd;      /*Сумма по документу*/
  double     uslb;      /*Сумма за услуги банка*/
  short	     dd,md,gd;   /*Дата документа*/
  iceb_u_str kodop;  /*Код операции*/
  int	     ktoz;       /*Кто записал*/
  long	     vremz;      /*Время записи*/
  short	     prov;         /*0-проводки сделаны 1-нет */
  short	     podt;         /*Метка подтверждения*/
  short	     dv,mv,gv;     //Дата валютирования
  iceb_u_str kodnr;    //Код не резидента
  class iceb_u_str naz_plat; /*Назначение платежа*/

  int tipz; //1-платёжное требование 2-платёжное поручение
  
  //Уникальные реквизиты корректируемого (исходного)документа
  iceb_u_str nomdk_i;
  short ddi,mdi,gdi;
  //неуникальный реквизит корректировка которого приводит к исправлению записей в сопутствующих таблицах
  iceb_u_str kodop_i;

  REC()
   {
    clear_data();
   }

  void clear_data()
   {
    naz_plat.new_plus("");
    kodor.new_plus("");
    naior.new_plus("");
    gorod.new_plus("");
    kod.new_plus("");
    naiban.new_plus("");
    mfo.new_plus("");
    nsh.new_plus("");
    rsnds.new_plus("");

    kodor1.new_plus("");
    naior1.new_plus("");
    gorod1.new_plus("");
    kod1.new_plus("");
    naiban1.new_plus("");
    mfo1.new_plus("");
    nsh1.new_plus("");
    rsnds1.new_plus("");
    
    nomdk_i.new_plus("");
    kodop_i.new_plus("");
    ddi=mdi=gdi=0;
  
    nds=0.;
    shbzu.new_plus("");
    nomdk.new_plus("");
    sumd=0.;
    uslb=0.;
    dd=md=gd=0;
    kodop.new_plus("");
    ktoz=0;
    vremz=0;
    prov=0;
    podt=0;
    dv=mv=gv=0;
    kodnr.new_plus("");
    tipz=0;
   }

};
