/* $Id: matsporw.c,v 1.12 2011-02-21 07:35:55 sasa Exp $ */
/*04.03.2010    03.06.1997      Белых А.И.      matsporw.c
Распечатка ранее созданных и заполенных массивов
*/
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include        "dvtmcf3_r.h"

void ppppp(class dvtmcf3_r_data *,int kka,int kon,iceb_u_double *nao,iceb_u_double *nao1,iceb_u_double *nao2,int tz,\
FILE *kaw,short tt);
void sappk(class dvtmcf3_r_data *data,int kk,int kka,char st[],long pn,long pk,short konu,iceb_u_double *nao,FILE *kaw,short tt);
void rasgraf(class dvtmcf3_r_data *data,int kka,int kon,iceb_u_double *nao,iceb_u_double *nao1,
iceb_u_double *nao2,iceb_u_int *mkk,FILE *kaw,short mm);


extern SQL_baza bd;

void matsporw(class dvtmcf3_r_data *data,
FILE *kaw,FILE *ff1)
{

/*Печатаем приходы на материалы*/
ppppp(data,data->spoprp.kolih(),data->mko.kolih(),&data->mkm1p,&data->mkm2p,&data->mkm3p,1,kaw,0);
rasgraf(data,data->spoprp.kolih(),data->mko.kolih(),&data->mkm1p,&data->mkm2p,&data->mkm3p,&data->mko,kaw,0);

//Печатаем приходы на группы материалов
ppppp(data,data->spoprp.kolih(),data->mkg.kolih(),&data->mkg1p,&data->mkg2p,&data->mkg3p,1,ff1,2);

//Распечатка в графическом виде группы материалов
rasgraf(data,data->spoprp.kolih(),data->mkg.kolih(),&data->mkg1p,&data->mkg2p,&data->mkg3p,&data->mkg,ff1,1);

//Печатаем расходы материалов
ppppp(data,data->spoprr.kolih(),data->mko.kolih(),&data->mkm1r,&data->mkm2r,&data->mkm3r,2,kaw,0);
//rasgraf(data,kolopr,kolmat,mkm1r,mkm2r,mkm3r,mko,kaw,0);
rasgraf(data,data->spoprr.kolih(),data->mko.kolih(),&data->mkm1r,&data->mkm2r,&data->mkm3r,&data->mko,kaw,0);


//Печатаем расходы на группы материалов
ppppp(data,data->spoprr.kolih(),data->mkg.kolih(),&data->mkg1r,&data->mkg2r,&data->mkg3r,2,ff1,2);
rasgraf(data,data->spoprr.kolih(),data->mkg.kolih(),&data->mkg1r,&data->mkg2r,&data->mkg3r,&data->mkg,ff1,1);



}
/***********************************/
void            ppppp(class dvtmcf3_r_data *data,
int kka,int kon,iceb_u_double *nao,iceb_u_double *nao1,
iceb_u_double *nao2,int tz, //1-приход 2-расход
FILE *kaw,short tt)
{
short           kkol=5; /*Количество колонок в распечатке*/
double          br1,br2,br3,br11,br12,sn,sn1,sn2;
long            ii,i2,i1,i,skn;
short		nums; /*Нумерация строки*/
short		kolpr; /*Количество проходов*/
short		kod;
SQL_str         row;
SQLCURSOR       cur;
char		strsql[512];
char            bros[512];
char		st[1024];

//iceb_poldan("Н.Д.С.",bros,"matnast.alx",NULL);
//float pnds=iceb_u_atof(bros);
float pnds=iceb_pnds(NULL);

/*Распечатываем*/
sn=sn1=sn2=0.;
nums=0;
for(ii=0 ; ii < kka; ii += kkol)
 {
/*  printw("ii-%d kka-%d \n",ii,kka);*/
  /*Проверяем если все равно нулю то печатаем следующюю группу столбиков*/
  i2=0; br2=0;
  for(i=ii; i<ii+kkol+i2 && i < kka;i++)
   {
    br1=0.;
    for(skn=i;skn< kka*(kon-1)+ii+kkol && skn<kka*kon; skn+=kka)
     {
      if((br1+=nao->ravno(skn)) != 0.)
        break;
     }
    if(br1 == 0.)
     i2++;
    br2+=br1;
   }
  if(br2 == 0.)
    continue;
  memset(st,'\0',sizeof(st));

  sappk(data,tz,kka,st,ii,(ii+kkol),kon,nao,kaw,tt);

  br1=0.;
  for(i=0 ; i<kon;i++)
   {
    /*Проверяем если по горизонтали сумма нулевая то строчку не печатаем*/
/*  printw("\ni-%d kka-%d\n",i,kka);*/
    br1=0.;
     for(skn=0;skn< kka ; skn++)
      {
/*     printw("skn-%d nao[%d]=%.2f\n",skn,skn+i*kka,nao[skn+i*kka]);*/
       if((br1+=nao->ravno(skn+i*kka)) != 0.)
        break;
      }
/*  printw("\n\n");
    OSTANOV();   */
    if(br1 == 0.)
      continue;
    
    if(tt == 0 || tt == 1)
     {
      kod=data->mko.ravno(i);
      sprintf(strsql,"select naimat from Material where kodm=%d",
      data->mko.ravno(i));
      if(sql_readkey(&bd,strsql,&row,&cur) != 1)
        {
         iceb_menu_soob("ppppp-Не найдено наименование материалла !\n",data->window);
        }
      strcpy(bros,row[0]);
     }
    else
     {
      kod=data->mkg.ravno(i);
      sprintf(strsql,"select naik from Grup where kod=%d",data->mkg.ravno(i));
      if(sql_readkey(&bd,strsql,&row,&cur) != 1)
       {
        iceb_menu_soob("ppppp-Не найдена группа материалла !\n",data->window);
       }
      strcpy(bros,row[0]);
     }    
    nums++;
    fprintf(kaw,"%3d|%3d|%-*.*s",nums,kod,iceb_u_kolbait(30,bros),iceb_u_kolbait(30,bros),bros);
  
    /*Предыдущая информация*/
    br1=br11=br12=0.;
    if(ii > 0 )
     {
      for(skn=i*kka; skn<i*kka+ii && skn < kka*kon ;skn++)
       {
	/*printw(" skn=%d",skn);*/
	 br1+=nao->ravno(skn);
	 br11+=nao1->ravno(skn);
	 br12+=nao2->ravno(skn);
       }
      /*printw("br1=%.2f\n",br1);
      refresh();*/
     }
    i2=0;
    kolpr=0;
    for(skn=i*kka+ii; skn<i*kka+ii+kkol+i2 && skn < i*kka+kka ;skn++)
     {

      /*Если колонка нулевая не печатать*/
      br2=0.;
      for(i1=skn-(i*kka); i1< kka*kon ; i1+=kka)
	 br2+=nao->ravno(i1);

      if(br2 != 0.)
       {
	/*printw("skn-%d (%d) br2=%.2f %d  \n",skn,i*kka+ii+kkk+i2,br2,kka*kon);*/
        kolpr++;
	fprintf(kaw,"|%12.2f|%12.2f|%12.2f",nao->ravno(skn),nao1->ravno(skn),nao2->ravno(skn));
       }
      else
       {
	i2++;
       }
      br1+=nao->ravno(skn);
      br11+=nao1->ravno(skn);
      br12+=nao2->ravno(skn);
     }
    if(kolpr > 1 || ii > 0)
      fprintf(kaw,"|%12.2f|%12.2f|%12.2f\n",br1,br11,br12);
    else
      fprintf(kaw,"\n");
    
    /*printw("%3d %-30s %.2f\n",zr2.tn,nai,br1);*/
   }
  fprintf(kaw,"%s\n",st);

  fprintf(kaw,"%-*s",iceb_u_kolbait(38,gettext(" Итого")),gettext(" Итого"));

  i2=0;
  kolpr=0;
  for(i=ii; i<ii+kkol+i2 && i < kka ;i++)
   {
    br1=br2=br3=0.;
    for(skn=i;skn < kka*(kon-1)+ii+kkol+i2 && skn< kka*kon ; skn+=kka)
     {
      br1+=nao->ravno(skn);
      br2+=nao1->ravno(skn);
      br3+=nao2->ravno(skn);
     }
    if(br1 != 0.)
     {
       kolpr++;
       fprintf(kaw,"|%12.2f|%12.2f|%12.2f",br1,br2,br3);
     }
    else
      i2++;
    sn+=br1;
    sn1+=br2;
    sn2+=br3;
   }
  if(kolpr > 1 || ii > 0)
    fprintf(kaw,"|%12.2f|%12.2f|%12.2f\n",sn,sn1,sn2);
  else
      fprintf(kaw,"\n");

  sprintf(strsql," %12.2f %12.2f %12.2f\n",sn,sn1,sn2);

  iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

  fprintf(kaw,"%s\n",st);
  fprintf(kaw,"%s: %.10g\n",gettext("Сумма с НДС"),sn2+sn2*pnds/100.);

  if(i == kka)
   break;
  ii+=i2;
 }
}

/***************/
/*Шапка таблицы*/
/***************/
void            sappk(class dvtmcf3_r_data *data,
int kk,int kka,char st[],
long pn, //Позиция начала
long pk, //Позиция конца
short konu, //Количество начислений или удержаний
iceb_u_double *nao,FILE *kaw,short tt)
{
int             i,i1,i2;
double          br1,br2;
short		kolpr;
SQL_str         row;
SQLCURSOR       cur;
char		strsql[512];

if(tt == 0)
 {
  if(kk == 1)
   {
    sprintf(strsql,"%s\n---------------------------------------\n",
    gettext("Распечатываем приходы"));

    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    fprintf(kaw,"\n%s\n",gettext("Приход"));
   }
  if(kk == 2)
   {
    sprintf(strsql,"%s\n---------------------------------------\n",
    gettext("Распечатываем расходы"));

    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);

    fprintf(kaw,"\n%s\n",gettext("Расход"));
   }
 }

if(tt == 2)
 {
  if(kk == 1)
   {
    sprintf(strsql,"%s\n---------------------------------------\n",
    gettext("Распечатываем группы материалов (приходы)"));

    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    fprintf(kaw,"\n%s\n",gettext("Группы (приходы)"));
   }
  if(kk == 2)
   {
    sprintf(strsql,"%s\n---------------------------------------\n",
    gettext("Распечатываем группы материалов (расходы)"));

    iceb_printw(iceb_u_toutf(strsql),data->buffer,data->view);
    fprintf(kaw,"\n%s\n",gettext("Группы (расходы)"));
   }
  
 }

/*Формируем строку подчеркивания*/
/*Полка над наименованием*/
memset(st,'\0',sizeof(st));
strcpy(st,"--------------------------------------");
kolpr=i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  for(i1=i; i1< kka*konu ; i1+=kka)
   {
     br2+=nao->ravno(i1);
   }
  if(br2 != 0. )
   {
    kolpr++;
    strcat(st,"-------------");
    strcat(st,"-------------");
    strcat(st,"-------------");
   }
  else
   i2++;
 }
/*Полка над ИТОГО*/
if(kolpr > 1 || i > 0)
 {
  strcat(st,"-------------");
  strcat(st,"-------------");
  strcat(st,"-------------");
 }
fprintf(kaw,"%s\n",st);
/*
if(tt == 2)
  fprintf(kaw," N |Код|%-30s","Найменування групи матеріала");
else
  fprintf(kaw," N |Код|%-30s","Найменування матеріала");
*/
if(tt == 2)
  fprintf(kaw,"%s%-*s",gettext(" N |Код|"),
  iceb_u_kolbait(30,gettext("Наименование группы материалла")),gettext("Наименование группы материалла"));
else
  fprintf(kaw,"%s%-*s",gettext(" N |Код|"),
  iceb_u_kolbait(30,gettext("Наименование материалла")),gettext("Наименование материалла"));

i2=0;

for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br2=0.;
  for(i1=i; i1< kka*konu ; i1+=kka)
   {
     br2+=nao->ravno(i1);
   }
  if(br2 == 0.)
   {
    i2++;
    continue;
   }
  if(kk == 1)
   {
//    iceb_u_pole(spoprp,bros,i+1,'#');
    sprintf(strsql,"select naik from Prihod where kod='%s'",data->spoprp.ravno(i));
   }
  if(kk == 2)
   {
//    iceb_u_pole(spoprr,bros,i+1,'#');
    sprintf(strsql,"select naik from Rashod where kod='%s'",data->spoprr.ravno(i));
//    bros);
   }
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    iceb_menu_soob("sapppk-Не найдена операция !\n",data->window);
   }

  fprintf(kaw,"| %-*.*s",iceb_u_kolbait(37,row[0]),iceb_u_kolbait(37,row[0]),row[0]);
 }

if(kolpr > 1 || i > 0) 
 {
  fprintf(kaw,"|%-*s\n",iceb_u_kolbait(38,gettext(" Итого")),gettext(" Итого"));
 }
else
 fprintf(kaw,"|\n");

fprintf(kaw,"   |   |%-30s"," ");
   
i2=0;
for(i=pn; i< pk+i2 && i < kka; i++)
 {
  /*Если колонка нулевая не печатать*/
  br1=0.;
  for(i1=i; i1< kka*konu ; i1+=kka)
   {
    if((br1+=nao->ravno(i1)) != 0.)
     break;
   }
  if(br1 != 0.)
   { 
    if(data->rk->metka_ras < 2)
      fprintf(kaw,"| %-*s| %-*s| %-*s",
      iceb_u_kolbait(11,gettext("Количетсво")),gettext("Количетсво"),
      iceb_u_kolbait(11,gettext("Килограммы")),gettext("Килограммы"),
      iceb_u_kolbait(11,gettext("Сумма (грн)")),gettext("Сумма (грн)"));

    if(data->rk->metka_ras == 2)
      fprintf(kaw,"| %-*s| %-*s| %-*s",
      iceb_u_kolbait(11,gettext("Количетсво")),gettext("Количетсво"),
      iceb_u_kolbait(11,gettext("Прибыль")),gettext("Прибыль"),
      iceb_u_kolbait(11,gettext("Сумма (грн)")),gettext("Сумма (грн)"));
   }
  else
   i2++;
 }

if(kolpr > 1 || i > 0 )
 {
  if(data->rk->metka_ras < 2 )
      fprintf(kaw,"| %-*s| %-*s| %-*s\n",
      iceb_u_kolbait(11,gettext("Количетсво")),gettext("Количетсво"),
      iceb_u_kolbait(11,gettext("Килограммы")),gettext("Килограммы"),
      iceb_u_kolbait(11,gettext("Сумма (грн)")),gettext("Сумма (грн)"));
  if(data->rk->metka_ras == 2)
      fprintf(kaw,"| %-*s| %-*s| %-*s\n",
      iceb_u_kolbait(11,gettext("Количетсво")),gettext("Количетсво"),
      iceb_u_kolbait(11,gettext("Прибыль")),gettext("Прибыль"),
      iceb_u_kolbait(11,gettext("Сумма (грн)")),gettext("Сумма (грн)"));
 }
else
 fprintf(kaw,"|\n");

fprintf(kaw,"%s\n",st);
/*
printw("Шапка закончена.\n");
refresh();
*/
}

/*************************************************/
/*Программа печати информации в графическом виде*/
/*************************************************/
void		rasgraf(class dvtmcf3_r_data *data,
int kka, //Количество по горизонтали
int kon, //Количество по вертикали
iceb_u_double *nao,iceb_u_double *nao1,
iceb_u_double *nao2,iceb_u_int *mkk,FILE *kaw,short mm) //Метка массива 0-материалы 1-группы
{
unsigned short	i,i1,i2,i3,i4,i5=0,ii;
double		sbk; /*Самая большое количество*/
double		sbs; /*Самая большая сумма*/
double          sbkg; /*Самый больщой вес*/
double		br,br1,br2;
short		kod=0;
short		nums;
short		pr;
double		it1,it2,it3;
SQL_str         row;
SQLCURSOR        cur;
char		strsql[512];
char		bros[512];


/*Определить самую большую сумму по горизонтали для 
масштабирования остальных сумм*/

pr=0;
sbkg=sbs=sbk=0.;
for(i=0 ; i < kon; i++)
 {
  br=br1=br2=0.;
  for(i1=0; i1 < kka ; i1++)
   {
    br+=nao->ravno(i1+i*kka);
    br2+=nao1->ravno(i1+i*kka);
    br1+=nao2->ravno(i1+i*kka);
   }
  if(sbk < br)
   sbk=br;
  if(sbkg < br2)
   sbkg=br2;
  if(sbs < br1)
   sbs=br1;
 }
/*
printw("Найбільша кількість %.2f Найбільша сума %.2f\n",sbk,sbs);
refresh();
*/

/*Распечатываем*/
nums=0;
for(ii=0; ii < 3; ii++)
 {
  pr=0;
  it1=it2=it3=nums=0;
  for(i=0 ; i < kon; i++)
   {
    br=br1=br2=0.;
    for(i1=0; i1 < kka ; i1++)
     {
      br+=nao->ravno(i1+i*kka);
      br1+=nao2->ravno(i1+i*kka);
      br2+=nao1->ravno(i1+i*kka);
     }
    if(br == 0.)
     continue;
    if(ii == 1 && br2 == 0.)
     continue;
  
    if(pr == 0)
      fprintf(kaw,"\
----------------------------------------------------------------------------------------------------------------\n");
    pr++;
    
    i2=(int)(br*50/sbk);
    i4=(int)(br1*50/sbs);    
    if(br2 != 0.)
      i5=(int)(br2*50/sbkg);  

    if(mm == 1)
     { 
      kod=mkk->ravno(i);
      sprintf(strsql,"select naik from Grup where kod=%d",
      mkk->ravno(i));
      if(sql_readkey(&bd,strsql,&row,&cur) != 1)
       {
        iceb_menu_soob("rasgraf-Не найдена группа материалла !\n",data->window);
       }
      strcpy(bros,row[0]);

     }

    if(mm == 0)
     {
      kod=data->mko.ravno(i);
      sprintf(strsql,"select naimat from Material where kodm=%d",
      data->mko.ravno(i));
      if(sql_readkey(&bd,strsql,&row,&cur) != 1)
        {
         iceb_menu_soob("rasgraf-Не найдено наименование материалла !\n",data->window);
        }
      strcpy(bros,row[0]);
     }      

    nums++;
    if(ii == 0)
     {
      fprintf(kaw,"%3d|%4d|%-*.*s|%*.*s|%15s|",
      nums,
      kod,
      iceb_u_kolbait(30,bros),iceb_u_kolbait(30,bros),bros,
      iceb_u_kolbait(5,gettext("Колич")),iceb_u_kolbait(5,gettext("Колич")),gettext("Колич"),
      iceb_u_prnbr(br));
      
      for(i3=0; i3 < i2 ; i3++) 
       fprintf(kaw,"*");
      
      fprintf(kaw,"\n"); 
      it1+=br;
     }
 
    if(ii == 1)
     {
      fprintf(kaw,"%3d|%4d|%-*.*s|%*.*s|%15s|",
      nums,
      kod,
      iceb_u_kolbait(30,bros),iceb_u_kolbait(30,bros),bros,
      iceb_u_kolbait(5,gettext(" Кг. ")),iceb_u_kolbait(5,gettext(" Кг. ")),gettext(" Кг. "),
      iceb_u_prnbr(br2));
      
      for(i3=0; i3 < i5 ; i3++) 
       fprintf(kaw,"*");
      fprintf(kaw,"\n"); 
      it1+=br2;
     }

    if(ii == 2)
     {
      fprintf(kaw,"%3d|%4d|%-*.*s|%*.*s|%15s|",
      nums,
      kod,
      iceb_u_kolbait(30,bros),iceb_u_kolbait(30,bros),bros,
      iceb_u_kolbait(5,gettext("Рубл.")),iceb_u_kolbait(5,gettext("Рубл.")),gettext("Рубл."),
      iceb_u_prnbr(br1));
      
      for(i3=0; i3 < i4 ; i3++) 
       fprintf(kaw,"*");
      fprintf(kaw,"\n"); 
      it1+=br1;
     }
   }
  if(pr != 0)
    fprintf(kaw,"\
-----------------------------------------------------------------------------------------------------------------\n\
%*s %15s\n",iceb_u_kolbait(45,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(it1));
 }


}
