/*$Id: zar_f1dfr.h,v 1.4 2011-01-13 13:49:55 sasa Exp $*/
/*04.03.2009	31.04.2005	Белых А.И.	zar_f1dfr.h
Настройки для расчёта формы 1ДФ
*/

class zar_f1dfr_nast
 {
  public:
  class iceb_u_spisok sheta;               //Настройки пар счетов для поиска нужных проводок
  class iceb_u_int kodi_pd; //коды признаков доходов
  class iceb_u_spisok kodi_nah;       // коды начислений к признакам доходов
  char gosstrah[100]; //коды фондов на 27 признак
  short		TYP; /*Метка 0-юридическая особа 1-физическая особа*/
  short    *kodnvf8dr;  /*Коды не входящие в форму 8ДР*/
  short    *kodud;  /*Коды удержаний не входящие в форму 8ДР*/
  
  class iceb_u_int kodi_pd_prov; //коды признаков доходов для которых данные берём из главной книги
  
  zar_f1dfr_nast() //Конструктор
   {
    memset(gosstrah,'\0',sizeof(gosstrah));
    TYP=0;    
    kodnvf8dr=NULL;
    kodud=NULL;
   }

  ~zar_f1dfr_nast() //Деструкток
   {
    if(kodnvf8dr != NULL)
      delete(kodnvf8dr);
    if(kodud != NULL)
      delete(kodud);
   }

 };

