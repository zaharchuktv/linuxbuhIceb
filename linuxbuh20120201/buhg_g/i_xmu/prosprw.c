/* $Id: prosprw.c,v 1.22 2011-09-05 08:18:27 sasa Exp $ */
/*06.04.2010    22.02.1994      Белых А.И.      prosprw.c
Программа просмотра проводок и определения не сделанных
для материального учета
*/
#include <stdlib.h>
#include        <math.h>
#include        "../headers/buhg_g.h"

short provpr(short d,short m,short g,const char *nn,int tp,int skk,class iceb_u_str *matshet,int kolparshet,int kolshetvpd,int skl1,GtkWidget*);

extern double   okrg1; /*Округление*/
class iceb_u_int dp,mp,gp; /*Даты последнего подтверждения*/
class iceb_u_double snn; /*Суммы по документа*/
class iceb_u_double sn; /*Суммы подтвержденные в карточках*/
double          *snpar=NULL; /*Суммы подтвержденные в парном документе*/
class iceb_u_double sp; /*Суммы в проводках*/
double          *sppar=NULL; /*Суммы в проводках для парного документа*/
class iceb_u_double sbnds; /*Суммы без НДС*/
class iceb_u_spisok    spsh; /*Список счетов*/
class iceb_u_spisok    spshpar; //Список счетов в парном документе / счетов списания 
class iceb_u_spisok    vspsh; /*Список пар счетов для документов списания / внутреннего перемещения и изменения стоимости*/
double          *vsnn1=NULL; /*Суммы по приходному документа для пар счетов*/
double          *vsnn2=NULL; /*Суммы по расходному документа для пар счетов*/
double          *vsnn3=NULL; /*Суммы выполненых проводок для пар счетов*/
double		*kofusl=NULL; //Коэффициенты распределения услуг
double		sumusl=0.; //Сумма услуг по документа
extern double	okrcn;
extern short    vtara; /*Код группы возвратная тара*/
extern char     *mtsh;  /*Перечень материальных счетов*/
extern char	*kodopsp; //Коды операций сторнирования для приходов
extern char	*kodopsr; //Коды операций сторнирования для расходов
extern SQL_baza bd;

int prosprw(short mr, //0-Проверить выполнены проводки или нет
                                 //1-составить список проводок которые нужно сделать
                                 //2-распечатку на экран
				 //3-составить список и проверить
int skl, //Склад
short d,short m,short g, //Дата
const char *nn,  //Номер документа
int tp, //1-приход 2-расход*/
int lnds, //Метка льгот по НДС
int skl1, //Склад встречного документа
const char *nomdokv, //Номер встречного документа
const char *kodop, //Код операции
class iceb_u_str *shet_suma, //Список счетов с суммами для отображения
GtkWidget *wpredok)
{
class iceb_u_str shetu("");
int nom_sh=0;
char		bros[512];
short           d1=0,m1=0,g1=0;
short           dpd=0,mpd=0,gpd=0; //Дата парного документа
short           mpi=0; /*0 - удалить отметку >0 записать отметку*/
char            sh1[32];
double          cnn=0.,cnn1=0.,vcnn1=0.;
double          bb;
short           mop=0; /*0- нет проводок >0 -есть*/
int             i,i1;
double          sumn=0.,cn=0.,vcn=0.;
long		kolstr;
SQL_str         row,row1,row2;
char		strsql[512];
int		nk,nomkarpar;
int		kgrm;
double		nds;
short		dp1,mp1,gp1;
double		kolih,cena;
double		kolihpod=0.;
short		sv;
short		voz=0;
class iceb_u_str matshet;
char		vstrshet[32]; //Встречный счет
static int	kolparshet; //Количество пар счетов
static int	kolshetvpd; //Количество счетов в парном документе
int		storno=0;
static double	sumad=0.; //Общая сумма по документу
static double suma_nacenki=0.;
double cena_real=0.;
double cena_uh=0.;
/*****************
printw("\nprospr-mr=%d skl=%d %d.%d.%d nn=%s tp=%d lnds=%d skl1=%d nomdokv=%s\n",
mr,skl,d,m,g,nn,tp,lnds,skl1,nomdokv);
OSTANOV();
**********************/
if(mr == 2) //Распечатку на экран
 {
  provpr(d,m,g,nn,tp,skl,&matshet,kolparshet,kolshetvpd,skl1,wpredok);

  shet_suma->new_plus(gettext("Подтвержденные суммы"));
  shet_suma->plus(":");
  bb=0.;
  
  for(i=0; i < spsh.kolih(); i++)
   {
    
    sprintf(strsql,"%-*s %12s",iceb_u_kolbait(8,spsh.ravno(i)),spsh.ravno(i),iceb_u_prnbr(sn.ravno(i)));
    shet_suma->ps_plus(strsql);

    bb+=sn.ravno(i);
   }
  
  if(fabs(sumusl) > 0.009)
   {
    sprintf(strsql,"%-*s %12s",iceb_u_kolbait(8,gettext("Услуги")),gettext("Услуги"),iceb_u_prnbr(sumusl));
    shet_suma->ps_plus(strsql);
    bb+=sumusl;
   }
  sprintf(strsql,"%-*s %12s",iceb_u_kolbait(8,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(bb));
  shet_suma->ps_plus(strsql);
  bb=0.;

  if(vsnn1 != NULL)
   {
    
    if(skl1 != 0)
     {
      sprintf(strsql,"%s:",gettext("Парний док."));
      shet_suma->ps_plus(strsql);
      for(i=0; i < spshpar.kolih(); i++)
       {
        sprintf(strsql,"%-10s %10.2f",spshpar.ravno(i),snpar[i]);
        shet_suma->ps_plus(strsql);
        bb+=snpar[i];
       }
     }
    else
     {
      sprintf(strsql,"%s:",gettext("Счета списания"));
      shet_suma->ps_plus(strsql);

      for(i=0; i < vspsh.kolih() ; i++)
       {
        sprintf(strsql,"%-*s %10.2f",iceb_u_kolbait(10,vspsh.ravno(i)),vspsh.ravno(i),vsnn2[i]);
        shet_suma->ps_plus(strsql);
        bb+=vsnn2[i];
       }
     }
    sprintf(strsql,"%-*s %12s",iceb_u_kolbait(8,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(bb));
    shet_suma->ps_plus(strsql);
   }
  if(suma_nacenki > 0.)
   {
//    move(yv++,xv);
    sprintf(strsql,"%-*s %12s",iceb_u_kolbait(8,gettext("Наценка")),gettext("Наценка"),iceb_u_prnbr(suma_nacenki));
    shet_suma->ps_plus(strsql);
   }   
  return(0);
 }

if(mr == 1 || mr == 3) //Составить список проводок которые надо сделать
 {
  suma_nacenki=0.;
  SQLCURSOR cur;
  SQLCURSOR cur1;
  storno=0;
  if(tp == 1)
   if(iceb_u_proverka(kodopsp,kodop,0,1) == 0)
     storno=1;
  if(tp == 2)
   if(iceb_u_proverka(kodopsr,kodop,0,1) == 0)
     storno=1;
  if(storno == 1) //Ишем дату документа по которому произошла реализация
   {
    sprintf(strsql,"select sodz from Dokummat2 where god=%d and \
nomd='%s' and sklad=%d and nomerz=14",g,nn,skl);
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) > 0)
      iceb_u_rsdat(&dpd,&mpd,&gpd,row[0],1);

   }         

  kolparshet=0;
  kolshetvpd=0;
  sumad=0.;  
  sprintf(strsql,"select nomkar,kolih,cena,nds,kodm,nomkarp,shet from \
Dokummat1 where datd='%d-%02d-%02d' and sklad=%d and nomd='%s' \
order by kodm asc",g,m,d,skl,nn);  
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
   }
/*
  printw("\nprospr-1 kolstr=%ld\n",kolstr);
  OSTANOV();  
*/
  if(kolstr == 0)
    return(0);

  spsh.free_class();
  spshpar.free_class();
  vspsh.free_class();
  sp.free_class();
  snn.free_class();
  sn.free_class();  
  sbnds.free_class();
  dp.free_class();
  mp.free_class();
  gp.free_class();
  

  while(cur.read_cursor(&row) != 0)
   {
    /*Читаем карточку материалла*/
    nk=atoi(row[0]);
    nds=atof(row[3]);
    memset(vstrshet,'\0',sizeof(vstrshet));
    strcpy(vstrshet,row[6]);
    sumad+=atof(row[1])*atof(row[2]);
    if(tp == 1 && storno == 1 && nomdokv[0] != '\0' && dpd != 0)
     {
      //Читаем в документе какая там была отпускная цена
      sprintf(strsql,"select cena from Dokummat1 where datd='%d-%d-%d'\
and nomd='%s' and kodm=%s and tipz=2",gpd,mpd,dpd,nomdokv,row[4]);
      if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) <= 0)
       {
        iceb_u_str repl;
        repl.plus(gettext("Не найдена запись !"));
        repl.ps_plus(strsql);
        iceb_menu_soob(&repl,wpredok);
        continue;
       }
      sumn=cn=atof(row1[0]);
      
     }
    else
      sumn=cn=atof(row[2]);
    cena_uh=cena_real=0.;    
    vcn=0.;
    nomkarpar=atoi(row[5]);
    //Определяем стоимость во встречном документе
    if(tp == 2 && skl1 != 0 && nomdokv[0] != '\0' && nk != 0)
     {
      memset(vstrshet,'\0',sizeof(vstrshet));
      if(atoi(row[5]) != 0)
        sprintf(strsql,"select cena from Dokummat1 where datd='%d-%d-%d' \
and sklad=%d and nomd='%s' and kodm=%s and tipz=1 and nomkar=%s",
        g,m,d,skl1,nomdokv,row[4],row[5]);
      else
        sprintf(strsql,"select cena from Dokummat1 where datd='%d-%d-%d' \
and sklad=%d and nomd='%s' and kodm=%s and tipz=1",
        g,m,d,skl1,nomdokv,row[4]);
//      printw("\n%s\n",strsql);
//      OSTANOV();
      if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) > 0)
       {
        vcn=atof(row1[0]);
        vcn=iceb_u_okrug(vcn,okrcn);
//        printw("vcn=%f\n",vcn); 
        //Определяем счет в карточке
        sprintf(strsql,"select shetu,cena,cenap from Kart where sklad=%d and \
nomk=%s",skl1,row[5]);

//        printw("\n%s\n",strsql);

        if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) > 0)
         {
          strncpy(vstrshet,row1[0],sizeof(vstrshet)-1);

          cena_uh=atof(row1[1]);

          cena_real=atof(row1[2]);
         }
//        printw("vstrshet-%s\n",vstrshet);
       }
//      OSTANOV(); 
     }

    if(nk != 0)
     {
      sprintf(strsql,"select shetu,krat,cena from Kart where sklad=%d and nomk=%d",
      skl,nk);
      if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) != 1)
       {
        sprintf(bros,"%s-Не нашли карточки %d склад %d\n",__FUNCTION__,nk,skl);
        iceb_msql_error(&bd,bros,strsql,wpredok);
       }
     }
    else
      continue;

    shetu.new_plus(row1[0]);
    if((nom_sh=spsh.find(shetu.ravno())) < 0)
     {
      spsh.plus(row1[0]);
      sp.plus(0.,-1);
      sn.plus(0.,-1); /*должны быть синхронны*/
     }

    strcpy(sh1,row1[0]);      
    cn=atof(row1[2]);

    /*Определяем группу материалла*/
    sprintf(strsql,"select kodgr from Material where kodm=%s",row[4]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) != 1)
     {
      sprintf(bros,"%s-%s %s",__FUNCTION__,gettext("Не найден код материалла"),row[4]);
      iceb_msql_error(&bd,bros,strsql,wpredok);
     }
    
    kgrm=atoi(row1[0]);


    /*Читаем количество в карточке*/
    
    kolihpod=0.;
     
    sprintf(strsql,"select datdp,kolih from Zkart where sklad=%d and \
nomk=%d and nomd='%s' and datd='%d-%02d-%02d' order by datdp asc",
    skl,nk,nn,g,m,d);
    SQLCURSOR cur1;
    if((kolstr=cur1.make_cursor(&bd,strsql)) < 0)
     {
      iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
     }
/*
  printw("\nprospr-1 kolstr=%ld\n",kolstr);
  OSTANOV();  
*/
//  if(kolstr == 0)
//    continue;

    vcnn1=cnn1=cnn=0.;
    d1=m1=g1=0;

    while(cur1.read_cursor(&row2) != 0)
     {
      kolih=atof(row2[1]);
      kolihpod+=kolih;
      bb=kolih*cn;
      bb=iceb_u_okrug(bb,okrg1);
      cnn+=bb;

      bb=atof(row2[1])*sumn;
      bb=iceb_u_okrug(bb,okrg1);
      cnn1+=bb;

      bb=atof(row2[1])*vcn;
      bb=iceb_u_okrug(bb,okrg1);
      vcnn1+=bb;
      if(cena_real > 0.)      
        suma_nacenki+=(cena_real-cena_uh)*kolih;
 
      iceb_u_rsdat(&dp1,&mp1,&gp1,row2[0],2);
      if(iceb_u_sravmydat(d1,m1,g1,dp1,mp1,gp1) < 0)
       {
        d1=dp1; m1=mp1; g1=gp1;
       }

     }

    snn.plus(cnn1,nom_sh);
    if(nds < 0. || kgrm == vtara || lnds > 0)
      sbnds.plus(cnn1,nom_sh);
    else
      sbnds.plus(0.,nom_sh);
    
    sn.plus(cnn,spsh.find(shetu.ravno()));
    if(nom_sh < 0 || (nom_sh >=0 && d1 != 0))
     {
      dp.new_plus(d1,nom_sh); 
      mp.new_plus(m1,nom_sh);
      gp.new_plus(g1,nom_sh);
     }

    /********Работа со счетами парного документа **********/

    if(vstrshet[0] != '\0')
     {
      if( skl1 != 0 && nomdokv[0] != '\0' && nk != 0 ) //Внутреннее перемещение
       {
        //Смотрим количество подтвержденное во встречном документе
        bb=readkolkw(skl1,nomkarpar,d,m,g,nomdokv,wpredok);  

        //Если в расходном документе несколько записей ссылаются на одну в приходном
        if(kolihpod < bb)
          bb=kolihpod;

        bb=bb*vcn;
       }       
      else
       bb=kolihpod*cnn1;
       
      bb=iceb_u_okrug(bb,okrg1);
      if((i1=spshpar.find(vstrshet)) >= 0)
       {
        if( skl1 != 0 && nomdokv[0] != '\0' && nk != 0 ) //Внутреннее перемещение
          snpar[i1]+=bb;
       }       
      else
       {
        if(spshpar.find_r(vstrshet) < 0)
         {
          spshpar.plus(vstrshet);
          kolshetvpd++;
         }
        if((snpar=(double *)realloc(snpar,kolshetvpd*sizeof(double))) == NULL)
         {
          printf( "Не могу добавить память для snpar !!!");
          return(0);
         }
        if((sppar=(double *)realloc(sppar,kolshetvpd*sizeof(double))) == NULL)
         {
          printf( "Не могу добавить память для sppar !!!");
          return(0);
         }
        snpar[kolshetvpd-1]=bb;
       }
      sprintf(bros,"%s,%s",sh1,vstrshet);
      if((i1=vspsh.find(bros)) >= 0)
       {
        vsnn1[i1]+=vcnn1;
        vsnn2[i1]+=cnn1;
       }
      else
       {
//        printw("Нет такой пары %s\n",bros);
//        OSTANOV();
//        kolparshet++;
        sprintf(bros,"%s,%s",sh1,vstrshet);

        if(vspsh.find_r(bros) < 0)
         {
          kolparshet++;
          vspsh.plus(bros);
         }
        if((vsnn1=(double *)realloc(vsnn1,kolparshet*sizeof(double))) == NULL)
         {
          printf("Не могу добавить память для vsnn1 !!!\n");
          return(0);
         }
        if((vsnn2=(double *)realloc(vsnn2,kolparshet*sizeof(double))) == NULL)
         {
          printf("Не могу добавить память для vsnn2 !!!\n");
          return(0);
         }
        if((vsnn3=(double *)realloc(vsnn3,kolparshet*sizeof(double))) == NULL)
         {
          printf("Не могу добавить память для vsnn3 !!!\n");
          return(0);
         }
        vsnn1[kolparshet-1]=vcnn1;
        vsnn2[kolparshet-1]=cnn1;
        vsnn3[kolparshet-1]=0.;
//        printw("%.2f %.2f %.2f %.2f\n",
//        vsnn1[kolparshet-1],vsnn2[kolparshet-1],vcnn1,cnn1);
       }
     }    
   }


  /*Смотрим сумму услуг, если они есть*/ 

  sumusl=0;

  memset(strsql,'\0',sizeof(strsql));
  sprintf(strsql,"select kolih,cena from Dokummat3 where datd='%d-%02d-%02d' and \
  sklad=%d and nomd='%s'",g,m,d,skl,nn);
  /*printw("\nstrsql=%s\n",strsql);*/
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,bros,strsql,wpredok);
   }

  while(cur.read_cursor(&row) != 0)
   {
    kolih=atof(row[0]);
    cena=atof(row[1]);
    cena=iceb_u_okrug(cena,okrcn);
    
    if(kolih > 0)
      bb=kolih*cena;
    else
      bb=cena;

    bb=iceb_u_okrug(bb,okrg1);
    sumusl+=iceb_u_okrug(bb,okrg1);

   }

  if(sumusl != 0.)
    sumusl=iceb_u_okrug(sumusl,0.01);

  kofusl=NULL;  
  if(sn.kolih() > 0)
    kofusl=new double[sn.kolih()];
/*************
  bb=0.;
  for(i=0; i < spsh.kolih() ; i++)
   {
    bb+=sn.ravno(i);
   }
***********/
  bb=sn.suma();
  for(i=0; i < sn.kolih() ; i++)
   {
    kofusl[i]=sn.ravno(i)/bb;
//    printw("kofusl[i]=%f\n",kofusl[i]);
   }
//  OSTANOV();    


  if(mr == 1)
    return(0);
 
 }
/******************************************/
  /*Ищем проводки*/
if( mr == 0 || mr == 3) //Проверить выполнены все проводки или нет
 {
  mop=mpi=0;
      
  mop=provpr(d,m,g,nn,tp,skl,&matshet,kolparshet,kolshetvpd,skl1,wpredok);
  class iceb_u_str repl;

  if( mop > 0 )
   {
    sv=0;
    if(matshet.getdlinna() > 1)
     {
      repl.plus(gettext("В документе нет материалла со счетом"));
      repl.plus(" ");
      repl.plus(matshet.ravno());
      repl.plus(" !\n");
      mpi++;
     }

    if(skl1 != 0 && sppar != NULL)
     {
      for(i=0; i < spsh.kolih() ; i++)
       {
        strncpy(sh1,spsh.ravno(i),sizeof(sh1)-1);
        if(fabs(sp.ravno(i) - sn.ravno(i)) > 0.009)
         {
          repl.plus(gettext("По счету"));
          repl.plus(" ");
          repl.plus(sh1);
          repl.plus(" ");
          repl.plus(gettext("не сделаны все проводки"));
          repl.plus(" !");
          
          sprintf(strsql," %.14g != %.14g %d/%d\n",
          sp.ravno(i),sn.ravno(i),i,spsh.kolih());
          repl.plus(strsql);
          mpi++;
          mpi++;
         }
        }
      for(i=0; i < kolshetvpd ; i++)
       {
        strncpy(sh1,spshpar.ravno(i),sizeof(sh1)-1);
        if(fabs(sppar[i] - snpar[i]) > 0.009)
         {
          repl.plus(gettext("По счету"));
          repl.plus(" ");
          repl.plus(sh1);
          repl.plus(" ");
          repl.plus(gettext("не сделаны все проводки"));
          repl.plus(" !");
          sprintf(strsql," %.14g != %.14g %d/%d\n",
          sppar[i],snpar[i],i,kolshetvpd);
          repl.plus(strsql);
          mpi++;
         }
        }
     }
    else
     for(i=0; i < spsh.kolih() ; i++)
      {
       strncpy(sh1,spsh.ravno(i),sizeof(sh1)-1);
       if(fabs(sp.ravno(i) - sn.ravno(i)) > 0.009)
        {
         repl.plus(gettext("По счету"));
         repl.plus(" ");
         repl.plus(sh1);
         repl.plus(" ");
         repl.plus(gettext("не сделаны все проводки"));
         repl.plus(" !");
         sprintf(strsql," %.14g != %.14g %d/%d\n",
         sp.ravno(i),sn.ravno(i),i,spsh.kolih());
         repl.plus(strsql);
         mpi++;
        }
      }
   
   }
  if(mpi > 0)
   {
    sprintf(strsql,"%s %s %s %d.%d.%d",
    gettext("Документ"),
    nn,
    gettext("Дата"),
    d,m,g);
    repl.plus(strsql);
    iceb_menu_soob(&repl,wpredok);
   }
 }

if(mop == 0 && sumad != 0.)
  mpi++;

sumusl=0.;

if(sppar != NULL)
 {
  free(sppar);
  sppar=NULL;
 }
if(snpar != NULL)
 {
  free(snpar);
  snpar=NULL;
 }

if(kofusl != NULL)
 {
  delete [] kofusl;
  kofusl=NULL;
 }
if(vsnn1 != NULL)
 {
  free(vsnn1);
  vsnn1=NULL;
 }
if(vsnn2 != NULL)
 {
  free(vsnn2);
  vsnn2=NULL;
 }
if(vsnn3 != NULL)
 {
  free(vsnn3);
  vsnn3=NULL;
 }

/*
printw("\n***mpi-%d mop-%d\n",mpi,mop);
OSTANOV();
  */

voz=0;
if(mpi == 0 )
 {
  /*Проводки выполнены*/
  sprintf(strsql,"update Dokummat \
set pro=1 where datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",
  g,m,d,skl,nn);
  voz=1;
 }
else
 {
  /*Проводки не выполнены*/
  voz=0;
  sprintf(strsql,"update Dokummat \
set pro=0 where datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",
  g,m,d,skl,nn);
 }

/*Если документ заблокирован то никаких меток не ставим*/
if(iceb_pblok(m,g,ICEB_PS_MU,wpredok) == 0) //Дата не заблокирована
  if(sql_zap(&bd,strsql) != 0)
   if(sql_nerror(&bd) != ER_DBACCESS_DENIED_ERROR)
    iceb_msql_error(&bd,gettext("Ошибка корректировки записи !"),strsql,wpredok);

return(voz);
}

/****************************/
/*Поиск выполненных проводок*/
/****************************/
short provpr(short d,short m,short g,
const char *nn,int tp,int skk,class iceb_u_str *matshet,int kolparshet,int kolshetvpd,int skl1,
GtkWidget *wpredok)
{
char 		sh1[32];
int             i,i1;
short           mop;
long		kolstr;
SQL_str         row;
char		strsql[512];
char		bros[512];
double		deb=0.,kre=0.;

matshet->new_plus("");

if(sp.kolih() == 0)
 return(0);
 
mop=0;

sp.clear_class();

for(i=0; i < kolshetvpd; i++)
  sppar[i]=0.;
for(i=0; i < kolparshet; i++)
  vsnn3[i]=0.;

sprintf(strsql,"select sh,shk,deb,kre from Prov \
where kto='%s' and pod=%d and nomd='%s' and datd='%d-%02d-%02d'",
gettext("МУ"),skk,nn,g,m,d);

SQLCURSOR cur;
  
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0);
 }

if(kolstr == 0)
  return(0);
int nomer_sh=0;
while(cur.read_cursor(&row) != 0)
 {
  mop++;
  deb=atof(row[2]);
  kre=atof(row[3]);
  if(skl1 == 0)
   {
    if(tp == 1 )
      strcpy(sh1,row[0]);

    if(tp == 2 )
      strcpy(sh1,row[1]);

    if((nomer_sh=spsh.find(sh1)) >= 0)
     {        
      if(iceb_u_SRAV(row[0],row[1],0) == 0 )  /*Для внутреннего перемещения*/
        sp.plus(atof(row[2]),nomer_sh);
      else         
        sp.plus(atof(row[2])+atof(row[3])*(-1),nomer_sh);
     }
    else
     if(iceb_u_pole1(mtsh,sh1,',',0,&i1) == 0 || iceb_u_SRAV(mtsh,sh1,0) == 0)
      {
       matshet->plus(sh1);
       matshet->plus(" ");
      }     
   }
  else
   {
    /*Смотрим для расходного документа*/
    if((i1=spsh.find(row[0])) >= 0) /*есть такой счет*/
      if(fabs(kre) > 0.009)
        sp.plus(kre,i1);
    /*Смотрим для приходного документа*/
    if((i1=spshpar.find(row[0])) >= 0) /*есть такой счет*/
     if(fabs(deb) > 0.009)
        sppar[i1]+=deb;
   }


  if(vsnn3 != NULL)
   {
    sprintf(bros,"%s,%s",row[0],row[1]);
    if((i1=vspsh.find(bros)) >= 0) /*есть такой счет*/
     {
      vsnn3[i1]+=atof(row[3]);
     }
   }

 }

return(mop);
}
