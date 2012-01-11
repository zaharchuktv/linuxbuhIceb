/*$Id: rozkor.h,v 1.4 2011-01-13 13:49:52 sasa Exp $*/
/*20.10.2004	20.10.2004	Белых А.И.	rozkor.h
Данные для расчёта возврата в материалном учёте
*/
class rozkor_data
 {
  public:
  static iceb_u_str nomer_ras;
  static iceb_u_str nomer_nal_nak;
  static iceb_u_str kto_v_ras;
  static iceb_u_str prihina;
  static iceb_u_str nomer_dogovora;
  static iceb_u_str data_dogovora;
  static iceb_u_str god_nal_nak;

  //Реквизиты документа 
  short dd,md,gd;
  int    tipz; //1-приход 2-расход
  iceb_u_str nomdok_c;
  int    skl;
  float pnds;

  rozkor_data()
   {
    clear_data();
   }

  void clear_data()
   {
    nomer_ras.new_plus("");
    nomer_nal_nak.new_plus("");
    kto_v_ras.new_plus("");
    prihina.new_plus("");
    nomer_dogovora.new_plus("");
    data_dogovora.new_plus("");
    god_nal_nak.new_plus("");

   }
 };
