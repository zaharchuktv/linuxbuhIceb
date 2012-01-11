/*$Id: zar_eks_ko.h,v 1.5 2011-01-13 13:49:55 sasa Exp $*/
/*15.09.2009	20.10.2006	Белых А.И.	zar_eks_ko.h
Реквизиты для експорта кассовых ордеров из зарплаты в "Учёт кассовых ордеров"
*/
class zar_eks_ko_rek
 {
  public:
   class iceb_u_str kassa;
   class iceb_u_str data;
   class iceb_u_str kodop;

   short metka_r;   
  
   class iceb_u_str zapros;
   short prn;    /*1-приходные кассовые ордера 2-расходные кассовые ордера*/
   class iceb_u_str shetk;  
   class iceb_u_str naimop;  

  zar_eks_ko_rek()
   {
  
    clear_data();
   }
  void clear_data()
   {
    kassa.new_plus("");
    data.new_plus("");
    kodop.new_plus("");
    metka_r=0;
   }
 
 };
