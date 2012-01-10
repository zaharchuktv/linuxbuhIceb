/* $Id: avtprouslw.c,v 1.21 2011-02-21 07:35:51 sasa Exp $ */
/*10.12.2010    21.11.2000      Белых А.И.      avtprouslw.c
Автоматическое проведение проводок для учета услуг
*/
#include <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        <math.h>
#include <unistd.h>
#include        "buhg_g.h"


extern class iceb_u_int dp,mp,gp; /*Даты последнего подтверждения*/
extern class iceb_u_double snn; /*Суммы по накладных*/
extern class iceb_u_double sumpokart; /*Суммы по карточкам*/
extern class iceb_u_double sp; /*Суммы в проводках*/
extern class iceb_u_spisok   spsh; /*Список счетов*/
extern class iceb_u_double sbnds; /*Суммы без НДС*/
extern short    ggg;
extern SQL_baza bd;

void avtprouslw(int tipz, //1-приход 2-расход
short dd,short md,short gd, //Дата документа
const char kop[], //Код операции
const char nn[], //Номер документа
int podr,  //код подразделения
const char kor[], //Код контрагента
double sumkor, //Сумма корректировки суммы документа
const char nn1[],  //Номер встречного документа
float pnds, //Процент НДС на момент создания документа
FILE *ff_prot,
GtkWidget *wpredok)
{
char		bros[1024];
double          bb=0.,deb=0.,kre=0.;
char            shp1[32],shp2[32],shp21[32],shp3[32],shp4[32],shp5[32],shp6[32];
char            bros1[512];
class iceb_u_str kontr1("");
class iceb_u_str kontr2("");
class iceb_u_str kontr21("");
class iceb_u_str kontr3("");
class iceb_u_str kontr4("");
class iceb_u_str kontr5("");
class iceb_u_str kontr6("");
char		kto[32];
short           tp1,tp2,tp21,tp3,tp4,tp5;
short		mvd;
class iceb_u_str kvd("");
char		mopr;
char		kopz[32];
short		mon,mvz; /*Метка оплаты накладной / метка выдачи запроса*/
time_t		vrem;
char		korr[16];
short		mkp;
short		kp;
double		prockor=0.;
char		strsql[512];
SQL_str         row;
struct OPSHET	shetv;
int		kolpr=2;
double		sumandspr=0.;
int		metkands=0;
class iceb_u_spisok sp_prov;
class iceb_u_double sum_prov_dk;
SQLCURSOR curr;
int kekv=0;
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;


if(ff_prot != NULL)
 fprintf(ff_prot,"%s-tipz=%d %d.%d.%d kop=%s nn=%s\n",__FUNCTION__,
 tipz,dd,md,gd,kop,nn);

if(dp.suma() == 0)
 {
  sprintf(strsql,"%s\n%d.%d.%d %s",gettext("Документ не подтверждён!"),dd,md,gd,nn);
  iceb_menu_soob(strsql,wpredok);

  if(ff_prot != NULL)
   fprintf(ff_prot,"%s\n",strsql);

  return;
 }

prockor=0.;
if(sumkor != 0)
 {
  prockor=sumkor*100./snn.suma();
/*
  printw("\nПроцент корректировки %.6g Сумма=%.2f/%.2f\n",
  prockor,bb,sumkor);
  OSTANOV();
*/
 }

kp=mvz=mon=mopr=mvd=0;
memset(korr,'\0',sizeof(korr));
strcpy(korr,kor);

strcpy(kto,gettext("УСЛ"));
strcpy(kopz,kop);

/*Если возврат с реализации то ищем накладную на реализацию
определяем какая там операция и делаем проводки по этой операции
только красным*/
class iceb_u_str imaf_nast;
if(tipz == 1)
  imaf_nast.plus("avtprouslp.alx");
if(tipz == 2)
  imaf_nast.plus("avtprouslr.alx");
sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_nast.ravno());
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдены настройки %s\n",imaf_nast.ravno());
  return;
 }


int metka_vkontr=2; /*метка режима выполнения проводки для контрагента если его нет в списке счёта 0-запрос 2-автоматическая вставка*/
if(iceb_poldan("Автоматическая запись нового контрагента в список счёта",strsql,"uslnast.alx",wpredok) == 0)
 {
   if(iceb_u_SRAV("Вкл",strsql,1) == 0)
    metka_vkontr=2;
   else
    metka_vkontr=0;
 }
if(ff_prot != NULL)
 {
  if(metka_vkontr == 0)
   fprintf(ff_prot,"Ручная вставка кода контрагента в список счёта с развёрнутым сальдо если его там нет\n");
  if(metka_vkontr == 2)
   fprintf(ff_prot,"Автоматическая вставка кода контрагента в список счёта с развёрнутым сальдо если его там нет\n");
 }

if(tipz == 1)
 {
  sprintf(strsql,"select sumnds from Usldokum \
where datd='%04d-%d-%d' and podr=%d and nomd='%s' and tp=%d",
  gd,md,dd,podr,nn,tipz);
  if(sql_readkey(&bd,strsql,&row,&curr) == 1)
   {
    sumandspr=atof(row[0]);
    metkands=1;
   }
 }

mkp=0;
time(&vrem);
int nomer_sheta=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
    continue;
  if(iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|') != 0)
   continue;

  if(iceb_u_SRAV(kopz,bros,0) != 0)
    continue;
  kontr1.new_plus("");
  kontr2.new_plus("");
  kontr21.new_plus("");
  kontr3.new_plus("");
  kontr4.new_plus("");
  kontr5.new_plus("");
  kontr6.new_plus("");
  mopr=1;
  
  shp1[0]=shp2[0]=shp21[0]=shp3[0]=shp4[0]=shp5[0]=shp6[0]='\0';
  tp1=tp2=tp21=tp3=tp4=tp5=0;

  iceb_u_polen(row_alx[0],shp1,sizeof(shp1),3,'|');
  if((nomer_sheta=spsh.find(shp1)) < 0)
    continue;

  
  if(iceb_pvglkni(mp.ravno(nomer_sheta),gp.ravno(nomer_sheta),wpredok) != 0)
   continue;
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s",row_alx[0]);

  iceb_u_polen(row_alx[0],bros,sizeof(bros),4,'|');

  if(iceb_u_polen(bros,shp2,sizeof(shp2),1,',') != 0)
    strncpy(shp2,bros,sizeof(shp2));
  else
    iceb_u_polen(bros,shp21,sizeof(shp21),2,',');     

  iceb_u_polen(row_alx[0],shp3,sizeof(shp4),5,'|');
  iceb_u_polen(row_alx[0],shp4,sizeof(shp5),6,'|');
  iceb_u_polen(row_alx[0],shp5,sizeof(shp5),7,'|');
  iceb_u_polen(row_alx[0],shp6,sizeof(shp6),8,'|');

  if(mvd == 0 && shp4[0] != '\0')
   {
    mvd++;
    iceb_u_str repl;

    if(tipz == 2)
      repl.plus(gettext("Введите код валового дохода."));
    if(tipz == 1)
      repl.plus(gettext("Введите код валового расхода."));

    kvd.new_plus("");

    if(iceb_menu_vvod1(&repl,&kvd,10,wpredok) != 0)
      return;

    if(kvd.getdlinna() <= 1)
     {
      iceb_u_str nai("");

      if(iceb_l_kontrsh(1,shp4,&kvd,&nai,wpredok) != 0)
        return;
      kontr4.new_plus(kvd.ravno());
      gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
     }


    if(kvd.getdlinna() <= 1)
     {
      iceb_u_str nai("");

      if(iceb_l_kontrsh(1,shp4,&kvd,&nai,wpredok) != 0)
        return;
      kontr4.new_plus(kvd.ravno());
      gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
     }

    sprintf(strsql,"select kodkon from Skontr where ns='%s' and \
kodkon='%s'",shp4,kvd.ravno());
    if(sql_readkey(&bd,strsql,&row,&curr) != 1)
     {
      iceb_u_str repl;
      repl.plus(gettext("Не нашли код контрагента"));
      repl.plus(" ");
      repl.plus(kvd.ravno());
      repl.plus(gettext("в списке счёта"));
      repl.plus(" ");
      repl.plus(shp4);
      repl.plus(" !");
      iceb_menu_soob(&repl,wpredok);      
      return;
     }
    gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
   }
  
  if(shp1[0] == '\0')
   continue;

  kontr1.new_plus(korr);   
  if(shp1[0] != '\0')
    if(iceb_provsh(&kontr1,shp1,&shetv,metka_vkontr,0,wpredok) != 0)
     {
      return;
     }
  tp1=shetv.saldo;

  kontr2.new_plus(korr);   
  if(shp2[0] != '\0')
    if(iceb_provsh(&kontr2,shp2,&shetv,metka_vkontr,0,wpredok) != 0)
     {
      return;
     }
  tp2=shetv.saldo;

  if(shp21[0] != '\0')
   {
    if(mvz == 0)
     {

      iceb_u_str titl;
      iceb_u_str zagolovok;
      iceb_u_spisok punkt_m;

      titl.plus(gettext("Выбор"));


      zagolovok.plus(gettext("Выберите нужный счет для НДС."));


      punkt_m.plus(shp2);
      punkt_m.plus(shp21);


      int kom2=0;
      if((kom2=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,wpredok)) < 0)
        return;

      gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));

      if(kom2 == 1)
       {
        kontr21.new_plus(korr);     
        if(iceb_provsh(&kontr21,shp21,&shetv,metka_vkontr,0,wpredok) != 0)
          return;

        tp21=shetv.saldo;
        if(kontr21.getdlinna() > 1)
         strncpy(korr,kontr21.ravno(),sizeof(korr)-1);
        kontr21.new_plus(korr);        
        mon=1;
       }



     }
   if(mvz == 1 && mon == 1)
    {
     kontr21.new_plus(korr);     
     if(iceb_provsh(&kontr21,shp21,&shetv,metka_vkontr,0,wpredok) != 0)
       {
        return;
       }
     tp21=shetv.saldo;
     if(kontr21.getdlinna() > 1)
      strncpy(korr,kontr21.ravno(),sizeof(korr)-1);
     kontr21.new_plus(korr);        
    }
    mvz=1;

   }
  if(shp3[0] != '\0') /*Счёт списания*/
   {
    kontr3.new_plus(korr);     
    if(iceb_provsh(&kontr3,shp3,&shetv,metka_vkontr,0,wpredok) != 0)
     {
      return;
     }
    tp3=shetv.saldo;
    if(kontr3.getdlinna() > 1)
     strncpy(korr,kontr3.ravno(),sizeof(korr)-1);
   }

  if(shp4[0] != '\0')
   {
    kontr4.new_plus(kvd.ravno());     
    if(iceb_provsh(&kontr4,shp4,&shetv,0,0,wpredok) != 0)
     {
      return;
     }
    tp4=shetv.saldo;
    if(kontr4.getdlinna() > 1)
     kvd.new_plus(kontr4.ravno());
   }

  if(shp5[0] != '\0')
   {
    kontr5.new_plus(korr);     
    if(iceb_provsh(&kontr5,shp5,&shetv,metka_vkontr,0,wpredok) != 0)
     {
      return;
     }
    tp5=shetv.saldo;
    if(kontr5.getdlinna() > 1)
     strncpy(korr,kontr5.ravno(),sizeof(korr)-1);
   }


  bb=sumpokart.ravno(nomer_sheta);
  bb=iceb_u_okrug(bb,0.01);
  if(ff_prot != NULL)
   fprintf(ff_prot,"bb=%f\n",bb);
   
  memset(bros1,'\0',sizeof(bros1));
  if(ff_prot != NULL)
    fprintf(ff_prot,"\n1=%s 2=%s 3=%s 4=%s 5=%s 6=%s\n",
    shp1,shp2,shp3,shp4,shp5,shp6);
  /*** ПРИХОДЫ *****/
  if(tipz == 1)
   {
    if(ff_prot != NULL)
     {
      fprintf(ff_prot,"Приход услуг\n");

     }     
    
    //Если счет валового расхода отсутствует делаем проводку на сумму без НДС
    if(shp4[0] == '\0')
     {
      if(atoi(shp6) == 1) //1-й вариант расчета
       {
        //cумма без НДС
        deb=bb; kre=0.;
        memset(bros1,'\0',sizeof(bros1));
        if(nn1[0] != '\0')
          sprintf(bros1,"N%s",nn1);
        if(shp5[0] != '\0' && shp3[0] != '\0')
          iceb_zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp5,shp3,kontr5.ravno(),kontr3.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);

        if(shp5[0] != '\0' && shp1[0] != '\0')
        iceb_zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp1,shp5,kontr1.ravno(),kontr5.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);

       }
      else
        if(shp1[0] != '\0' && shp4[0] == '\0' && shp3[0] != '\0')
         { 
          deb=bb; kre=0.;
          memset(bros1,'\0',sizeof(bros1));
          if(nn1[0] != '\0')
            sprintf(bros1,"N%s",nn1);
          iceb_zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp1,shp3,kontr1.ravno(),kontr3.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
         }
     }
    /*НДС оплачено*/
    if(shp2[0] != '\0' && shp3[0] != '\0' && mon == 0 && metkands < 2)
     {
      if(sumandspr != 0.)
       {
        bb=sumandspr;
        metkands++;
       }
      else
       {
        bb=snn.ravno(nomer_sheta)+snn.ravno(nomer_sheta)*prockor/100.;
        bb=(bb-sbnds.ravno(nomer_sheta))*pnds/100.;
       }
      bb=iceb_u_okrug(bb,0.01);
      memset(bros1,'\0',sizeof(bros1));
      if(nn1[0] != '\0')
       sprintf(bros1,"N%s",nn1);

      deb=bb; kre=0.;
      if(fabs(bb) > 0.009)      
        iceb_zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp2,shp3,kontr1.ravno(),kontr3.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
     }       

    /*НДС не оплачено*/
    if(shp21[0] != '\0' && shp3[0] != '\0' && mon == 1 && metkands < 2)
     {
      if(sumandspr != 0.)
       {
        bb=sumandspr;
        metkands++;
       }
      else
       {
        bb=snn.ravno(nomer_sheta)+snn.ravno(nomer_sheta)*prockor/100.;
        bb=(bb-sbnds.ravno(nomer_sheta))*pnds/100.;
       }
      bb=iceb_u_okrug(bb,0.01);
      if(nn1[0] != '\0')
        sprintf(bros1,"N%s",nn1);
      deb=bb; kre=0.;

      if(fabs(bb) > 0.009)      
        iceb_zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp21,shp3,kontr21.ravno(),kontr3.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
     }

    if(shp4[0] != '\0') //счёт валового расхода введён
     {
//      bb=sn[nomer_sheta];
      bb=sumpokart.ravno(nomer_sheta);
      bb=iceb_u_okrug(bb,0.01);
      if(fabs(bb) > 0.009  && shp3[0] != '\0')      
       {
        deb=bb; kre=0.;
        iceb_zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp4,shp3,kontr4.ravno(),kontr3.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
       }
      if(atoi(shp6) == 0) //Нулевой вариант расчета
       {            
        if(fabs(bb) > 0.009)
         {
          strcat(bros1," ");
          strcat(bros1,shp3);
          strcat(bros1,"-код");
          strcat(bros1,korr);
          
          iceb_zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp1,shp4,kontr1.ravno(),kontr4.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
         }
       }

      if(atoi(shp6) == 1) //1-й вариант расчета
       {
        if(fabs(bb) > 0.009 )
         {
          memset(bros1,'\0',sizeof(bros1));
          strcat(bros1," ");
          strcat(bros1,shp3);
          strcat(bros1,"-код");
          strcat(bros1,korr);
          if(shp5[0] != '\0')
           {
            iceb_zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp5,shp4,kontr5.ravno(),kontr4.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
           }
          if(shp1[0] != '\0' && shp5[0] != '\0')
           {
            iceb_zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp1,shp5,kontr1.ravno(),kontr5.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
           }
         }
       }
     }
  }

  /******** РАСХОДЫ **********/
  if(tipz == 2)
   {
    if(ff_prot != NULL)
     {
      fprintf(ff_prot,"Расход услуг\n");
     }     
    
    bb=bb+bb*prockor/100.;
    if(shp5[0] != '\0' && shp1[0] != '\0')
     {
      deb=bb; kre=0.;

      /*Проводка на сумму Учета*/  
      iceb_zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp1,shp5,kontr1.ravno(),kontr5.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
     }

    /*НДС оплачено*/
    if(shp2[0] != '\0' && shp1[0] != '\0' && mon == 0)
     {
      bb=(bb-sbnds.ravno(nomer_sheta))*pnds/100.;

      bb=iceb_u_okrug(bb,0.01);
         
      if(fabs(bb) > 0.009)      
       {
        deb=bb; kre=0.;

        iceb_zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp1,shp2,kontr1.ravno(),kontr2.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
       }
     }       

    /*НДС не оплачено*/
    if(shp1[0] != '\0' && shp21[0] != '\0' && mon == 1)
     {
      bb=snn.ravno(nomer_sheta)+snn.ravno(nomer_sheta)*prockor/100.;

      bb=(bb-sbnds.ravno(nomer_sheta))*pnds/100.;
      bb=iceb_u_okrug(bb,0.01);
         
      if(fabs(bb) > 0.009)      
       {
        deb=bb; kre=0.;
        iceb_zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp1,shp21,kontr1.ravno(),kontr21.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
       }
     }       

    //Если счет валового дохода не введен

    if(shp3[0] != '\0' && shp4[0] == '\0' && shp1[0] != '\0')
     {
      bb=snn.ravno(nomer_sheta)+snn.ravno(nomer_sheta)*prockor/100.;
      bb=(bb-sbnds.ravno(nomer_sheta))+(bb-sbnds.ravno(nomer_sheta))*pnds/100.+sbnds.ravno(nomer_sheta);
      bb=iceb_u_okrug(bb,0.01);
      if(fabs(bb) > 0.009)      
       {
        deb=bb; kre=0.;
        iceb_zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp3,shp1,kontr3.ravno(),kontr1.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
       }
     }

    if(shp4[0] != '\0')
     {

      if(atoi(shp6) == 0) //Нулевой вариант расчета
       {

        bb=snn.ravno(nomer_sheta)+snn.ravno(nomer_sheta)*prockor/100.;
        bb=(bb-sbnds.ravno(nomer_sheta))+(bb-sbnds.ravno(nomer_sheta))*pnds/100.+sbnds.ravno(nomer_sheta);

        strcat(bros1," ");
        strcat(bros1,shp3);
        strcat(bros1,"-код");
        strcat(bros1,korr);

        bb=iceb_u_okrug(bb,0.01);

        if(fabs(bb) > 0.009 && shp1[0] != '\0')      
         {
          deb=bb; kre=0.;
          iceb_zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp4,shp1,kontr4.ravno(),kontr1.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
         }
        bb=snn.ravno(nomer_sheta)+snn.ravno(nomer_sheta)*prockor/100.;
//        bb=bb*pnds/100.;
        bb=(bb-sbnds.ravno(nomer_sheta))*pnds/100.;
        bb=iceb_u_okrug(bb,0.01);

        if(fabs(bb) > 0.009 && shp3[0] != '\0')      
         {
          memset(bros1,'\0',sizeof(bros1));

          strcat(bros1,shp1);
          strcat(bros1,"-ПДВ");
          deb=bb; kre=0.;
          iceb_zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp3,shp4,kontr3.ravno(),kontr4.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
         }
       }

      if(atoi(shp6) == 1) //1-й вариант расчета
       {
        bb=snn.ravno(nomer_sheta)+snn.ravno(nomer_sheta)*prockor/100.;
        bb=iceb_u_okrug(bb,0.01);


        if(fabs(bb) > 0.009  && shp1[0] != '\0')      
         {
          memset(bros1,'\0',sizeof(bros1));
          strcat(bros1,shp3);
          strcat(bros1,"-код");
          strcat(bros1,korr);
          deb=bb; kre=0.;
          iceb_zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp4,shp1,kontr4.ravno(),kontr1.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
         }
        bb=(bb-sbnds.ravno(nomer_sheta))*pnds/100.;
        bb=iceb_u_okrug(bb,0.01);
        if(fabs(bb) > 0.009  && shp1[0] != '\0' && shp3[0] != '\0')      
         {
          deb=bb; kre=0.;
          memset(bros1,'\0',sizeof(bros1));
          iceb_zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp3,shp1,kontr3.ravno(),kontr1.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
         }
       }

      bb=snn.ravno(nomer_sheta)+snn.ravno(nomer_sheta)*prockor/100.;
      bb=iceb_u_okrug(bb,0.01);


      if(fabs(bb) > 0.009  && shp3[0] != '\0')      
       {
        memset(bros1,'\0',sizeof(bros1));
        strcpy(bros1,shp1);
        deb=bb; kre=0.;
        iceb_zapmpr(gp.ravno(nomer_sheta),mp.ravno(nomer_sheta),dp.ravno(nomer_sheta),shp3,shp4,kontr3.ravno(),kontr4.ravno(),deb,kre,bros1,kolpr,kekv,&sp_prov,&sum_prov_dk,ff_prot);
       }
     }


   }
 }
/*Запись проводок из памяти в базу*/
iceb_zapmpr1(nn,kop,podr,vrem,kto,dd,md,gd,tipz,&sp_prov,&sum_prov_dk,ff_prot,wpredok);


if(mopr == 0)
 {
  sprintf(strsql,gettext("Для операции %s не найдена настройка проводок !"),kopz);
  iceb_menu_soob(strsql,wpredok);
 }   



}
