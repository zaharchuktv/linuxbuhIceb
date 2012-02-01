/*$Id: l_zarpodr.h,v 1.3 2011-01-13 13:49:51 sasa Exp $*/
/*22.06.2006	22.06.2006	Белых А.И.	l_zarpodr.h
Реквизиты для работы со списком подразделений в подсистеме "Заработная плата"
*/
class l_zarpodr_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   class iceb_u_str grupa;
   
   l_zarpodr_rek()
    {
     clear_data();
    }
   void clear_data()
    {
     kod.new_plus("");
     naim.new_plus("");
     grupa.new_plus("");
    }
 }; 

