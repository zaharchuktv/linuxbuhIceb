/*$Id: rnn_imp.h,v 1.2 2011-01-13 13:49:52 sasa Exp $*/
/*06.05.2008	06.05.2008	Белых А.И.	rnn_imp.h
Реквизиты иморта счет-фактур
*/

class rnn_imp_data
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   
   rnn_imp_data()
    {
     clear();
    }   

   void clear()
    {
     datan.new_plus("");
     datak.new_plus("");
    }

 };
