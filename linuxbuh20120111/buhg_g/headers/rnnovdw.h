/*$Id: rnnovdw.h,v 1.2 2011-01-13 13:49:52 sasa Exp $*/
/*13.12.2009	13.12.2009	Белых А.И.	rnnovdw.h
Структура для записи настроек для определения вида документа
*/
class rnnovd_rek
 {
  public:
   class iceb_u_spisok muvd; /*Список видов документов материального учёта*/
   class iceb_u_spisok uuvd; /*Список видов документов учёта услуг*/
   class iceb_u_spisok uosvd;/*Список видов документов учёта основных средств*/
   class iceb_u_spisok ukrvd;/*Список видов документов учёта командировочных расходов*/

   class iceb_u_spisok muso; /*список кодов операций*/
   class iceb_u_spisok uuso;  /*список кодов операций*/
   class iceb_u_spisok uosso; /*список кодов операций*/
   class iceb_u_spisok ukrsr;/*Список кодов расходов*/
    
 };
 