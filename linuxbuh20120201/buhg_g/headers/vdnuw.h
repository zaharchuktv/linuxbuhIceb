/*$Id: vdnuw.h,v 1.6 2011-01-13 13:49:55 sasa Exp $*/
/*07.04.2008	22.08.2005	Белых А.И.	vdnuw.h
Реквизиты шапки документа на услуги
*/

class vdnuw_rek
 {
  public:
   //Ключевые реквизиты документа 
   short ds,ms,gs;
   iceb_u_str nomdoks;
   int podrs;
   short tipz;
   iceb_u_str kontrz; //Код контрагента первоначальный
   iceb_u_str nom_nal_nakz; //Первоначальный номер счет-фактуры
   
   //Введённые в меню реквизиты
   iceb_u_str kontr;
   iceb_u_str datad;
   iceb_u_str podrv;
   iceb_u_str nomdokv;
   iceb_u_str kodop;
   iceb_u_str usl_prod;
   iceb_u_str for_opl;
   iceb_u_str nom_nal_nak;
   iceb_u_str data_opl;
   iceb_u_str osnov;
   iceb_u_str nomdok_pos;
   class iceb_u_str data_vnp;
   iceb_u_str data_pol_nal_nak;
   iceb_u_str dover;
   iceb_u_str data_dover;
   iceb_u_str sherez;
   
   short lnds;    

   vdnuw_rek()
    {
     clear_rek();
     kontrz.plus("");
     nom_nal_nakz.plus("");
    }
   void clear_rek()
    {
     lnds=0;
     kontr.new_plus("");
     datad.new_plus("");
     podrv.new_plus("");
     nomdokv.new_plus("");
     kodop.new_plus("");
     usl_prod.new_plus("");
     for_opl.new_plus("");
     nom_nal_nak.new_plus("");
     data_opl.new_plus("");
     osnov.new_plus("");
     nomdok_pos.new_plus("");
     data_pol_nal_nak.new_plus("");
     dover.new_plus("");
     data_dover.new_plus("");
     sherez.new_plus("");
     data_vnp.new_plus("");
    }

 };


