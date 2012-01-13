/*$Id: sprkvrt1w.h,v 1.1 2011-08-29 07:13:44 sasa Exp $*/
/*04.03.2009	31.04.2005	Белых А.И.	sprkvrt1w.h
Настройки для расчёта формы 1ДФ
*/

class sprkvrt1_nast
 {
  public:
  class iceb_u_spisok sheta;               //Настройки пар счетов для поиска нужных проводок
  class iceb_u_int kodi_pd; //коды признаков доходов
  class iceb_u_spisok kodi_nah;       // коды начислений к признакам доходов
  char gosstrah[512]; //коды фондов на 27 признак
  short		TYP; /*Метка 0-юридическая особа 1-физическая особа*/
  short    *kodnvf8dr;  /*Коды не входящие в форму 8ДР*/
  short    *kodud;  /*Коды удержаний не входящие в форму 8ДР*/
  
  class iceb_u_int kodi_pd_prov; //коды признаков доходов для которых данные берём из главной книги
  
  sprkvrt1_nast() //Конструктор
   {
    memset(gosstrah,'\0',sizeof(gosstrah));
    TYP=0;    
    kodnvf8dr=NULL;
    kodud=NULL;
   }

  ~sprkvrt1_nast() //Деструкток
   {
    if(kodnvf8dr != NULL)
      delete(kodnvf8dr);
    if(kodud != NULL)
      delete(kodud);
   }

 };

