/* $Id: procentw.c,v 1.17 2011-04-14 16:09:36 sasa Exp $ */
/*22.12.2010    10.05.1995      Белых А.И.      procentw.c
Перерасчет начисления по заданному табельному номеру и проценту
Если введен тариф то считается от тарифа и отработанных дней
Если тариф не введен то считается от суммы введенной в
указанное начисление
*/
#include <ctype.h>
#include <stdlib.h>
#include        <errno.h>
#include <unistd.h>
#include        "buhg_g.h"

extern double   okrg;
extern float    dnei; /*Количество рабочих дней в текущем месяце*/
extern float    hasov; /*Количество рабочих часов в текущем месяце*/
extern short    *kodotrs;  /*Коды отдельного рачета доплаты до минимальной зарплаты*/
extern short	koddopmin; /*Код доплаты до минимальной зарплаты*/
//extern double	minzar; /*Минимальная зарплата*/		
extern char	*shetb; /*Бюджетные счета начислений*/
extern SQL_baza bd;

void procentw(int tn,short mp,short gp,int podr,const char *nah_only,FILE *ff_prot,GtkWidget *wpredok)
{
double		koefpv; /*Коэффициент полноты выплаты*/
double		doplata;
short		nomz;
char            str[1024];
char            br[1024];
char            kn1[1024]; /*Перечень входных кодов*/
short           kn2=0;     /*Выходной код*/
short		kodud;   //Код удержания
double          proc;
class iceb_u_str tarif_str("");
int            tn1;
double          nah=0.;
double		nahb=0.;
double          tarif=0.;
double          sum,sumb;
int             i,i1;
short           d;
int             mhs=0; /*Метка часовой ставки*/
short		ktab=0;
char		shet[32];
SQL_str         row,row_alx;
char		strsql[512];
long		kolstr;
float		has=0.;
short		gg,mm;
short		nomzap;
float		odnei=0.;
char		den[32];
char		uprp[16]; /*Метка учета выполенения плана подразделением*/
char		kodpodr[16]; /*Код подразделения план которого надо учесть*/
double		procentp=0.; /*Процент выполнения плана подразделением*/
double		procentpb=0.; /*Процент выполнения плана подразделением бюджет*/
float		dneir;    /*Колічество дней взятых для расчета*/
float		hasovr;   /*Количество часов взятых для расчета*/
int		iskkod;   /*Исключающий код*/
char		mesqc[32];
char		shetbd[32]; //Бюджетный счет
char		shetnbd[32]; //Не бюджетный счет
char		shetuder[32]; //Счет удержания
class ZARP     zp;
double		bb;
SQLCURSOR curr,cur_alx;
/*
printw("proc-%d %d.%d %d\n",tn,mp,gp,podr);
refresh();
*/
memset(shetbd,'\0',sizeof(shetbd));
memset(shetnbd,'\0',sizeof(shetnbd));
koefpv=proc=nah=tarif=sum=sumb=0.;
dneir=dnei;

sprintf(strsql,"select str from Alx where fil='zarproc.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдены настройки zarproc.alx\n");
  return;
 }

class SQLCURSOR cur;
 
iceb_u_dpm(&d,&mp,&gp,5);
zp.tabnom=tn;
zp.prn=1;
zp.knu=kn2;
zp.dz=d;
zp.mz=mp;
zp.gz=gp;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;
short metka_ras=0; //если равна 1-делать расчёт

while(cur_alx.read_cursor(&row_alx) != 0)
 {
//   printw("%s",row_alx[0]);
   if(row_alx[0][0] == '#')
     continue;
     
   hasovr=hasov;  //Иногда обнуляется
   has=odnei=0.;
   memset(br,'\0',sizeof(br));

   iceb_u_polen(row_alx[0],br,sizeof(br),1,'|');

   tn1=(short)iceb_u_atof(br);
   metka_ras=0;

   if(iceb_u_strstrm(br,"всем") == 1)
    metka_ras=1;

   if(iceb_u_SRAV("под-ние",br,1) == 0)
    {
     //извлекаем номер подразделения
     iceb_u_polen(br,str,sizeof(str),2,' ');
     int podr_v_str=atoi(str);
     if(podr_v_str != 0)
      {
       //смотрим в каком подразделении находится табельный номер
       sprintf(strsql,"select podr from Zarn where tabn=%d and god=%d and mes=%d",
       tn,gp,mp);
       if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
        if(atoi(row[0]) == podr_v_str)
         {
          if(ff_prot != NULL)
            fprintf(ff_prot,"Находится в подразделении %d\n",podr_v_str);
          metka_ras=1;
         }
      }
    }    

   if(tn1 == tn || metka_ras == 1)
    {
     koefpv=nah=0.;
     nomz=1;
     if(ff_prot != NULL)
       fprintf(ff_prot,"\nРасчет по процентам.\n\
---------------------------------------------------\n");
     /*Входной код*/
     memset(kn1,'\0',sizeof(kn1));
     iceb_u_polen(row_alx[0],kn1,sizeof(kn1),2,'|');

     /*Если первый символ не число то это ссылка на строку с настройкой списка кодов*/
     iceb_u_ud_simv(kn1," "); /*удаляем пробелы если они есть*/
     if(isdigit(kn1[0]) == 0) /*Значит первый символ не число*/
      {
       if(ff_prot != NULL)
        fprintf(ff_prot,"Ищем настройку %s\n",kn1);
       class iceb_u_str spn("");
       iceb_poldan(kn1,&spn,"zarproc.alx",wpredok);
       strncpy(kn1,spn.ravno(),sizeof(kn1));      
       if(ff_prot != NULL)
        fprintf(ff_prot,"Список входных кодов:%s\n",kn1);
      }

     /*Выходной код*/
     zp.nomz=nomzap=0;
     memset(br,'\0',sizeof(br));
     iceb_u_polen(row_alx[0],br,sizeof(br),3,'|');
     kn2=(short)iceb_u_atof(br);
     if(iceb_u_proverka(nah_only,kn2,0,0) != 0)
      {
       if(ff_prot != NULL)
        fprintf(ff_prot,"Код %d исключён из расчёта\n",kn2);
       continue;
      }
     if(ff_prot != NULL)
      {
       fprintf(ff_prot,"Рaсчёт начисления:%d",kn2);
       sprintf(strsql,"select naik from Nash where kod=%d",kn2);
       if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
        fprintf(ff_prot," %s",row[0]);
       fprintf(ff_prot,"\n");
      }    
         
     kodud=0;
     if(iceb_u_polen(br,str,sizeof(str),2,'/') == 0)
      kodud=(short)iceb_u_atof(str);
     if(kodud != 0)
      {
       memset(shetuder,'\0',sizeof(shetuder));
       if(ff_prot != NULL)
        fprintf(ff_prot,"kodud=%d\n",kodud);
       sprintf(strsql,"select shet from Uder where kod=%d",kodud);
       if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
         strncpy(shetuder,row[0],sizeof(shetuder)-1);
       else
        {
         if(ff_prot != NULL)
          fprintf(ff_prot,"Не найден код удержания %d в таблице удержаний !\n",
          kodud);
         kodud=0;
        }
      }
     if(iceb_u_polen(br,str,sizeof(str),2,':') == 0)
      zp.nomz=nomzap=(short)iceb_u_atof(str);

     memset(shet,'\0',sizeof(shet));
     iceb_u_polen(br,shet,sizeof(shet),3,':');

     /*Процент*/
     iceb_u_polen(row_alx[0],&proc,4,'|');

     /*Тариф*/
     mhs=0;
     iceb_u_polen(row_alx[0],&tarif_str,5,'|');

     tarif=0.;
     if(iceb_u_strstrm(tarif_str.ravno(),"окл") == 1)
      tarif=okladw(tn,mp,gp,&mhs,wpredok);
     else
      tarif=tarif_str.ravno_atof();

     memset(uprp,'\0',sizeof(uprp));
     memset(kodpodr,'\0',sizeof(kodpodr));
     memset(den,'\0',sizeof(den));

     iceb_u_polen(tarif_str.ravno(),den,sizeof(den),2,':');
     iceb_u_polen(tarif_str.ravno(),uprp,sizeof(uprp),3,':');
     iceb_u_polen(tarif_str.ravno(),kodpodr,sizeof(kodpodr),4,':');

     if(iceb_u_strstrm(tarif_str.ravno(),"/") != 0) /*номер тарифа/разряд*/
      {
       int nomt=atoi(br);
       int razrad=0;
       iceb_u_polen(br,&razrad,2,'/');
      
       if(ff_prot != NULL)
        fprintf(ff_prot,"Номер тарифа/разряд: %d/%d\n",nomt,razrad);
       tarif=zartarroz1w(nomt,razrad,ff_prot);

       if(ff_prot != NULL)
        fprintf(ff_prot,"Берём в расчёт=%f\n",tarif);
       
      }

     if(iceb_u_strstrm(tarif_str.ravno(),"Ч") != 0)
      {
       if(ff_prot != NULL)
         fprintf(ff_prot,"Часовая ставка.\n");

	mhs=1;
      }
     if(iceb_u_strstrm(tarif_str.ravno(),"*") != 0)
      {
       if(ff_prot != NULL)
         fprintf(ff_prot,"Оклад не зависящий от отработанного времени.\n");
	mhs=2;
      }
     if(ff_prot != NULL && mhs == 0)
         fprintf(ff_prot,"Оклад зависящий от отработанного времени.\n");

     /*Месяц*/
     memset(mesqc,'\0',sizeof(mesqc));
     iceb_u_polen(row_alx[0],mesqc,sizeof(mesqc),6,'|');
     gg=gp;
     mm=mp;
     if(iceb_u_strstrm(mesqc,"п") == 1)
      {
       mhs=2;
       mm-=1;
       if(mm == 0)
        {
         mm=12;
         gg-=1;
        }
      }

     /*Исключающий код*/
     iceb_u_polen(row_alx[0],str,sizeof(str),7,'|');
     if((iskkod=atoi(str)) != 0)
      {
       sprintf(strsql,"select suma from Zarp where datz >= '%04d-%d-01' \
and  datz <= '%04d-%d-31' and tabn=%d and prn='1' and knah=%d and \
suma <> 0.",gp,mp,gp,mp,tn1,iskkod);
       if(iceb_sql_readkey(strsql,wpredok) > 0)
        {
         if(ff_prot != NULL)
          fprintf(ff_prot,"Есть не нулевой исключающий код %d !\n",iskkod);
         continue;
        }
      }

    /*rintw("br-%s i=%d\n",br,i);*/
     /*Проверяем есть ли код выходной начисления в списке*/

     sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='1' \
and knah=%d",tn,kn2); 
     if(iceb_sql_readkey(strsql,&row,&curr,wpredok) != 1)
      {
       if(ff_prot != NULL)
         fprintf(ff_prot,"Нет кода начисления %d в списке  TN%d !\n",kn2,tn);
       continue; 
      }
     if(shet[0] == '\0')
       strncpy(shet,row[0],sizeof(shet)-1);

     /*Читаем суммы по входным кодам если они есть*/
     sum=sumb=0.;

     if(iceb_u_polen(kn1,br,sizeof(br),1,',') != 0)
       strcpy(br,kn1);
     i=1;
     do
      {
       i++;
       sprintf(strsql,"select suma,shet from Zarp where datz >= '%d-%02d-01' \
and datz <= '%d-%02d-31' and tabn=%d and prn='1' and knah=%d and suma <> 0.",
       gg,mm,gg,mm,tn,atoi(br));
/*       printw("%s\n",strsql);*/
       if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
        iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

       if(kolstr <= 0)
         continue;

       while(cur.read_cursor(&row) != 0)
        {
    
         sum+=atof(row[0]);         
         if(iceb_u_proverka(shetb,row[1],0,1) == 0)
          {
           sumb+=atof(row[0]);
           strncpy(shetbd,row[1],sizeof(shetbd)-1);
          }           
         else
           strncpy(shetnbd,row[1],sizeof(shetnbd)-1);
           
        }

      }
     while(iceb_u_polen(kn1,br,sizeof(br),i,',') == 0);

     if(tarif == 0.)
      {
       if(ff_prot != NULL)
        {
         fprintf(ff_prot,"Тариф равен нолю !\n");
         fprintf(ff_prot,"sum=%.2f sumb=%.2f proc=%.2f\n",
         sum,sumb,proc);
        }
       nah=sum*proc/100.;
       nahb=sumb*proc/100.;

//       printw("nah=%f nahb=%f\n",nah,nahb);
//       OSTANOV();
       koefpv=proc/100.;
      }
     else
      {
       if(ff_prot != NULL)
         fprintf(ff_prot,"Тариф:%.2f\n",tarif);
       /*Чтение табеля*/
       sprintf(strsql,"select kodt from Nash where kod=%d",
       kn2);
//       printw("%s\n",strsql);
       if(iceb_sql_readkey(strsql,&row,&curr,wpredok) != 1)
        {
         sprintf(strsql,"procent-Не найден код начисления %d",kn2);
         iceb_menu_soob(strsql,wpredok);
         continue;
        }
       
       ktab=atoi(row[0]);
       if(ff_prot != NULL)
        fprintf(ff_prot,"Код табеля=%d для кода начисления %d\n",ktab,kn2);
/*
       if(ktab == 0)
        {
         beep();
         printw("%s %d\n",gettext("procent-Не введён код табеля для начисления"),kn2);
         OSTANOV();
         continue; 
        }
*/
       sprintf(strsql,"select has,dnei,kolrd from Ztab where \
god=%d and mes=%d and tabn=%d and kodt=%d order by nomz asc",
       gp,mp,tn,ktab);
//       printw("\n%s\n",strsql);
//       OSTANOV();
       has=odnei=0.;
       if(sql_readkey(&bd,strsql,&row,&curr) > 0)
        {
         has=atof(row[0]);
         odnei=atof(row[1]);
         if(ff_prot != NULL)
          fprintf(ff_prot,"Прочитано часов=%f дней=%f Количество рабочих дней в месяце %s\n",
          has,odnei,row[2]);

         if(row[2][0] != '0')
           dneir=atof(row[2]);
         else
          if(ktab != 1)
           {
            sprintf(strsql,"select kolrd from Ztab where \
god=%d and mes=%d and tabn=%d and kodt=1 order by nomz asc",gp,mp,tn);
//       printw("%s\n",strsql);

            if(sql_readkey(&bd,strsql,&row,&curr) > 0)
             {
              if(atof(row[0]) != 0.)
                dneir=atof(row[0]);
              if(ff_prot != NULL)
                fprintf(ff_prot,"Берем для расчета %f рабочих дней в месяце.\n",
                dneir);                
             }
           }
          else
           {
            dneir=dnei;
            hasovr=hasov;
          
           }
        }
       else
        {
         hasovr=dneir=0.;
        }

       if(proc != 0.)
	{
	 if(mhs == 0)
          {
//           if(den[0] == 'Д' || den[0] == 'д')
           if(iceb_u_strstrm(den,"Д") == 1)
            {
             if(dneir != 0.)
   	      {
   	       nah=tarif/dneir*odnei*proc/100.;  /*Оклад зависящий от отработ. врем*/
               koefpv=dneir/odnei*proc/100.;
   	       if(ff_prot != NULL)
 	        fprintf(ff_prot,"Расчет по отработанным дням.\n\
nah=%.2f/%.2f*%.2f*%.2f/100.=%.2f\n",
                tarif,dneir,odnei,proc,nah);
              }
             else
 	       if(ff_prot != NULL)
                fprintf(ff_prot,"Не введено количество рабочих дней в месяце !\n");              
            }
           else
            {
             if(hasovr != 0.)
  	      {
  	       nah=tarif/hasovr*has*proc/100.;  /*Оклад зависящий от отработ. врем*/
               koefpv=hasovr/has*proc/100.;
 	       if(ff_prot != NULL)
 	        fprintf(ff_prot,"Расчет по отработанным часам.\n\
nah=%.2f/%.2f*%.2f*%.2f/100.=%.2f\n",
                tarif,hasovr,has,proc,nah);
              }
             else
 	       if(ff_prot != NULL)
                fprintf(ff_prot,"Не введено количество рабочих часов в месяце !\n");
            }
          }
	 if(mhs == 1) /*Часовая ставка*/
          {
 	   nah=tarif*has*proc/100.;
           koefpv=proc/100.;
 	   if(ff_prot != NULL)
 	    fprintf(ff_prot,"Расчет по часовой ставке.\n\
nah=%.2f*%.2f*%.2f/100.=%.2f\n",
            tarif,has,proc,nah);
          }
	 if(mhs == 2) /*Оклад не зависящий от отработанного времени*/
          {
 	   nah=tarif*proc/100.;
           koefpv=proc/100.;
 	   if(ff_prot != NULL)
 	    fprintf(ff_prot,"Не зависит от отработанного времени.\n\
nah=%.2f*%.2f/100.=%.2f\n",
            tarif,proc,nah);
          }
  /*     
	 printw("nah=%.2f tarif=%.2f mhs=%d has=%.2f\n\n",
	 nah,tarif,mhs,has);
*/	 
	}
       if(proc == 0.)
        {
         if(ff_prot != NULL)
          fprintf(ff_prot,"Сумма не зависящая ни от чего %.2f.\n",tarif);
         
         nah=tarif;
         koefpv=1.;
        }
      }
     /*Учет выполения плана подразделением*/
     if(ff_prot != NULL)
       fprintf(ff_prot,"uprp=%s kodpodr=%s nah=%.2f nahb=%.2f\n",
       uprp,kodpodr,nah,nahb);
//     if(uprp[0] == (char)'П' || uprp[0] == (char)'п')
     if(iceb_u_strstrm(uprp,"П") == 1)
      {
       if((i=(int)iceb_u_atof(kodpodr)) == 0)
          i=podr;

       procentp=zarpodprw(i,&procentpb,wpredok);

       koefpv*=procentp/100.;
       if(ff_prot != NULL)
         fprintf(ff_prot,"nah=%.2f procentp=%f koefpv=%f\n",nah,procentp,koefpv);
         
      }     
     nah=iceb_u_okrug(nah,okrg);
     nahb=iceb_u_okrug(nahb,okrg);
/*      
     printw("kn2=%d nah=%.2f\n",kn2,nah);
     OSTANOV();      
*/
     if(nahb != 0. && shetb != NULL)
      {
       if(ff_prot != NULL)
        {
         fprintf(ff_prot,"Сумма %f Бюджет %f/%s Не бюджет %s\n",
         nah,nahb,shetbd,shetnbd);
        }
 
       memset(br,'\0',sizeof(br));

       strcpy(zp.shet,shetbd);
       bb=nahb;        
//       if(uprp[0] == (char)'П' || uprp[0] == (char)'п')
     if(iceb_u_strstrm(uprp,"П") == 1)
        {
         bb=nahb*procentpb/100.;
         if(ff_prot != NULL)
          fprintf(ff_prot,"Бюджет %.2f*%f/100.=%.2f Счет:%s\n",nahb,procentpb,bb,shetbd);
        }

       if(shetbd[0] != '\0')
        {
         strcpy(zp.shet,shetbd);
         zp.knu=kn2;
         zapzarpw(&zp,bb,d,mp,gp,0,shet,"",0,podr,"",wpredok);
//         zapzarp(d,mp,gp,tn,1,kn2,bb,shetbd,mp,gp,0,nomzap,br,podr,"",zp); 
         if(kodud != 0)
          {
           strcpy(zp.shet,shetuder);
           zp.knu=kodud;
           zapzarpw(&zp,nah,d,mp,gp,0,shet,"",0,podr,"",wpredok);
//           zapzarp(d,mp,gp,tn,2,kodud,bb*-1,shetuder,mp,gp,0,nomzap,br,podr,"",zp); 
          }       
        }
       memset(br,'\0',sizeof(br));

       bb=nah-nahb;        

//       if(uprp[0] == (char)'П' || uprp[0] == (char)'п')
       if(iceb_u_strstrm(uprp,"П") == 1)
        {
         bb=(nah-nahb)*procentp/100.;
         if(ff_prot != NULL)
          fprintf(ff_prot,"(%.2f-%.2f)*%f/100.=%.2f Счет:%s\n",
          nah,nahb,procentp,bb,shetnbd);
        }

       if(shetnbd[0] != '\0')
        {
         strcpy(zp.shet,shetnbd);
         zp.knu=kn2;
         zapzarpw(&zp,nah,d,mp,gp,0,shet,"",0,podr,"",wpredok);
//         zapzarp(d,mp,gp,tn,1,kn2,bb,shetnbd,mp,gp,0,nomzap,br,podr,"",zp); 
         if(kodud != 0)
          {
           strcpy(zp.shet,shetuder);
           zp.knu=kodud;
           zapzarpw(&zp,nah,d,mp,gp,0,shet,"",0,podr,"",wpredok);
//           zapzarp(d,mp,gp,tn,2,kodud,bb*-1,shetuder,mp,gp,0,nomzap,br,podr,"",zp); 
          }
        }
      }
     else
      {
       memset(br,'\0',sizeof(br));
       if(proc != 0.)
        sprintf(br,"%.2f%% %s",proc,tarif_str.ravno());       
       strcpy(zp.shet,shet);

       bb=nah;        
//       if(uprp[0] == (char)'П' || uprp[0] == (char)'п')
       if(iceb_u_strstrm(uprp,"П") == 1)
        {
         bb=nah*procentp/100.;
         if(ff_prot != NULL)
          fprintf(ff_prot,"Хозрасчет %.2f*%f/100.=%.2f Счет:%s\n",nah,procentp,bb,shet);
        }
       else
        {
         if(ff_prot != NULL)
          fprintf(ff_prot,"Сумма=%.2f Счет:%s\n",bb,shet);
        }
       strcpy(zp.shet,shet);
       zp.knu=kn2;
       zapzarpw(&zp,nah,d,mp,gp,0,shet,"",0,podr,"",wpredok);
//       zapzarp(d,mp,gp,tn,1,kn2,bb,shet,mp,gp,0,nomzap,br,podr,"",zp); 
       if(kodud != 0)
        {
         if(ff_prot != NULL)
          fprintf(ff_prot,"Удержание=%.2f shetuder=%s kodud=%d\n",bb,shetuder,kodud);
         strcpy(zp.shet,shetuder);
         zp.knu=kodud;
         zapzarpw(&zp,nah,d,mp,gp,0,shet,"",0,podr,"",wpredok);
//         zapzarp(d,mp,gp,tn,2,kodud,bb*-1,shetuder,mp,gp,0,nomzap,br,podr,"",zp); 
        }
      }

     /*Проверяем на коды отдельного расчета доплаты до минимальной зарплаты*/
     if(kodotrs != NULL && koddopmin != 0)
      {

       /*Проверяем есть ли код доплаты в списке*/

       sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='1' and \
       knah=%d",tn,koddopmin); 
       if(sql_readkey(&bd,strsql,&row,&curr) < 1)
         continue;

       strncpy(shet,row[0],sizeof(shet)-1);

       for(i1=1; i1 <= kodotrs[0]; i1++)
        if(kn2 == kodotrs[i1])
         break;
       if(i1 > kodotrs[0])
         continue;

       class zar_read_tn1h nastr;
       zar_read_tn1w(1,mp,gp,&nastr,ff_prot,wpredok);

       doplata=nastr.minzar*koefpv-nah;

       if(ff_prot != NULL)
        {
         fprintf(ff_prot,"Расчет доплаты до минимальной зарплаты.\n\
%.2f*%f-%.2f=%.2f (i1=%d kodotrs[0]=%d)\n",
         nastr.minzar,koefpv,nah,doplata,i1,kodotrs[0]);
        }
       if(doplata < 0.01)
          continue;
       doplata=iceb_u_okrug(doplata,okrg);
       sprintf(br,"%d",kn2);
       strcpy(zp.shet,shet);
       zp.knu=koddopmin;
       zapzarpw(&zp,nah,d,mp,gp,0,shet,"",0,podr,"",wpredok);
//       zapzarp(d,mp,gp,tn,1,koddopmin,doplata,shet,mp,gp,0,nomz++,br,podr,"",zp); 

      }
    }
  }
}
