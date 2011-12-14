//---------------------------------------------------------------------------

#ifndef Unit2H
#include <vcl.h>
#include <jpeg.hpp>
#include <iostream.h>
#include <conio>
#include <stdlib.h>

#define Unit2H
#include <math.h>
//---------------------------------------------------------------------------
int round(double x)
{
        int y=0;
        if(x-int(x)<0.5)y=int(x);
                else y=int(x)+1;
        return y;
}
//------------------------------------------------------------------------
class CBmp
{
public:
        string path;

        int szer,wys;
        int **tab0;
        int obiekt,licz;
        int *pola_ob,*obwody_ob;
        float *srodki_x,*srodki_y,*momenty_bezw;


        int wczytaj_obr();
        int wypisz(int x,TMemo *memo);
        int wyswietl(TImage *image);
        int wyswietl_org(TImage *image);
        int binaryzacja_niblack(float k,float R,float R1,int grid);
        int segmentacja();
        int policz_obwody();
        int obcinaj_obiekty_tla(int tres_min_pole,int tres_max_pole,int tres_min_obw,int tres_max_obw);
        int brzegi();
        int policz_srodek();
        int policz_moment_b();
        int element(int ktory,TImage *image,TMemo *memo);
        int policz_kat(int ktory,TMemo *memo);
};
//------------------------------------------------------------------------------
CBmp::wczytaj_obr()
        {
        int *ptr;
        Graphics::TBitmap *obr = new Graphics::TBitmap;
        obr->PixelFormat = pf32bit;
        obr->LoadFromFile(path.c_str());
        szer = obr->Width;
        wys = obr->Height;
        tab0= new int *[szer];
        for(int i = 0; i < szer; i++)
                {
                tab0[i] = new int [wys];
                }

        for(int j=0;j<wys;j++)
        {
        ptr = (int *)obr->ScanLine[j];
        for (int i = 0; i < szer; i++)tab0[i][j]=GetRValue(ptr[i]);
        }
        delete obr;
        }
//------------------------------------------------------------------------------
CBmp::wypisz(int x,TMemo *memo){
        string a,b;
        b="Pole: ";
        a =b + IntToStr(pola_ob[x]).c_str();
        memo->Lines->Add(a.c_str());
        b="Obwod: ";
        a =b + IntToStr(obwody_ob[x]).c_str();
        memo->Lines->Add(a.c_str());
        b="Polozenie: ";
        memo->Lines->Add(b.c_str());
        b="x: ";
        a =b + FloatToStr(srodki_x[x]).c_str();
        memo->Lines->Add(a.c_str());
        b="y: ";
        a =b + FloatToStr(srodki_y[x]).c_str();
        memo->Lines->Add(a.c_str());
        }
//------------------------------------------------------------------------------

CBmp::wyswietl(TImage* image)
        {
        int **tab1;
        tab1 = new int *[szer];
        for(int i = 0; i < szer; i++)
                {
                tab1[i] = new int [wys];
                }
        for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)tab1[i][j]=tab0[i][j];
        for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)if(tab1[i][j])tab1[i][j]=255;

        int* ptr;
        Graphics::TBitmap *Rysunek = new Graphics::TBitmap;
        Rysunek->PixelFormat = pf32bit;
        RECT Rect;
        Rect.top = 0;
        Rect.left = 0;
        Rect.right = image->Width;
        Rect.bottom = image->Height;
        Rysunek->Width = szer;
        Rysunek->Height = wys;

        for(int j=0;j<wys;j++)
                        {
                        ptr = (int *)Rysunek->ScanLine[j];
                        for(int i=0;i<szer;i++)
                                                {
                                                int a=tab1[i][j];
                                                ptr[i]=RGB(a,a,a);
                                                }
                        }
        image->Canvas->StretchDraw(Rect,Rysunek);
        image->Visible = true;
        for(int i = 0; i < szer; i++)
                {
                delete [] tab1[i];
                }
                delete []tab1;
                delete Rysunek;

        return 0;
        }
//------------------------------------------------------------------------------
CBmp::wyswietl_org(TImage* image)
        {
        int **tab1;
        tab1 = new int *[szer];
        for(int i = 0; i < szer; i++)
                {
                tab1[i] = new int [wys];
                }
        for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)tab1[i][j]=tab0[i][j];
        
        int* ptr;
        Graphics::TBitmap *Rysunek = new Graphics::TBitmap;
        Rysunek->PixelFormat = pf32bit;
        RECT Rect;
        Rect.top = 0;
        Rect.left = 0;
        Rect.right = image->Width;
        Rect.bottom = image->Height;
        Rysunek->Width = szer;
        Rysunek->Height = wys;

        for(int j=0;j<wys;j++)
                        {
                        ptr = (int *)Rysunek->ScanLine[j];
                        for(int i=0;i<szer;i++)
                                                {
                                                int a=tab1[i][j];
                                                ptr[i]=RGB(a,a,a);
                                                }
                        }
        image->Canvas->StretchDraw(Rect,Rysunek);
        image->Visible = true;
        for(int i = 0; i < szer; i++)
                {
                delete [] tab1[i];
                }
                delete []tab1;
                delete Rysunek;

        return 0;
        }
//------------------------------------------------------------------------------
CBmp::binaryzacja_niblack(float k,float R,float R1,int grid)    //u¿ywa algorytmu Niblack'a
                {
                int x=0,y=0,x1=0,y1=0;
                //ilosc obszarow w pionie i w poziome
                for(int l=0;l<wys;l+=grid)
                        {
                        y1=y;
                        y=0;
                        x++;
                        for(int i=0;i<szer;i+=grid)y++;
                        }
                x1=x;
                //tablica przechowujaca wartosci srednich jasnosci dla wszystkich obszarow
                float **avg;
                avg = new float *[x1];
                for(int i = 0; i < x1; i++)avg[i] = new float [y1];
                for(int i=0;i<x1;i++)for(int j=0;j<y1;j++)avg[i][j]=0;
                //obliczamy srenia jasnosc dla kazdego obszaru
                x=0;
                y=0;
                for(int l=0;l<wys;l+=grid)
                        {
                        y=0;
                        x++;
                        for(int i=0;i<szer;i+=grid)
                                {
                                y++;
                                for(int j=i;j<=i+grid;j++)
                                        {
                                        if(j==szer)break;
                                        for(int k=l;k<l+grid;k++)
                                                {
                                                if(k==wys)break;
                                                avg[x-1][y-1]+=tab0[j][k];
                                                }
                                        }
                                }
                        }
                for(int i=0;i<x1;i++)for(int j=0;j<y1;j++)avg[i][j]=avg[i][j]/(grid*grid);
                //tablica przechowujaca wartosci odchylen standardowych dla wszystkich obszarow
                float **std_dev;
                std_dev = new float *[x1];
                for(int i = 0; i < x1; i++)std_dev[i] = new float [y1];
                for(int i=0;i<x1;i++)for(int j=0;j<y1;j++)std_dev[i][j]=0;
                //odchylenie standardowe dla kazdego obszaru
                x=0;
                y=0;
                for(int l=0;l<wys;l+=grid)
                        {
                        y=0;
                        x++;
                        for(int i=0;i<szer;i+=grid)
                                {
                                y++;
                                for(int j=i;j<=i+grid;j++)
                                        {
                                        if(j==szer)break;
                                        for(int k=l;k<l+grid;k++)
                                                {
                                                if(k==wys)break;
                                                std_dev[x-1][y-1]+=pow((tab0[j][k]-avg[x-1][y-1]),2);
                                                }
                                        }
                                }
                        }
                for(int i=0;i<x1;i++)for(int j=0;j<y1;j++)
                        {
                        std_dev[i][j]=pow(std_dev[i][j]/(grid*grid),0.5);
                        }
                // binaryzacja
                int tres;
                x=0;
                y=0;
                for(int l=0;l<wys;l+=grid)
                        {
                        y=0;
                        x++;
                        for(int i=0;i<szer;i+=grid)
                                {
                                y++;
                                tres=k*pow(avg[x-1][y-1],R1)-R;
                                for(int j=i;j<=i+grid;j++)
                                        {
                                        if(j==szer)break;
                                        for(int k=l;k<l+grid;k++)
                                                {
                                                if(k==wys)break;
                                                if(tab0[j][k]<tres)tab0[j][k]=0;
                                                        else tab0[j][k]=255;
                                                }
                                        }
                                }
                        }
                for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)if(tab0[i][j]==0)tab0[i][j]=255;
                                                                        else tab0[i][j]=0;
                for(int i=0;i<x1;i++)delete [] avg[i];
                delete [] avg;
                for(int i = 0; i < x1; i++)delete [] std_dev[i];
                delete [] std_dev;
                }
//------------------------------------------------------------------------------
CBmp::segmentacja()
        {
        int n=1;
        for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)
                                if(tab0[i][j])
                                        {
                                         tab0[i][j]=n;
                                         n++;
                                        }


        int min;
        int ile=70;
        for(int a=0;a<ile;a++){
        for(int i=1;i<szer-1;i++)for(int j=1;j<wys-1;j++)
                                {
                                if(tab0[i][j]){
                                                min=szer*wys;
                                                for(int k=i-1;k<i+2;k++)for(int l=j-1;l<j+2;l++)if(tab0[k][l]<min && tab0[k][l])min=tab0[k][l];
                                                tab0[i][j]=min;
                                                }
                                }
                                }
        for(int a=0;a<ile;a++){
        for(int i=szer-2;i>1;i--)for(int j=wys-2;j>1;j--)
                                if(tab0[i][j]){
                                min=szer*wys;
                                for(int k=i-1;k<i+2;k++)for(int l=j-1;l<j+2;l++)if(tab0[k][l]<min && tab0[k][l])min=tab0[k][l];
                                tab0[i][j]=min;
                                }
                                }

        int *obiekty;
        obiekty = new int [szer*wys];
        for(int i=0;i<szer*wys;i++)obiekty[i]=0;
        licz=1;
        bool czy;

        for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)
                                                        {
                                                        czy = true;
                                                        if(tab0[i][j]){
                                                                        for(int k=1;k<licz;k++)if(obiekty[k]==tab0[i][j])czy=false;
                                                                        if(czy){
                                                                                obiekty[licz]=tab0[i][j];
                                                                                licz++;
                                                                                }
                                                                        }
                                                        }
        for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)
                                                if(tab0[i][j])
                                                for(int k=1;k<licz;k++)if(tab0[i][j]==obiekty[k]){
                                                                                                     tab0[i][j]=k;
                                                                                                     break;
                                                                                                    }
        delete[] obiekty;

        pola_ob = new int[licz];
        for(int i=0;i<licz;i++)pola_ob[i]=0;
        for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)pola_ob[tab0[i][j]]++;
        }
//------------------------------------------------------------------------------
CBmp::policz_obwody()
{
obwody_ob = new int[licz];
for(int i=0;i<licz;i++)obwody_ob[i]=0;
bool czy;
for(int i=1;i<szer-1;i++)for(int j=1;j<wys-1;j++)
        {
        czy =false;
        if(tab0[i][j])for(int k=i-1;k<i+2;k++)for(int l=j-1;l<j+2;l++)if(tab0[k][l]==0)czy=true;
        if(czy)obwody_ob[tab0[i][j]]++;
        }
}
//------------------------------------------------------------------------------
CBmp::obcinaj_obiekty_tla(int tres_min_pole,int tres_max_pole,int tres_min_obw,int tres_max_obw)
{
for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)if(pola_ob[tab0[i][j]]<tres_min_pole || pola_ob[tab0[i][j]]>tres_max_pole)
                                                                                                                {
                                                                                                                pola_ob[tab0[i][j]]=0;
                                                                                                                obwody_ob[tab0[i][j]]=0;
                                                                                                                tab0[i][j]=0;
                                                                                                                }
for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)if(obwody_ob[tab0[i][j]]<tres_min_obw || obwody_ob[tab0[i][j]]>tres_max_obw)
                {
                pola_ob[tab0[i][j]]=0;
                obwody_ob[tab0[i][j]]=0;
                tab0[i][j]=0;
                }
obiekt=0;
for(int i=1;i<licz;i++)if(pola_ob[i])obiekt++;
}
//------------------------------------------------------------------------------
CBmp::brzegi()
{
int lewy_bok=30,gora=50,dol=145,cypel=190;
for(int i=0;i<lewy_bok;i++)for(int j=0;j<wys;j++)
                {
                pola_ob[tab0[i][j]]=0;
                obwody_ob[tab0[i][j]]=0;
                tab0[i][j]=0;
                }
for(int i=0;i<szer;i++)for(int j=0;j<gora;j++)
                {
                pola_ob[tab0[i][j]]=0;
                obwody_ob[tab0[i][j]]=0;
                tab0[i][j]=0;
                }
for(int i=0;i<szer;i++)for(int j=wys-dol;j<wys;j++)
                {
                pola_ob[tab0[i][j]]=0;
                obwody_ob[tab0[i][j]]=0;
                tab0[i][j]=0;
                }
for(int i=szer/2-70;i<szer/2+130;i++)for(int j=wys-cypel;j<wys;j++)
                {
                pola_ob[tab0[i][j]]=0;
                obwody_ob[tab0[i][j]]=0;
                tab0[i][j]=0;
                }
obiekt=0;
for(int i=1;i<licz;i++)if(pola_ob[i])obiekt++;
}
//------------------------------------------------------------------------------
CBmp::policz_srodek()
{
srodki_x = new float[licz];
srodki_y = new float[licz];
for(int i=0;i<licz;i++)srodki_x[i]=0;
for(int i=0;i<licz;i++)srodki_y[i]=0;
for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)if(tab0[i][j])
        {
         srodki_x[tab0[i][j]]+=i;
         srodki_y[tab0[i][j]]+=j;
        }
for(int i=0;i<licz;i++)if(pola_ob[i])srodki_x[i]=srodki_x[i]/pola_ob[i];
for(int i=0;i<licz;i++)if(pola_ob[i])srodki_y[i]=srodki_y[i]/pola_ob[i];
}
//------------------------------------------------------------------------------
CBmp::policz_moment_b()
{
momenty_bezw = new float[licz];
for(int i=0;i<licz;i++)momenty_bezw[i]=0;
for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)if(tab0[i][j])
        {
        momenty_bezw[tab0[i][j]]+=pow(i-srodki_x[tab0[i][j]],2)+pow(j-srodki_y[tab0[i][j]],2);
        }
}
//------------------------------------------------------------------------------
CBmp::element(int ktory,TImage *image,TMemo *memo)
{
wyswietl(image);
memo->Text = "";
int j=0,x;
for(x=0;x<licz;x++)
{
if(pola_ob[x])j++;
if(j==ktory)break;
}
        int **tab1;
        tab1 = new int *[szer];
        for(int i = 0; i < szer; i++)
                {
                tab1[i] = new int [wys];
                }
for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)tab1[i][j]=tab0[i][j];
for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)if(tab1[i][j]!=x)tab1[i][j]=0;
for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)if(tab1[i][j])tab1[i][j]=255;
string a,b;
bool czy = false;
if(pola_ob[x]>5770 && pola_ob[x]<7020)if(momenty_bezw[x]>24000000 && momenty_bezw[x]<31000000)
        {
        memo->Lines->Add("Kondensator typ 3");
        wypisz(x,memo);
        czy = true;
        }
if(pola_ob[x]>5725 && pola_ob[x]<7020)if(momenty_bezw[x]>48700000 && momenty_bezw[x]<97600000)
        {
        memo->Lines->Add("Rezystor typ 4");
        wypisz(x,memo);
        czy = true;
        }

if(pola_ob[x]/obwody_ob[x]<3)if(momenty_bezw[x]>9500000 && momenty_bezw[x]<11900000)
        {
        memo->Lines->Add("Kondensator typ 1");
        wypisz(x,memo);
        czy = true;
        }
if(pola_ob[x]>1785 && pola_ob[x]<2420)
        {
        memo->Lines->Add("Rezystor typ 1");
        wypisz(x,memo);
        czy = true;
        }
if(pola_ob[x]>3750 && pola_ob[x]<4350)if(momenty_bezw[x]>13480000 && momenty_bezw[x]<19320000)
        {
        memo->Lines->Add("Kondensator typ 2");
        wypisz(x,memo);
        czy = true;
        }
if(pola_ob[x]>5390 && pola_ob[x]<5650)
        {
        memo->Lines->Add("Rezystor typ 2");
        wypisz(x,memo);
        czy = true;
        }
if(pola_ob[x]>5000 && pola_ob[x]<5050)       //dal rezystora z "ucieta" noga
        {
        memo->Lines->Add("Rezystor typ 6");
        wypisz(x,memo);
        czy = true;
        }
if(pola_ob[x]>5620 && pola_ob[x]<5630)
        {
        memo->Lines->Add("Rezystor typ 3");
        wypisz(x,memo);
        czy = true;
        }
if(pola_ob[x]>10895 && pola_ob[x]<11600)
        {
        memo->Lines->Add("Kondensator typ 4");
        wypisz(x,memo);
        czy = true;
        }
if(pola_ob[x]>9370 && pola_ob[x]<9720)if(pola_ob[x]/obwody_ob[x]>=6 && pola_ob[x]/obwody_ob[x]<8.8)
        {
        memo->Lines->Add("Tranzystor");
        wypisz(x,memo);
        czy = true;
        }
if(pola_ob[x]>7145 && pola_ob[x]<7795)
        {
        memo->Lines->Add("Uklad scalony typ 2");
        wypisz(x,memo);
        czy = true;
        }
if(obwody_ob[x]>275 && obwody_ob[x]<420)
        {
        memo->Lines->Add("Uklad scalony typ 1");
        wypisz(x,memo);
        czy = true;
        }
if(pola_ob[x]<9900 && pola_ob[x]>9150)if(pola_ob[x]/obwody_ob[x]>=12 && pola_ob[x]/obwody_ob[x]<=14)
        {
        memo->Lines->Add("Uklad scalony typ 3");
        wypisz(x,memo);
        czy = true;
        }
if(pola_ob[x]>8780 && pola_ob[x]<9570)if(momenty_bezw[x]>82300000 && momenty_bezw[x]<98700000)
        {
        memo->Lines->Add("Rezystor typ 5");
        wypisz(x,memo);
        czy = true;
        }
if(pola_ob[x]>10000 && pola_ob[x]<11000)if(momenty_bezw[x]>100500000 && momenty_bezw[x]<112700000)
        {
        memo->Lines->Add("Rezystor typ 6");
        wypisz(x,memo);
        czy = true;
        }
if(!czy)
        {
        memo->Lines->Add("XXXXXX");
        memo->Lines->Add(FloatToStr(momenty_bezw[x]));
        wypisz(x,memo);
        czy = true;
        }

        int xx = (srodki_x[x])/2.812;
        int yy = (srodki_y[x])/2.771;
        image->Canvas->Font->Color=clGreen;
        image->Canvas->Font->Size=15;
        image->Canvas->TextOutA(xx,yy,IntToStr(ktory));
       
        for(int i = 0; i < szer; i++)
                {
                delete [] tab1[i];
                }
                delete [] tab1;
}
//------------------------------------------------------------------------------
CBmp::policz_kat(int ktory,TMemo *memo)
{
double kat=0;
int j=0,x;
for(x=0;x<licz;x++)
{
        if(pola_ob[x])j++;
        if(j==ktory)break;
}
int xYmin=0,yYmax=0,xYmax=0,xXmin=szer+100,yXmin=0,xXmax=0,yXmax=0,yYmin=wys+100;
for(int i=0;i<szer;i++)
for(int j=0;j<wys;j++)
        if(tab0[i][j]==x)
        {
        if(i<xXmin){xXmin=i;
                    yXmin=j;
                    }
        if(i>xXmax){xXmax=i;
                    yXmax=j;
                    }
        if(j>yYmax){yYmax=j;
                    xYmax=i;
                    }
        if(j<yYmin){yYmin=j;
                    xYmin=i;
                    }
        }
float tx=0,o,p;
string a,b;
if((pow(xYmin-xXmin,2)+pow(yXmin-yYmin,2))<(pow(xYmax-xXmin,2)+pow(yYmax-yXmin,2)))
        {
        o=(yYmax-yXmin);
        p=(xYmax-xXmin);
        tx=o/p;
        kat=atan(tx);
        kat=180 - kat*180/3.1415927;
        }
else
        {
        o=yYmax-yXmax;
        p=xXmax-xYmax;
        tx=o/p;
        kat=atan(tx);
        kat=kat*180/3.1415927;
        }
float fkat=kat;
a  ="Kat: ";
a +=FloatToStr(fkat).c_str();
memo->Lines->Add(a.c_str());
}

#endif
