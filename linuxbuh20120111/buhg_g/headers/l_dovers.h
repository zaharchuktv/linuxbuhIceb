/*$Id: l_dovers.h,v 1.3 2011-01-13 13:49:50 sasa Exp $*/
/*08.05.2009	21.04.2009	Белых А.И.	l_dovers.h
Реквизиты поиска в списке доверенностей
*/
class l_dovers_poi
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str nomdov;
   class iceb_u_str komu;
   class iceb_u_str post;
   int metka_poi;
   int metka_isp;
  l_dovers_poi()
   {
    clear();
    metka_poi=0;
    metka_isp=0;
   }
  void clear()
   {
    datan.new_plus("");
    datak.new_plus("");
    nomdov.new_plus("");
    komu.new_plus("");
    post.new_plus("");
   }
   
 };
