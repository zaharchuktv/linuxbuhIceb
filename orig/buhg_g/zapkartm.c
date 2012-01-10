/*$Id: zapkartm.c,v 1.10 2011-02-21 07:35:59 sasa Exp $*/
/*04.04.2006	13.06.2004	Белых А.И.	zapkartm.c
Запись карточки материалла
*/
#include <unistd.h>
#include <time.h>
#include "buhg_g.h"

extern double okrcn;
extern SQL_baza bd;

int zapkartm(int mz, //0-запись новой карточки 1-корректировка реквизитов старой
class rek_kartmat *data,GtkWidget *wpredok)
{


short deng,mesg,godg;
short denv,mesv,godv;
iceb_u_rsdat(&deng,&mesg,&godg,data->data_kon_is.ravno(),1);
iceb_u_rsdat(&denv,&mesv,&godv,data->datv.ravno(),1);


return(zapkarw(data->skl.ravno_atoi(),data->n_kart.ravno_atoi(),
data->kodm.ravno_atoi(),data->cena.ravno_atof(),data->ei.ravno(),
data->shet.ravno(),data->krat.ravno_atof(),data->fas.ravno_atof(),data->kodtar.ravno_atoi(),data->nds.ravno_atof(),
data->mnds.ravno_atoi(),data->cenapr.ravno_atof(),
denv,mesv,godv, //Дата ввода в эксплуатацию (для малоценки)
data->innom.ravno(), //Инвентарный номер (для малоценки)
data->rnd.ravno(), //Регистрационный номер для мед. препарата
data->nomzak.ravno(), //Номер заказа
deng,mesg,godg, //Дата конечного использования
mz, //0-ввести новую 1-исправить старую
wpredok));


}
