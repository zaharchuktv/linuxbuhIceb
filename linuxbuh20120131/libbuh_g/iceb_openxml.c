/*$Id: iceb_openxml.c,v 1.1 2011-08-29 07:13:46 sasa Exp $*/
/*22.07.2011	22.07.2011	Белых А.И.	iceb_openxml.c
Чтение файла настроек и открытие xml файла для отчётности в налоговую
Если вернули 0-настройки прочитаны, файл открыт
             1- нет
*/
#include <stdlib.h>
#include <errno.h>
#include "iceb_libbuh.h"


int iceb_openxml(short dn,short mn,short gn, //Дата начала
short dk,short mk,short gk,//Дата конца
char *imaf_xml, //Имя файла
const char *tip_dok,  //Тип документа  J12
const char *pod_tip_dok, //Подтип документа
const char *nomer_versii_dok, //Номер версии документа
int nomer_dok_v_pakete, //Номер документа в пакете 
int *period_type, //Тип периода
class iceb_rnfxml_data *rek_zag_nn,
FILE **ff_xml,
GtkWidget *wpredok)
{
*period_type=1; // 1-месяц 2 квартал 3 полугодие 4-9 месяцев 5 год

*period_type=iceb_u_period(dn,mn,gn,dk,mk,gk,1);

if(*period_type == 3)
 *period_type=2;
else
 if(*period_type == 6)
  *period_type=3;
 else
  if(*period_type == 12)
   *period_type=5;


sprintf(imaf_xml,"%.4s%010d%3.3s%3.3s%2.2s100%05d%02d%04d.xml",
rek_zag_nn->kod_dpi,
atoi(rek_zag_nn->kod_edrpuo),
tip_dok,
pod_tip_dok,
nomer_versii_dok,
nomer_dok_v_pakete,
mk,gk);

if((*ff_xml = fopen(imaf_xml,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_xml,__FUNCTION__,errno,wpredok);
  return(1);
 }
return(0);
}
