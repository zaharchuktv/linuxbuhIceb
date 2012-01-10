/*$Id: l_doksmat.h,v 1.4 2011-01-13 13:49:50 sasa Exp $*/
/*13.10.2009	24.06.2004	Белых А.И.	l_doksmat.h
Класс с реквизитами поискп документов
*/
class doksmat_poi
 {
  public:
  iceb_u_str nomdok;
  iceb_u_str datan;
  iceb_u_str datak;
  iceb_u_str kontr;
  iceb_u_str kodop;
  iceb_u_str skl;
  int        pr; //0-все 1-только приходы 2-только расходы
  iceb_u_str dover;
  iceb_u_str nomnalnak;
  
  short metka_poi; //0-без поиска 1- с поиском
  short metka_opl; /*0-все 1-только оплаченные 2-только не оплаченные*/
  doksmat_poi()
   {
    clear();
    metka_opl=0;
   }
  
  void clear()
   {
    metka_poi=0;
  
    nomdok.new_plus("");
    datan.new_plus("");
    datak.new_plus("");
    kontr.new_plus("");
    kodop.new_plus("");
    skl.new_plus("");
    pr=0;
    dover.new_plus("");
    nomnalnak.new_plus("");
   
   }
 };
