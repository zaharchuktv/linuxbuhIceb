/* $Id: matstatw.c,v 1.4 2011-01-13 13:49:51 sasa Exp $ */
/*27.06.2001	17.03.1998	Белых А.И. makstatw.c
Открытие массива для графического отображение динамики реализации
товаров за любой период времени
Если вернули 0 - массив выделен
             1 - нет
*/
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include        "dvtmcf3_r.h"


int		matstatw(class dvtmcf3_r_data *data)
{
short dn,mn,gn;
short dk,mk,gk;

iceb_rsdatp(&dn,&mn,&gn,data->rk->datan.ravno(),&dk,&mk,&gk,data->rk->datak.ravno(),data->window);


/*В нулевой ячейке записан размер самого массива*/
//rmdp=(31*12)*(gk-gn+1)+1;
int rmdp=(31*12)*(gk-gn+1);
/*
printw("rmdr=%d gk=%d gn=%d\n",rmdp,gk,gn);
OSTANOV();
*/
data->mkdp.make_class(rmdp);
data->mkdr.make_class(rmdp);

return(0);

}
