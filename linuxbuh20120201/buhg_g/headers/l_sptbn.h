/*$Id: l_sptbn.h,v 1.3 2011-01-13 13:49:50 sasa Exp $*/
/*14.09.2006	14.09.2006	Белых А.И.	l_sptbn.h
Реквизиты поиска в списке табельных номеров
*/
class l_sptbn_p 
 {
  public:
   class iceb_u_str fio;
   class iceb_u_str inn;
   class iceb_u_str metka_nks; //Метка наличия карт-счёта " "-всё, "+" - у кого есть, "-" - у кого нет
   short metka_poi; //0-без поиска 1-поиск  

   l_sptbn_p()
    {
     clear_rek();
    }

   void clear_rek()
    {
     fio.new_plus("");
     inn.new_plus("");
     metka_nks.new_plus("");
     metka_poi=0;
    }
 };

