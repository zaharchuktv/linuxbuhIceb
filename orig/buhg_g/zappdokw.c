/* $Id: zappdokw.c,v 1.10 2011-02-21 07:35:59 sasa Exp $ */
/*16.05.2006	07.05.1998	Белых А.И.	zappdokw.c
Запись платежного поручения
Если вернули 0-записали
             1-нет
*/
#include        <time.h>
#include        <pwd.h>
#include        <errno.h>
#include        "buhg_g.h"
#include        "dok4w.h"
#include <unistd.h>

extern struct REC rec;
extern SQL_baza bd;

int zappdokw(char tabl[],GtkWidget *wpredok)
{
time_t		vrem;
char		plat[1024],polu[1024];
char		strsql[2048];

/*Записываем саму платежку*/

//printw("zappdok-tabl=%s %d.%d.%d npp=%s sum=%.2f smp=%.2f kop=%s rec.ddi=%d\n",
//tabl,rec.dd,rec.md,rec.gd,rec.nomdk,rec.sumd,rec.uslb,rec.kodop.ravno(),rec.ddi);

time(&vrem);

sprintf(plat,"%s#%s#%s#%s#%s#%s#%s",
rec.kodor.ravno(),rec.naior.ravno(),rec.gorod.ravno(),rec.kod.ravno(),rec.naiban.ravno(),rec.mfo.ravno(),rec.nsh.ravno());

sqlfiltr(plat,sizeof(plat));

sprintf(polu,"%s#%s#%s#%s#%s#%s#%s",
rec.kodor1.ravno(),rec.naior1.ravno(),rec.gorod1.ravno(),rec.kod1.ravno(),rec.naiban1.ravno(),rec.mfo1.ravno(),rec.nsh1.ravno());

sqlfiltr(polu,sizeof(polu));

sprintf(strsql,"insert into %s \
values ('%04d-%02d-%02d','%s','%s',%.2f,%.2f,%.2f,'%s','%s','%s',0,1,\
'%s',%d,%ld,'%04d-%d-%d','%s')",
tabl,rec.gd,rec.md,rec.dd,rec.nomdk.ravno(),rec.kodop.ravno(),rec.sumd,rec.nds,rec.uslb,rec.shbzu.ravno(),plat,polu,"",
iceb_getuid(wpredok),vrem,rec.gv,rec.mv,rec.dv,rec.kodnr.ravno());   

if(sql_zap(&bd,strsql) != 0)
 {
  if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
   return(0); //чтобы не выдавала сообщение что документ уже есть
  else
   {
    iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,wpredok);
    return(1);
   }
 }

#if 0
##################################33
short		i1;
FILE		*ff;
char		imaf[32];
/*Записываем текст комментария*/
sprintf(imaf,"pl%d.tmp",getpid());
if((ff = fopen(imaf,"r")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return(0);
 }

for(i1=0; i1 < 5 ; i1++)
 {
  memset(plat,'\0',sizeof(plat));
  if(fgets(plat,sizeof(plat),ff) == 0)
    break;

  iceb_u_ud_simv(plat,"\n");

  if(plat[0] == '#')
   continue;

  sqlfiltr(plat,sizeof(plat));
/*
  if(plat[0] == '\n' || plat[0] == '\0')
   continue;
*/
  sprintf(strsql,"insert into %sz values ('%04d-%02d-%02d','%s',0,%d,'%s')",
  tabl,rec.gd,rec.md,rec.dd,rec.nomdk.ravno(),i1,plat);   

  iceb_sql_zapis(strsql,1,0,wpredok);
 }

fclose(ff);
#########################3
#endif
sprintf(strsql,"insert into %sz values ('%04d-%02d-%02d','%s',0,%d,'%s')",
tabl,rec.gd,rec.md,rec.dd,rec.nomdk.ravno(),0,rec.naz_plat.ravno_filtr());   

printf(strsql,"%s-%s\n",__FUNCTION__,strsql);

iceb_sql_zapis(strsql,1,0,wpredok);

//Выполняем корректировки в сопутствующих таблицах
if(rec.ddi != 0)
 {
  if(iceb_u_SRAV(rec.nomdk.ravno(),rec.nomdk_i.ravno(),0) != 0 || 
  iceb_u_sravmydat(rec.dd,rec.md,rec.gd,rec.ddi,rec.mdi,rec.gdi) != 0 || \
  iceb_u_SRAV(rec.kodop.ravno(),rec.kodop_i.ravno(),0) != 0)
   {
    char tipd[16];
    memset(tipd,'\0',sizeof(tipd));
    short tp=0;
    if(iceb_u_SRAV(tabl,"Pltp",0) == 0 || iceb_u_SRAV(tabl,"Tpltp",0) == 0)
     {
      sprintf(tipd,gettext("ПЛТ"));
      tp=0;
     }
    if(iceb_u_SRAV(tabl,"Pltt",0) == 0 || iceb_u_SRAV(tabl,"Tpltt",0) == 0)
     {
      sprintf(tipd,gettext("ТРЕ"));
      tp=1;
     }
     //Исправляем подтверждающие записи если они есть
     sprintf(strsql,"update Pzpd set \
datd='%04d-%d-%d', \
nomd='%s' \
where tp=%d and nomd='%s' and \
datd='%04d-%d-%d'",rec.gd,rec.md,rec.dd,rec.nomdk.ravno(),tp,rec.nomdk_i.ravno(),rec.gdi,rec.mdi,rec.ddi);

  //  printw("%s\n",strsql);
    
    iceb_sql_zapis(strsql,1,0,wpredok);

    sprintf(strsql,"update Pzpd set \
datp='%04d-%d-%d' \
where tp=%d and nomd='%s' and datd='%04d-%d-%d' and datp='%04d-%d-%d'",
    rec.gd,rec.md,rec.dd,rec.tipz,rec.nomdk_i.ravno(),
    rec.gd,rec.md,rec.dd,rec.gdi,rec.mdi,rec.ddi);

    iceb_sql_zapis(strsql,1,0,wpredok);
                        
    //исправляем в проводках дату документа и номер документа, операцию
    sprintf(strsql,"update Prov set \
datd='%04d-%d-%d',\
nomd='%s',\
oper='%s' \
where kto='%s' and pod=%d and nomd='%s' and \
datd='%04d-%d-%d' and tz=%d",rec.gd,rec.md,rec.dd,rec.nomdk.ravno(),rec.kodop.ravno(),
    tipd,0,rec.nomdk_i.ravno(),rec.gdi,rec.mdi,rec.ddi,rec.tipz);
//По операции не проверяем так как не может быть двух платежк с одним номером

    iceb_sql_zapis(strsql,1,0,wpredok);

    //исправляем в проводках дату проводки
    //с датой документа
    sprintf(strsql,"update Prov set \
datp='%04d-%d-%d' \
where kto='%s' and pod=%d and nomd='%s' and \
datd='%04d-%d-%d' and datp='%04d-%d-%d' and tz=%d",
    rec.gd,rec.md,rec.dd,tipd,0,rec.nomdk_i.ravno(),
    rec.gd,rec.md,rec.dd,rec.gdi,rec.mdi,rec.ddi,rec.tipz);


    iceb_sql_zapis(strsql,1,0,wpredok);
   }
 
 
 }
rec.ddi=rec.dd;
rec.mdi=rec.md;
rec.gdi=rec.gd;
rec.nomdk_i.new_plus(rec.nomdk.ravno());
rec.kodop_i.new_plus(rec.kodop.ravno());


return(0);
}


