/*
 * pokerops.h
 *
 *  Created on: Dec 30, 2014
 *      Author: sojourner
 */

#include <iostream>
#include <math.h>
#include <time.h>
#include <cstdlib>
//#include <basicUtilities.h>

using namespace std;

#ifndef POKEROPS_H
#define POKEROPS_H

#endif // POKEROPS_H

#define CARDSEEDA 2
#define CARDSEEDB 6
#define DECKSIZE 52
#define MAXHAND 5
#define DECKARRAYRANGE 6
#define SUITCOUNT 4
#define CARDCOUNT 13
#define GENERIC_LEN 64

char DECKRANK[DECKSIZE][GENERIC_LEN];
char SUITS[SUITCOUNT][10]={"HEARTS","DIAMONDS","CLUBS","SPADES"};
char CARDNAME[CARDCOUNT][10]={"TWO","THREE","FOUR","FIVE","SIX","SEVEN","EIGHT","NINE","TEN","JACK","QUEEN","KING","ACE"};
char NAMEDDECK[DECKSIZE][40];
char SHUFFLEDDECK[DECKSIZE][40];
int SHUFFLEDINDEX[DECKSIZE];
int SHUFFLEDINDEXDCR[DECKSIZE];
long long int CARDDECK[DECKSIZE][DECKARRAYRANGE];
long long int CARDDECKCRYP[DECKSIZE][DECKARRAYRANGE];
long long int CARDDECKSHFL[DECKSIZE][DECKARRAYRANGE];
long long int CARDINHAND[DECKSIZE][DECKARRAYRANGE];
int REF[2]={1,2};
int PRIMEOFFSET[8]={1,7,11,13,17,19,23,29};
//int CARDDECK[4][13]; // remove later ?
//int DECKSIZE=52;
long long int CARDMAIN[52];
long long int CARDTEMP[52];
char* CARDSTRM = (char*)calloc(MAXBUF, sizeof(char));
char* CARDSINHAND = (char*)calloc(MAXBUF, sizeof(char));
char* CARDSOTHERHAND = (char*)calloc(MAXBUF, sizeof(char));
char* CARDSSELFHAND = (char*)calloc(MAXBUF, sizeof(char));
int* REMOTECARDS = (int*)calloc(MAXHAND, sizeof(int));
int* SELFCARDS = (int*)calloc(MAXHAND, sizeof(int));
int PRG;
long long int N, KEY1,KEY2, TOTATIVES=0;
long long int TIME0;



long long int getRemainder(long long int a, long long int b)
{
        if(a<b) return a;
        do
        {
                a-=b;
        }while(a>=b);
        return a;
}

void establishrank()
{
    int count=0;
    for(int i=0; i<SUITCOUNT; i++)
    {
        for(int j=0; j<CARDCOUNT; j++)
        {
            strcat(DECKRANK[count], SUITS[i]); strcat(DECKRANK[count], " "); strcat(DECKRANK[count++], CARDNAME[j]);
        }
    }
}

int getrank(char str[])
{
    for(int i=0; i<DECKSIZE; i++)
    {
        if(strcmp(str, DECKRANK[i])==0)
        {
            int rem = getRemainder(i+1, CARDCOUNT);
            if(rem == 0) return CARDCOUNT; else return rem;
        }
    }
    return -1;
}

void arrangecardbyname()
{
    int count=0;
    for(int i=0; i<SUITCOUNT; i++)
    {
        for(int j=0; j<CARDCOUNT; j++)
        {
            strcat(NAMEDDECK[count], SUITS[i]); strcat(NAMEDDECK[count], " "); strcat(NAMEDDECK[count++], CARDNAME[j]);
        }
    }

}


long long int sqrootint(long long int n) //--------------------------------------------------------
{
        return int(sqrt(double(n)));

} //---------------------------------------------------------------------------------

//long long int getRemainder(long long int a, long long int b) //-------------------------------------------
//{
//        long long int x=a;
//        for(int i=0;a>=b;i++) a-=b;
//        return a;
//} //---------------------------------------------------------------------------------

long long int getExponential(long long int e, long long int x) //--------------------------
{
        if(e==1) return 1;
        long long int exponent=1;
        for(long long int i=0;i<x;i++) exponent*=e;
        return exponent;
} //---------------------------------------------------------------------------------

//int checkPrime(long long int d) //----------------------------------------------------------
//{
//        int iterator=0, k=0;
//        if(d==1) return 0;
//        if(d==2 || d==3 || d==5) return 1;
//        if(getRemainder(d, 2)==0 || getRemainder(d, 3)==0 || getRemainder(d, 5)==0) return 0;
//        for(long long int i=1;i<=sqrootint(d);i++)
//        {
//                if(iterator>7){ iterator=0; k++;}
//                if(k==0 && iterator==0)continue;
//                if(fmod(d,((30*k)+PRIMEOFFSET[iterator++]))==0.0)return 0;
//        }
//        return 1;
//} //----------------------------------------------------------------------------------

int checkPrime(long long int n)
{
            if(n==2 || n==3 || n==5) return 1;
            for(long long int i=2;i<n/*sqrootint(n)*/;i++) if(getRemainder(n,i) == 0) return 0;
            return 1;
}

int long euclidGCD(long long int a, long long int b) //----------------------------------------------------
{
        //cout<<endl<<a<<":"<<b;
        long long int result;
        if(a==0 && b==0) return -1;
        if(b==0){return a;}
        else
        {
                //long int remainder=fmod(a,b);
                long long int remainder=getRemainder(a,b);
                result=(b>remainder)? euclidGCD(b,remainder): euclidGCD(remainder,b);
        }
        return result;
} //---------------------------------------------------------------------------------------

void /*long long int*/ eulerPhi(long long int d) //----------------------------------------------------------
{
        //long long int totatives=0;
        if(checkPrime(d)==1) TOTATIVES = d-1; /*return d-1;*/
        else
        {
            for(long long int i=1;i<d;i++)
            {
                  if(euclidGCD(d, i)==1) TOTATIVES++;
            }
        }
        cout<<endl<<"\t\tEuler Phi("<<d<<"): "<<TOTATIVES;
        //return TOTATIVES;
} //----------------------------------------------------------------------------------



bool verifyKey(long long int k, long long int n) //-------------------------------------------------
{
        return (euclidGCD(k,TOTATIVES/*eulerPhi(n)*/)==1)?true:false;
} //----------------------------------------------------------------------------------



long long int encryptdecrypt(long long m, long long int exp) //-------------------------------------------------------------
{
        long long int res=1;
        long long int rem = getRemainder(m, N);
        for(int i=1;i<=exp;i++)
        {
                res*=rem;
                //cout<<endl<<"Remainder multiplied: "<<res;
        }
        return getRemainder(res, N);
/*
        cout<<endl<<"Encrypt ......."; // ---- remove later
        for(int i=0;i<DECKSIZE;i++) CARDTEMP[i]=getRemainder(getExponential(CARDTEMP[i],key), N);
*/
} //---------------------------------------------------------------------------------

void encryptDeck(long long int key) //-------------------------------------------------------------
{
        //cout<<endl<<"Encrypt ......."; // ---- remove later
        //for(int i=0;i<DECKSIZE;i++) CARDTEMP[i]=encryptdecrypt(CARDTEMP[i],key);//getRemainder(getExponential(CARDTEMP[i],key), N);
        for(int i=0;i<DECKSIZE;i++)
        {
            for(int j=0; j<DECKARRAYRANGE; j++) CARDDECKCRYP[i][j] = encryptdecrypt(CARDDECK[i][j],key);
        }
        //CARDTEMP[i]=encryptdecrypt(CARDTEMP[i],key);
} //---------------------------------------------------------------------------------

long long int findinverse(long long int a)
{
    //long long int totatives = eulerPhi(N);
    for(long long int i=1;i<TOTATIVES/*Ntotatives*/;i++)
    {
        //cout<<endl; // ------ remove later

        //int prod = a*i;
        //cout<<endl<<a<<"*"<<i<<" (mod "<<p<<"): "<<getRemainder(prod, p);
        if(getRemainder(a*i, TOTATIVES/*totatives*/) == 1) { return i;}
        //if(((a*i)%totatives) == 1) { cout<< endl<<"Returning: ++++++"; return i;}
        //cout<<"rem("<<a<<","<<i<<"="<<a*i<<","<<totatives<<" ; "; // ------ remove later
        //if((a*i)%p == 1) return i;
    }
    return -1;
} //--------------------------------------------------------------------------------------------

/*
long long int decryptcard(long long int c, long long int secretkey)
{
    //long long int exp = findinverse(secretkey);
    long long storeres=1;
    int exptemp;
    //int count=0;
    if(exp>5)
    {
        do
        {
            if(exp>=5) exptemp = 5;
            else exptemp = exp;
            storeres*=getRemainder(getExponential(c, exptemp), N);
            exp-=5;
        }while(exp>0);
        return getRemainder(storeres, N);
    }
    else return getRemainder(getExponential(c, exp), N);
    //return 1;
}
*/


void cardmasterarray() //-----------------------------------
{
    int offset=1;
    do
    {
            //cell=0;
            int gallops = getExponential(2, offset);
            int subgallops = gallops/2;

            for(int i=0;i<DECKSIZE;i+=gallops)
            {
                    int cell=0;
                    int s1 = ((i + subgallops)>DECKSIZE)?DECKSIZE:(i + subgallops);
                    int s2 = ((i + gallops)>52)?52:(i + gallops); //i + gallops;
                    for(int j = i ; j < s1 ; j++)   CARDDECK[j][DECKARRAYRANGE-offset] = REF[cell];
                    cell++;
                    for(int j = s1 ; j < s2 ; j++)  CARDDECK[j][DECKARRAYRANGE-offset] = REF[cell];

            }
            offset++;
    }while(offset<=DECKARRAYRANGE);
    //for(int i=0; i<52; i++) {cout<<endl<<"a["<<i<<"]: "<<A[i][0]<<A[i][1]<<A[i][2]<<A[i][3]<<A[i][4]<<A[i][5];}
    //cout<<endl<<"Card 52: "<<A[51][0];
} // -----------------------------------------------------------

void notateDeck() //-----------------------------------------------------------------
{
        arrangecardbyname();
        srand(time(0));
        PRG=rand()%CARDSEEDA+CARDSEEDB; // --- uncomment later
        //PRG=0; // --- remove later
        for(int i=0;i<DECKSIZE;i++) CARDTEMP[i]=PRG+i;
        cardmasterarray();

} //----------------------------------------------------------------------------------

void shuffleDeck() //-----------------------------------------------------------------
{
        //cout<<endl<<"Shuffle ......."; // ---- remove later
        int /*prg=0,*/ sum, randnum;
        //cout<<endl;
        for(int i=0;i<DECKSIZE;i++)
        {
                if(DECKSIZE-i-1==0)
                {
                    CARDMAIN[i]=CARDTEMP[0];
                    strcpy(SHUFFLEDDECK[i], NAMEDDECK[0]);
                    for(int k=0; k<DECKARRAYRANGE; k++) CARDDECKSHFL[i][k]=CARDDECKCRYP[0][k];
                }
                else
                {
                        int sum=0;
                        randnum = rand()%(DECKSIZE-i-1);

                        strcpy(SHUFFLEDDECK[i], NAMEDDECK[randnum]);

                        CARDMAIN[i]=CARDTEMP[randnum];
                        for(int j=0 ; j<DECKARRAYRANGE; j++)
                        {
                            CARDDECKSHFL[i][j] = CARDDECKCRYP[randnum][j];
                            sum+= CARDDECKSHFL[i][j] * getExponential(10, DECKARRAYRANGE-j-1);
                        }
                        SHUFFLEDINDEX[i] = sum;

                        for(int j=randnum;j<DECKSIZE-1/*DECKSIZE-i-1*/;j++)
                        { CARDTEMP[j]=CARDTEMP[j+1]; strcpy(NAMEDDECK[j], NAMEDDECK[j+1]); }
                        for(int j=randnum;j<DECKSIZE-1/*DECKSIZE-i-1*/;j++)
                        {
                            for(int l = 0 ; l < DECKARRAYRANGE ; l++)
                            CARDDECKCRYP[j][l]=CARDDECKCRYP[j+1][l];
                        }
                }
        }
        for(int i=PRG;i<PRG+DECKSIZE;i++)
        {
                sum=0;
                for(int j=0;j<DECKSIZE;j++)if(i==CARDMAIN[j]) sum++;
                if(sum>1) cout<<endl<<"Duplicate: "<<i;
        }

} //----------------------------------------------------------------------------------
