/*$Id: l_uplavt.h,v 1.2 2011-01-13 13:49:51 sasa Exp $*/
/*06.03.2008	06.03.2008	Белых А.И.	l_uplavt.h
Реквизиты для поиска записей в списке автомобилей
*/
class l_uplavt_rek
 {
  public:
   class iceb_u_str kod; /*код автомобиля*/
   class iceb_u_str naim; /*наименование автомобиля*/
   class iceb_u_str gos_nomer; /*Государственный номер*/
   class iceb_u_str lnz_g; /*летняя норма затрат в городе*/
   class iceb_u_str znz_g; /*зимняя норма затрат в городе*/
   class iceb_u_str lnz_zg; /*летняя норма затрат за городом*/
   class iceb_u_str znz_zg; /*зимняя норма затрат за городом*/
   class iceb_u_str kod_vod; /*код водителя*/
   class iceb_u_str nz_gruz; /*норма затрат на перевоз груза*/
   class iceb_u_str nz_ov;   /*норма затрат на отработанное время*/
            
   l_uplavt_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod.new_plus("");
    naim.new_plus("");
    gos_nomer.new_plus("");
    lnz_g.new_plus("");
    znz_g.new_plus("");
    lnz_zg.new_plus("");
    znz_zg.new_plus("");
    kod_vod.new_plus("");
    nz_gruz.new_plus("");
    nz_ov.new_plus("");    
   }
 
 };
