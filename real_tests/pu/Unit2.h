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
class OBRAZ
{
public:
        string path;

        int szer,wys;
        int **tablica;
        int obiekt,licz;
        int *pola_ob,*obwody_ob;
        float *srodki_x,*srodki_y,*momenty_bezw;


        int wypisz(int x,TMemo *memo);
        int wczytaj_obraz();
        int wyswietl(TImage *image);
        int wyswietl_org(TImage *image);
        int binaryzacja(int treshold);
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
/*
void rlft3(Doub *data, Doub *speq, const Int isign,
	const Int nn1, const Int nn2, const Int nn3) {
	Int i1,i2,i3,j1,j2,j3,k1,k2,k3,k4;
	Doub theta,wi,wpi,wpr,wr,wtemp;
	Doub c1,c2,h1r,h1i,h2r,h2i;
	VecInt nn(3);
	VecDoubp spq(nn1);
	for (i1=0;i1<nn1;i1++) spq[i1] = speq + 2*nn2*i1;
	c1 = 0.5;
	c2 = -0.5*isign;
	theta = isign*(6.28318530717959/nn3);
	wtemp = sin(0.5*theta);
	wpr = -2.0*wtemp*wtemp;
	wpi = sin(theta);
	nn[0] = nn1;
	nn[1] = nn2;
	nn[2] = nn3 >> 1;
	if (isign == 1) {
		fourn(data,nn,isign);
		k1=0;
		for (i1=0;i1<nn1;i1++)
			for (i2=0,j2=0;i2<nn2;i2++,k1+=nn3) {
				spq[i1][j2++]=data[k1];
				spq[i1][j2++]=data[k1+1];
			}
	}
	for (i1=0;i1<nn1;i1++) {
		j1=(i1 != 0 ? nn1-i1 : 0);
		wr=1.0;
		wi=0.0;
		for (i3=0;i3<=(nn3>>1);i3+=2) {
			k1=i1*nn2*nn3;
			k3=j1*nn2*nn3;
			for (i2=0;i2<nn2;i2++,k1+=nn3) {
				if (i3 == 0) {
					j2=(i2 != 0 ? ((nn2-i2)<<1) : 0);
					h1r=c1*(data[k1]+spq[j1][j2]);
					h1i=c1*(data[k1+1]-spq[j1][j2+1]);
					h2i=c2*(data[k1]-spq[j1][j2]);
					h2r= -c2*(data[k1+1]+spq[j1][j2+1]);
					data[k1]=h1r+h2r;
					data[k1+1]=h1i+h2i;
					spq[j1][j2]=h1r-h2r;
					spq[j1][j2+1]=h2i-h1i;
				} else {
					j2=(i2 != 0 ? nn2-i2 : 0);
					j3=nn3-i3;
					k2=k1+i3;
					k4=k3+j2*nn3+j3;
					h1r=c1*(data[k2]+data[k4]);
					h1i=c1*(data[k2+1]-data[k4+1]);
					h2i=c2*(data[k2]-data[k4]);
					h2r= -c2*(data[k2+1]+data[k4+1]);
					data[k2]=h1r+wr*h2r-wi*h2i;
					data[k2+1]=h1i+wr*h2i+wi*h2r;
					data[k4]=h1r-wr*h2r+wi*h2i;
					data[k4+1]= -h1i+wr*h2i+wi*h2r;
				}
			}
			wr=(wtemp=wr)*wpr-wi*wpi+wr;
			wi=wi*wpr+wtemp*wpi+wi;
		}
	}
	if (isign == -1) fourn(data,nn,isign);
}
*/
//------------------------------------------------------------------------------
OBRAZ::wypisz(int x,TMemo *memo){
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
OBRAZ::wczytaj_obraz()
        {
        int *ptr;
        Graphics::TBitmap *Rysunek = new Graphics::TBitmap;
        Rysunek->PixelFormat = pf32bit;
        Rysunek->LoadFromFile(path.c_str());
        szer = Rysunek->Width;
        wys = Rysunek->Height;
        tablica = new int *[szer];
        for(int i = 0; i < szer; i++)
                {
                tablica[i] = new int [wys];
                }

        for(int j=0;j<wys;j++)
        {
        ptr = (int *)Rysunek->ScanLine[j];
        for (int i = 0; i < szer; i++)tablica[i][j]=GetRValue(ptr[i]);
        }
        delete Rysunek;
        }
//------------------------------------------------------------------------------
OBRAZ::wyswietl(TImage* image)
        {
        int **tablica1;
        tablica1 = new int *[szer];
        for(int i = 0; i < szer; i++)
                {
                tablica1[i] = new int [wys];
                }
        for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)tablica1[i][j]=tablica[i][j];
        for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)if(tablica1[i][j])tablica1[i][j]=255;

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
                                                int a=tablica1[i][j];
                                                ptr[i]=RGB(a,a,a);
                                                }
                        }
        image->Canvas->StretchDraw(Rect,Rysunek);
        image->Visible = true;
        for(int i = 0; i < szer; i++)
                {
                delete [] tablica1[i];
                }
                delete []tablica1;
                delete Rysunek;

        return 0;
        }
//------------------------------------------------------------------------------
OBRAZ::wyswietl_org(TImage* image)
        {
        int **tablica1;
        tablica1 = new int *[szer];
        for(int i = 0; i < szer; i++)
                {
                tablica1[i] = new int [wys];
                }
        for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)tablica1[i][j]=tablica[i][j];
        
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
                                                int a=tablica1[i][j];
                                                ptr[i]=RGB(a,a,a);
                                                }
                        }
        image->Canvas->StretchDraw(Rect,Rysunek);
        image->Visible = true;
        for(int i = 0; i < szer; i++)
                {
                delete [] tablica1[i];
                }
                delete []tablica1;
                delete Rysunek;

        return 0;
        }
//------------------------------------------------------------------------------
OBRAZ::binaryzacja(int treshold)
        {
        for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)
                {
                if(tablica[i][j]<treshold)tablica[i][j]=0;
                        else tablica[i][j]=255;
                }

        }
//------------------------------------------------------------------------------
OBRAZ::binaryzacja_niblack(float k,float R,float R1,int grid)    //u¿ywa algorytmu Niblack'a
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
                                                avg[x-1][y-1]+=tablica[j][k];
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
                                                std_dev[x-1][y-1]+=pow((tablica[j][k]-avg[x-1][y-1]),2);
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
                                                if(tablica[j][k]<tres)tablica[j][k]=0;
                                                        else tablica[j][k]=255;
                                                }
                                        }
                                }
                        }
                for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)if(tablica[i][j]==0)tablica[i][j]=255;
                                                                        else tablica[i][j]=0;
                for(int i=0;i<x1;i++)delete [] avg[i];
                delete [] avg;
                for(int i = 0; i < x1; i++)delete [] std_dev[i];
                delete [] std_dev;
                }
//------------------------------------------------------------------------------
OBRAZ::segmentacja()
        {
        int n=1;
        for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)
                                if(tablica[i][j])
                                        {
                                         tablica[i][j]=n;
                                         n++;
                                        }


        int min;
        int ile=70;
        for(int a=0;a<ile;a++){
        for(int i=1;i<szer-1;i++)for(int j=1;j<wys-1;j++)
                                {
                                if(tablica[i][j]){
                                                min=szer*wys;
                                                for(int k=i-1;k<i+2;k++)for(int l=j-1;l<j+2;l++)if(tablica[k][l]<min && tablica[k][l])min=tablica[k][l];
                                                tablica[i][j]=min;
                                                }
                                }
                                }
        for(int a=0;a<ile;a++){
        for(int i=szer-2;i>1;i--)for(int j=wys-2;j>1;j--)
                                if(tablica[i][j]){
                                min=szer*wys;
                                for(int k=i-1;k<i+2;k++)for(int l=j-1;l<j+2;l++)if(tablica[k][l]<min && tablica[k][l])min=tablica[k][l];
                                tablica[i][j]=min;
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
                                                        if(tablica[i][j]){
                                                                        for(int k=1;k<licz;k++)if(obiekty[k]==tablica[i][j])czy=false;
                                                                        if(czy){
                                                                                obiekty[licz]=tablica[i][j];
                                                                                licz++;
                                                                                }
                                                                        }
                                                        }
        for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)
                                                if(tablica[i][j])
                                                for(int k=1;k<licz;k++)if(tablica[i][j]==obiekty[k]){
                                                                                                     tablica[i][j]=k;
                                                                                                     break;
                                                                                                    }
        delete[] obiekty;

        pola_ob = new int[licz];
        for(int i=0;i<licz;i++)pola_ob[i]=0;
        for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)pola_ob[tablica[i][j]]++;
        }
//------------------------------------------------------------------------------
OBRAZ::policz_obwody()
{
obwody_ob = new int[licz];
for(int i=0;i<licz;i++)obwody_ob[i]=0;
bool czy;
for(int i=1;i<szer-1;i++)for(int j=1;j<wys-1;j++)
        {
        czy =false;
        if(tablica[i][j])for(int k=i-1;k<i+2;k++)for(int l=j-1;l<j+2;l++)if(tablica[k][l]==0)czy=true;
        if(czy)obwody_ob[tablica[i][j]]++;
        }
}
//------------------------------------------------------------------------------
OBRAZ::obcinaj_obiekty_tla(int tres_min_pole,int tres_max_pole,int tres_min_obw,int tres_max_obw)
{
for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)if(pola_ob[tablica[i][j]]<tres_min_pole || pola_ob[tablica[i][j]]>tres_max_pole)
                                                                                                                {
                                                                                                                pola_ob[tablica[i][j]]=0;
                                                                                                                obwody_ob[tablica[i][j]]=0;
                                                                                                                tablica[i][j]=0;
                                                                                                                }
for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)if(obwody_ob[tablica[i][j]]<tres_min_obw || obwody_ob[tablica[i][j]]>tres_max_obw)
                {
                pola_ob[tablica[i][j]]=0;
                obwody_ob[tablica[i][j]]=0;
                tablica[i][j]=0;
                }
obiekt=0;
for(int i=1;i<licz;i++)if(pola_ob[i])obiekt++;
}
//------------------------------------------------------------------------------
OBRAZ::brzegi()
{
int lewy_bok=30,gora=50,dol=145,cypel=190;
for(int i=0;i<lewy_bok;i++)for(int j=0;j<wys;j++)
                {
                pola_ob[tablica[i][j]]=0;
                obwody_ob[tablica[i][j]]=0;
                tablica[i][j]=0;
                }
for(int i=0;i<szer;i++)for(int j=0;j<gora;j++)
                {
                pola_ob[tablica[i][j]]=0;
                obwody_ob[tablica[i][j]]=0;
                tablica[i][j]=0;
                }
for(int i=0;i<szer;i++)for(int j=wys-dol;j<wys;j++)
                {
                pola_ob[tablica[i][j]]=0;
                obwody_ob[tablica[i][j]]=0;
                tablica[i][j]=0;
                }
for(int i=szer/2-70;i<szer/2+130;i++)for(int j=wys-cypel;j<wys;j++)
                {
                pola_ob[tablica[i][j]]=0;
                obwody_ob[tablica[i][j]]=0;
                tablica[i][j]=0;
                }
obiekt=0;
for(int i=1;i<licz;i++)if(pola_ob[i])obiekt++;
}
//------------------------------------------------------------------------------
OBRAZ::policz_srodek()
{
srodki_x = new float[licz];
srodki_y = new float[licz];
for(int i=0;i<licz;i++)srodki_x[i]=0;
for(int i=0;i<licz;i++)srodki_y[i]=0;
for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)if(tablica[i][j])
        {
         srodki_x[tablica[i][j]]+=i;
         srodki_y[tablica[i][j]]+=j;
        }
for(int i=0;i<licz;i++)if(pola_ob[i])srodki_x[i]=srodki_x[i]/pola_ob[i];
for(int i=0;i<licz;i++)if(pola_ob[i])srodki_y[i]=srodki_y[i]/pola_ob[i];
}
//------------------------------------------------------------------------------
OBRAZ::policz_moment_b()
{
momenty_bezw = new float[licz];
for(int i=0;i<licz;i++)momenty_bezw[i]=0;
for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)if(tablica[i][j])
        {
        momenty_bezw[tablica[i][j]]+=pow(i-srodki_x[tablica[i][j]],2)+pow(j-srodki_y[tablica[i][j]],2);
        }
}
//------------------------------------------------------------------------------
OBRAZ::element(int ktory,TImage *image,TMemo *memo)
{
wyswietl(image);
memo->Text = "";
int j=0,x;
for(x=0;x<licz;x++)
{
if(pola_ob[x])j++;
if(j==ktory)break;
}
        int **tablica1;
        tablica1 = new int *[szer];
        for(int i = 0; i < szer; i++)
                {
                tablica1[i] = new int [wys];
                }
for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)tablica1[i][j]=tablica[i][j];
for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)if(tablica1[i][j]!=x)tablica1[i][j]=0;
for(int i=0;i<szer;i++)for(int j=0;j<wys;j++)if(tablica1[i][j])tablica1[i][j]=255;
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
if(pola_ob[x]>7145/*7149*/ && pola_ob[x]<7795/*7725*/)
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
        memo->Lines->Add("????");
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
                delete [] tablica1[i];
                }
                delete [] tablica1;
}
//------------------------------------------------------------------------------
OBRAZ::policz_kat(int ktory,TMemo *memo)
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
        if(tablica[i][j]==x)
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
