/*$Id: l_zarnah.h,v 1.2 2011-01-13 13:49:51 sasa Exp $*/
/*22.06.2006	22.06.2006	Белых А.И.	l_zarnah.h
Реквизиты для работы со списком категорий
*/
class l_zarnah_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   class iceb_u_str shet;
   class iceb_u_str kod_tab;
   short vid_nah; //0-основная 1-дополнительная 2-прочая
   short provodki; //0-делать 1- не делать
   short fond_z;   //0-входит в фонд зарплаты 1-не входит
   
   l_zarnah_rek()
    {
     clear_data();
    }
   void clear_data()
    {
     kod.new_plus("");
     naim.new_plus("");
     shet.new_plus("");
     kod_tab.new_plus("");
     vid_nah=0;
     provodki=0;
     fond_z=0;
    }
 
 };
