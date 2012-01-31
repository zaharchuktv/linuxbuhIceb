/*$Id: xrnn_poiw.h,v 1.5 2011-02-21 07:35:59 sasa Exp $*/
/*03.12.2009	01.05.2008	Белых А.И.	xrnn_poiw.h
Реквизиты поиска нужных записей в реестрах выданных и полученных счет-фактур
*/


class xrnn_poiw
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   
   class iceb_u_str nnn;  //Номер счет-фактуры
   class iceb_u_str nomdok; //Номер документа первоисточника
   class iceb_u_str inn;
   class iceb_u_str naim_kontr;
   class iceb_u_str koment;
   class iceb_u_str kodgr;  // для распечатки полученных документов
   class iceb_u_str kodgr1; //для поиска выданных документов
   class iceb_u_str podsistema;
   class iceb_u_str kodop; //Код операции в документе
   class iceb_u_str datavnn; /*Дата выписки счет-фактуры - начало периода*/
   class iceb_u_str datavnk; /*Дата выписки счет-фактуры - конец периода*/
   class iceb_u_str viddok; /*Вид документа*/
   short metka_poi; //0-нет поиска 1-есть   
   class iceb_u_str proc_dot; /*Процент дотации*/
   short metka_sort; /*0 сортировка по номерам накладных 1-по номерам дневных номеров записей*/
   
   class iceb_u_str nom_p_zp; //Номер первой записи для полученных документов (для распечатки)
   class iceb_u_str nom_p_zm; //Номер первой записи для выданных документов (для распечатки)
   int pr_ras; /*0-все 1-полученные 2-выданные*/
     
   double suma[2];

   class iceb_u_spisok imaf;
   class iceb_u_spisok naim;
   
  xrnn_poiw()
   {
    clear_data();    
    metka_sort=0;
   }

 void clear_data()
  {
   datan.new_plus("");
   datak.new_plus("");
   nnn.new_plus("");
   nomdok.new_plus("");
   inn.new_plus("");
   naim_kontr.new_plus("");
   koment.new_plus("");
   kodgr.new_plus("");
   kodgr1.new_plus("");
   podsistema.new_plus("");
   kodop.new_plus("");
   metka_poi=0;
   nom_p_zp.new_plus("");
   nom_p_zm.new_plus("");
   pr_ras=0;
   datavnn.new_plus("");   
   datavnk.new_plus("");   
   proc_dot.new_plus("");
   viddok.new_plus("");
  }
 };







