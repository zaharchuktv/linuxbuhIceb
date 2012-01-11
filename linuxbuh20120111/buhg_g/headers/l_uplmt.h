/*$Id: l_uplmt.h,v 1.2 2011-01-13 13:49:51 sasa Exp $*/
/*05.03.2008	05.03.2008	Белых А.И.	l_uplmt.h
Реквизиты для поиска записей в списке марок топлива
*/
class l_uplmt_rek
 {
  public:
   class iceb_u_str kod_top;
   class iceb_u_str kod_mat;
   
   l_uplmt_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod_top.new_plus("");
    kod_mat.new_plus("");
   }
 
 };
