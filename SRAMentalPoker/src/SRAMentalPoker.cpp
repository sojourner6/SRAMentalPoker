//============================================================================
// Name        : SRAMentalPoker.cpp
// Author      : Somnath Sarkar
// Version     :
// Copyright   : Public access
// Description : Hello World in C++, Ansi-style
//============================================================================

//#include <iostream>
//using namespace std;
//
//int main() {
//	cout << "SRA Mental Poker implementation" << endl; // prints SRA Mental Poker implementation
//	return 0;
//}
//import clientserver.h;

#include <iostream>
#include <cstdlib>
#include "clientserver.h"
#include "pokerops.h"
#include "basicutilities.h"

using namespace std;

char CARDSRCVD[DECKSIZE][STR_LEN];
char MYCARDS[MAXHAND][STR_LEN];
int MYBET;
int MYRANK;
int OPPRANK;


void cardlookupbyindexserver(int x, int inx)
{
    for(int i=0; i<DECKSIZE; i++)
    {
        int y = SHUFFLEDINDEXDCR[i];// cout<<y<<", ";
        if(x==y){ strcpy(MYCARDS[inx], SHUFFLEDDECK[i]); /*cout<<endl<<SHUFFLEDDECK[i];*/ break; }
    }
}

void cardlookupbyindexclient(int x, int inx)
{
    for(int i=0; i<DECKSIZE; i++)
    {
        int y = SHUFFLEDINDEX[i];
        if(x==y){ strcpy(MYCARDS[inx], SHUFFLEDDECK[i]); /*cout<<endl<<SHUFFLEDDECK[i];*/ break; }
    }
}


long long int returnprimes(long long int a, long long int b)
{
    long long int i;
    for(i=a;a<=b;i++) if(checkPrime(i) == 1) break;
    return i;
}

int boundedintinput(int a, int b, char* prompt)
{
    char c[10];
    int input;

    do
    {
        cout<<endl<<prompt;
        cin>>c;
        input = convertStrNumToNum(c);
        //input = c;
    }while(input<a || input>b);
    return input;
}

//-------------------------------------------------------------------------
void boundedstrprime(char* rcvdmem, int a, int b, char *prompt)
{
    char c[10];
    int input, isprime;

    do
    {
        cout<<endl<<prompt;
        cin>>c;
        input = convertStrNumToNum(c);
        isprime = checkPrime(input);
        //input = c;
    }while(isprime !=1 || input<a || input>b);
    strcat(rcvdmem, c);
}//-------------------------------------------------------------------------

// slows down communication to enable send/receive in order
void backmarker()
{
    int c;
    /*
       ******* cin allows the Remote Player *******
       ****** to have enough time to host a  ******
       *********** server at his end **************
    */
    c = boundedintinput(1, 2, "\t\tTo Continue, hit (1/2)");
}//-------------------------------------------------------------------------

// choosing private keys by each member
int getkey()
{
    //return boundedintinput(5, 15, "\t\tPlease Select your Secret Key in the range (5-15): ");
    int n;
    char* str = (char*)calloc(MAXBUF, sizeof(char));
    char* cnum =
            (char*)calloc(STR_LEN, sizeof(char));
    for(int i=2;i<=TOTATIVES/*N*/;i++)
    {
        if(verifyKey(i, N)){ convertNumToChar(cnum, i); strcat(str, cnum); strcat(str, ", "); }
    }
    cout<<endl<<"\t\tFor best cryptographic implementation, choose from among: "<<str;
    cout<<endl<<"\t\tEnter your Choice: ";
    cin>>n;
    return n;
}//-------------------------------------------------------------------------

//intermediate card notation
void formdeckstring(char *rcvdmem)
{
    //cout<<endl<<"Form Deck String ......."; // ---- remove later
    //CARDSTRM = (char*)calloc(MAXBUF, sizeof(char));
    char* temp = (char*)calloc(STR_LEN, sizeof(char));

    for(int i=0;i<DECKSIZE;i++)
    {
        // ++++++++++++++++++++
        for(int j=0; j<DECKARRAYRANGE; j++)
        {
            convertNumToChar(temp, CARDDECKSHFL[i][j]);
            strcat(rcvdmem, temp);
            strcat(rcvdmem, ",");
        }
        strcat(rcvdmem, "|");
    }
}//-------------------------------------------------------------------------


void formcardsrcvd()
{
    int j=0, count=0;
    char* temp = (char*)calloc(STR_LEN, sizeof(char));
    for(int i=0;BUFFER[i]!='\0';i++)
    {
        if(BUFFER[i]=='|'){ temp[j]='\0'; j=0; strcpy(CARDSRCVD[count++], temp); }
        else temp[j++]=BUFFER[i];
    }
}

void decryptcardindex(int key)
{
    int d /*n,[DECKARRAYRANGE]*/;//, inx=0;
    for(int i=0; i<DECKSIZE; i++)
    {
        int inx=DECKARRAYRANGE-1, sum=0, count=0;
        d=SHUFFLEDINDEX[i];
        do
        {
            int rem = getRemainder(d, 10);
            sum+=encryptdecrypt(rem, key)*getExponential(10, count);
            //n[inx] = encryptdecrypt(rem, key);
            d/=10;
            inx--; count++;
        }while(d>0);
        SHUFFLEDINDEXDCR[i] = sum;
    }
}


void formmycards(int vkey)
{
    int j=0, count=0, mergecount=0;
    long long int mergenum[DECKARRAYRANGE];
    char* temp = (char*)calloc(STR_LEN, sizeof(char));
    //cout<<endl;
    for(int i=0;BUFFER[i]!='\0';i++)
    {
        //cout<<BUFFER[i];
        if(BUFFER[i]=='|')
        {
            int sum=0; //cout<<endl;
            for(int k=0; k<DECKARRAYRANGE; k++)
            {
                CARDINHAND[count][k] = encryptdecrypt(mergenum[k], vkey);
                //cout<<CARDINHAND[count][k];

                //int num = convertStrNumToNum(CARDINHAND[count][k]);
                sum+= CARDINHAND[count][k] * getExponential(10, DECKARRAYRANGE - k -1);

            }
            cardlookupbyindexserver(sum, count++);
            mergecount=0;
            //cout<<sum;

        }
        else
        {
            if(BUFFER[i]==',')
            {
                temp[j]='\0'; j=0;
                mergenum[mergecount++] = convertStrNumToNum(temp);
            }
            else temp[j++]=BUFFER[i];
        }
    }
}


void deducercvdcards(char* rcvdmem)
{
    //cout<<endl<<"RCVDMEM at deducercvdcards: "<<rcvdmem;
    int j=0, count=0, mergecount=0;
    long long int mergenum[DECKARRAYRANGE];
    char* temp = (char*)calloc(STR_LEN, sizeof(char));
    for(int i=0;rcvdmem[i]!='\0';i++)
    {
        if(rcvdmem[i]=='|')
        {
            int sum=0; //cout<<endl;
            for(int k=0; k<DECKARRAYRANGE; k++)
            {
                //CARDINHAND[count][k] = encryptdecrypt(mergenum[k], vkey);
                //cout<<CARDINHAND[count][k];

                //int num = convertStrNumToNum(CARDINHAND[count][k]);
                sum+= mergenum[k] * getExponential(10, DECKARRAYRANGE - k -1);

            }
            //cout<<endl<<"\t\tSum card code = "<<sum;
            cardlookupbyindexclient(sum, count++);
            //SHUFFLEDINDEXDCR[cardcount++]=sum;
            mergecount=0;
            //cout<<sum;
            //cardlookupbyindex(sum);
        }
        else
        {
            if(rcvdmem[i]==',')
            {
                temp[j]='\0'; j=0;
                mergenum[mergecount++] = convertStrNumToNum(temp);
            }
            else temp[j++]=rcvdmem[i];
        }
    }
}




void decryptremotecards(char* rcvdmem, int key)
{
    int j=0, count=0, mergecount=0;
    long long int cards[MAXHAND][DECKARRAYRANGE], mergenum[DECKARRAYRANGE];
    char* temp = (char*)calloc(STR_LEN, sizeof(char));
    for(int i=0;BUFFER[i]!='\0';i++)
    {
        if(BUFFER[i]=='|')
        {
            //cout<<endl;
            for(int k=0; k<DECKARRAYRANGE; k++)
            {
                cards[count][k] = encryptdecrypt(mergenum[k], key);
                char* numchar = (char*)calloc(STR_LEN, sizeof(char));
                convertNumToChar(numchar, cards[count][k]);
                strcat(rcvdmem, numchar); strcat(rcvdmem, ",");
                //cout<<cards[count][k];
            }
            strcat(rcvdmem, "|");
            mergecount=0; count++;  //cout<<endl;
        }
        else
        {
            if(BUFFER[i]==',')
            {
                temp[j]='\0'; j=0;
                mergenum[mergecount++] = convertStrNumToNum(temp);
                //cout<<mergenum[mergecount-1];

            }
            else temp[j++]=BUFFER[i];
        }
    }
}


void selectremotecards()
{
    int input, duplicates=0;
    //char* str = (char*)calloc(STR_LEN, sizeof(char));
    cout<<endl<<"\t\tSelect 5 cards out of 52 for remote player: ";
    for(int i=0;i<MAXHAND;i++)
    {
        do
        {
            duplicates=0;
            input=boundedintinput(1, 52, "\t\tEnter Card number (1-52): ");
            for(int j=0;j<MAXHAND;j++)
            {
                if(input == REMOTECARDS[j])
                { cout<<endl<<"Dulpicate Entry! "<<duplicates++;
                  break;
                }
            }
        }while(duplicates!=0);
        REMOTECARDS[i]=input;
        cout<<endl<<"\t\t"<<MAXHAND-i-1<<" more Cards need to be chosen";
    }
}

void formremotecards()
{
    for(int i=0;i<MAXHAND;i++)
    {
        strcat(CARDSOTHERHAND, CARDSRCVD[(REMOTECARDS[i]-1)]);
        strcat(CARDSOTHERHAND, "|");
    }

}


void selectselfcards()
{
    int input, duplicates=0;
    //char* str = (char*)calloc(STR_LEN, sizeof(char));
    cout<<endl<<"\t\tSelect 5 cards out of the remaining 47 for yourself: ";
    for(int i=0;i<MAXHAND;i++)
    {
        do
        {
            duplicates=0;
            input=boundedintinput(1, 52, "\t\tEnter Card number (1-52): ");
            for(int j=0;j<MAXHAND;j++)
            {
                if(input == REMOTECARDS[j])
                { cout<<endl<<"Card already given to other player "<<duplicates++;
                  break;
                }
                if(input == SELFCARDS[j])
                { cout<<endl<<"Duplicate Selection of cards. "<<duplicates++;
                  break;
                }
            }
        }while(duplicates!=0);
        SELFCARDS[i]=input;
        cout<<endl<<"\t\t"<<MAXHAND-i-1<<" more Cards need to be chosen";
    }
}

void encryptselfcards(long long int k)
{
    for(int i=0;i<MAXHAND;i++)
    {
        strcat(CARDSSELFHAND, CARDSRCVD[(SELFCARDS[i]-1)]);
        strcat(CARDSSELFHAND, "|");
    }
    strcpy(BUFFER, CARDSSELFHAND);
    //cout<<endl<<"\t\tSelf Buffer: "<<BUFFER<<"^";
    formmycards(k);
}

long long int deduceopponentcard()
{
    int i;
    char* tempnum = (char*)calloc(STR_LEN, sizeof(char));
    for(i=0; BUFFER[i]!='\0'; i++)
    {   tempnum[i] = BUFFER[i];     }
    tempnum[i]='\0';
    return convertStrNumToNum(tempnum);

}

void gameHost()
{
    establishrank();
    char* cardstrm = (char*)calloc(MAXBUF, sizeof(char));
    int tempint;
    char *c;
    char* str = (char*)calloc(MAXBUF, sizeof(char));
    long long int secretkey, inversekey; long long int prime;
    actserver();

    backmarker();
    //getchar();
    actclient(OWNIP);
    //backmarker();
    //***************************************************************
    //*********** Choosing the common Prime Number ******************
    //***************************************************************
    //strcat(str, "Please choose any one of the 3 numbers for security purpose: ");
    strcat(str, "Please choose a number between 5 and 40 for security purpose: ");

            //+++++++++++++++++++++++++++++++++
    //long long commkey = returnprimes(7,23);
    //cout<<endl<<"Returned Prime: "<<commkey;
    //convertNumToChar(str, commkey);
    backmarker();
    actclient(str);
    actserver();
    cout<<endl<<"\t\tFinal Decision on Prime Number: enter the number (same as above?): ";
    cin>>prime;
    tempint=boundedintinput(1, 1, "\t\tSure? (hit 1): ");
    N=prime;
    eulerPhi(N);

    //***************************************************************
    //*************** Choosing the Secret Key ***********************
    //***************************************************************
    cout<<endl<<"\t\tNow you need to select a Secret Key for yourself. The Computer will help you in this";
    secretkey = getkey();
    inversekey = findinverse(secretkey);
    cout<<endl<<"\t\tYour Decryption key: "<<inversekey;
    if(inversekey == -1){ cout<<"Invalid key selection. Exiting..."; exit(1);}
    c="\t\tPlease Select your secret key: ";
    actclient(c);

    cout<<endl<<"\t\tPLAYER"<<ORDER<<": Creating a fresh Deck of Cards ..."<<endl;
    notateDeck();
    cout<<endl<<"\t\tFresh Deck of Cards Ready!"<<endl;

    actserver();
    //***************************************************************
    backmarker();
    //secret = getkey();
    //strcpy(str, "I have chosen "); strcpy(str, N);
    c="Thanks! I have chosen my key. Let the Game BEGIN...........";
    actclient(c);
    encryptDeck(secretkey);
    shuffleDeck();
    formdeckstring(CARDSTRM);

    // ----- send shuffled deck here
    decryptcardindex(inversekey);
    memset(str, 0, MAXBUF);
    for(int i=0; i<DECKSIZE; i++)
    {
        char* temp = (char*)calloc(STR_LEN, sizeof(char));
        convertNumToChar(temp, SHUFFLEDINDEXDCR[i]);
        strcat(str, temp); strcat(str, "|");
    }
    backmarker();
    actclient(str);
    // ----- send shuffled named deck
    memset(str, 0, MAXBUF);
    for(int i=0; i<DECKSIZE; i++){ strcat(str, SHUFFLEDDECK[i]); strcat(str, "|"); }
    backmarker();
    actclient(str);

    backmarker();
    actclient(CARDSTRM);

    //***************************************************************
    memset(BUFFER, 0, MAXBUF);
    SUPPRESSDATA=1; SPECIALFLAGS=1; actserver();
    //decryptcardindex(inversekey);  //decrypted the shuffled encrypted cards
    formmycards(inversekey/*secretkey*/);
    //cout<<endl<<"My Card 2: ";//<<MYCARDS[1];
    //for(int i=0; i<DECKARRAYRANGE; i++) cout<<CARDINHAND[1][i];
    memset(BUFFER, 0, MAXBUF);
    SPECIALFLAGS=1; SUPPRESSDATA=1; actserver();

    char* sendcards = (char*)calloc(MAXBUF, sizeof(char));
    decryptremotecards(sendcards, inversekey);
    backmarker();
    actclient(sendcards);
    cout<<endl<<"\t\tMY HAND:-\n\t\t----------------";
    int i;
    for(i=0; i<MAXHAND; i++) cout<<endl<<"\t\t"<<MYCARDS[i]; cout<<endl;
    int tempinx=0;
    do
    {
        MYBET = boundedintinput(1, MAXHAND, "\t\tPlease choose a card from your Hand to place a bet (1-5): ");
        tempinx=boundedintinput(1, 1, "\t\tSure? (hit 1): ");
    }while(tempinx!=1);
    memset(str, 0, MAXBUF);
    strcpy(str, MYCARDS[MYBET-1]);
    backmarker();
    actclient(str);
    SPECIALFLAGS=1; SUPPRESSDATA = 1; actserver();
    char* oppcard = (char*)calloc(STR_LEN, sizeof(char));
    strcpy(oppcard, BUFFER);
    //int oppcard = deduceopponentcard();

    MYRANK = getrank(MYCARDS[MYBET-1]);
    OPPRANK = getrank(oppcard);
    cout<<endl<<"\t\tMy Choice:            "<<MYCARDS[MYBET-1]<<" Rank: "<<MYRANK;
    cout<<endl<<"\t\tMy Opponent's Choice: "<<oppcard<<" Rank: "<<OPPRANK;

    if(MYRANK>OPPRANK)cout<<endl<<"\t\tYOU WIN!!!";
    else
    {
        if(MYRANK == OPPRANK)
        {
            cout<<"\t\tGAME TIED! :| :|";
        }
        else
        {
            cout<<endl<<"\t\tYOU LOSE :P :P :P";
        }
    }
    cout<<endl;
}

void gameConnect()
{
    establishrank();
    char hostip[IP_LEN], *c, charnum[5];
    char* str = (char*)calloc(MAXBUF, sizeof(char));
    c=(char*)calloc(MAXBUF, sizeof(char));
    long long int secretkey, inversekey;

    cout<<endl<<"\t\tPlease enter the host's IPv4 Address: ";
    cin>>hostip;
    strcpy(REMOTEIP,hostip);
    actclient(OWNIP);

    actserver();
    //backmarker();
    actserver();
    cout<<endl<<"\t\tChoose: "; // this can be implemented using boundedinput --- future work
    scanf("%s", charnum);
    //cin>>N;
    N=convertStrNumToNum(charnum);
    eulerPhi(N);
    strcat(str, "I have chosen "); strcat(str, charnum);
    actclient(str);
    actserver();
    //**********************************************************
    //*********** Selecting the Secret Key *********************

    cout<<endl<<"\t\tNow you need to select a Secret Key for yourself. The Computer will help you in this";
    secretkey = getkey();
    inversekey = findinverse(secretkey);
    cout<<endl<<"\t\tYour Decryption key: "<<inversekey;
    if(inversekey == -1){ cout<<"Invalid key selection. Exiting..."; exit(1);}

    c = "I have chosen my Secret Key!";
    backmarker();
    actclient(c);  actserver();
    //**********************************************************
    //************ Selecting Remote Cards from Deck ************
        //++++++++++++++++++++++
    SPECIALFLAGS=1; SUPPRESSDATA=1; actserver();
    char* strtemp = (char*)calloc(STR_LEN, sizeof(char));
    int spclcount=0, spclcardcount=0;
    for(int i = 0; BUFFER[i]!='\0'; i++)
    {
        if(BUFFER[i]=='|')
        {
            strtemp[spclcount]='\0'; spclcount=0;
            SHUFFLEDINDEX[spclcardcount++] = convertStrNumToNum(strtemp);
        }
        else strtemp[spclcount++] = BUFFER[i];

    }

    // ++++++++++++++++++++ remove later
//    cout<<endl<<"------------------------------------------";
//    for(int i=0; i<DECKSIZE; i++)cout<<SHUFFLEDINDEX[i]<<",";
//    cout<<"-------------------------------------------"<<endl;
    // ++++++++++++++++++++
    SUPPRESSDATA=1; SPECIALFLAGS=1;
    actserver();
    spclcount=0, spclcardcount=0;
    for(int i = 0; BUFFER[i]!='\0'; i++)
    {
        if(BUFFER[i]=='|')
        {
            strtemp[spclcount]='\0'; spclcount=0;
            strcpy(SHUFFLEDDECK[spclcardcount++], strtemp);
        }
        else strtemp[spclcount++] = BUFFER[i];
    }
        //++++++++++++++++++++++
    memset(BUFFER, 0, MAXBUF);
    SUPPRESSDATA=1; SPECIALFLAGS = 1; //cout<<endl<<"Last: ";
    actserver();
    formcardsrcvd();/* cout<<endl<<"CARD 6: "<<CARDSRCVD[5];*/
    selectremotecards();
    formremotecards();
    selectselfcards();
    encryptselfcards(secretkey);
    for(int i=0; i<MAXHAND; i++)cout<<endl<<"\t\t"<<MYCARDS[i];
    //cout<<endl;
    //strcat(CARDSOTHERHAND, CARDSINHAND);
    actclient(CARDSOTHERHAND);
    backmarker();
    char* numchar = (char*) calloc(STR_LEN, sizeof(char));
    memset(str, 0, MAXBUF);
    for(int i=0; i<MAXHAND; i++)
    {
        for(int j=0; j<DECKARRAYRANGE; j++)
        {
            //cout<<CARDINHAND[i][j];
            convertNumToChar(numchar, CARDINHAND[i][j]);
            //cout<<CARDINHAND[i][j]<<" : "<<numchar<<" , ";
            strcat(str,numchar);
            strcat(str,",");
        }
        strcat(str, "|");
    }
    //cout<<endl<<"\t\tCards in Hand: "<<str;//CARDSINHAND;
    actclient(str);
    memset(BUFFER, 0, MAXBUF);
    SUPPRESSDATA=1; SPECIALFLAGS=1; actserver();
    //cout<<endl<<"\t\tMy Cards: ";
    decryptremotecards(str, inversekey);
    deducercvdcards(str);
    cout<<endl<<"\t\tMY HAND:-\n\t\t----------------";
    for(int i=0; i<MAXHAND; i++) cout<<endl<<"\t\t"<<MYCARDS[i]; cout<<endl;
    SPECIALFLAGS=1; SUPPRESSDATA=1; actserver();
    char* oppcard = (char*)calloc(STR_LEN, sizeof(char));
    strcpy(oppcard, BUFFER);
//    int oppcard = deduceopponentcard();
    int tempinx=0;
    do
    {
        MYBET = boundedintinput(1, MAXHAND, "\t\tPlease choose a card from your Hand to place a bet: ");
        tempinx=boundedintinput(1, 1, "\t\tSure? (hit 1): ");
    }while(tempinx!=1);
    memset(str, 0, MAXBUF);
    strcpy(str, MYCARDS[MYBET-1]);
    backmarker();
    actclient(str);
    MYRANK = getrank(MYCARDS[MYBET-1]);
    OPPRANK = getrank(oppcard);
    cout<<endl<<"\t\tMy Choice:            "<<MYCARDS[MYBET-1]<<" Rank: "<<MYRANK;
    cout<<endl<<"\t\tMy Opponent's Choice: "<<oppcard<<" Rank: "<<OPPRANK;

    if(MYRANK>OPPRANK)cout<<endl<<"\t\tYOU WIN!!! \\m/ \m/";
    else
    {
        if(MYRANK == OPPRANK)
        {
            cout<<"\t\tGAME TIED! :| :|";
        }
        else
        {
            cout<<endl<<"\t\tYOU LOSE :P :P";
        }
    }
    cout<<endl;
    //**********************************************************

}


int gameMenuDisplay()
{
        int choice, lan;
        //char lanip[IP_LEN];
        cout<<endl<<"\t\t\tWELCOME TO MENTAL POKER v1.0";
        cout<<endl<<"\t\t------------------------------------------";
        cout<<endl<<"\t\t1. MENTAL POKER HOST";
        cout<<endl<<"\t\t2. CONNECT TO MULTIPLAYER";
        cout<<endl<<"\t\t3. QUIT";
        cout<<endl<<"\t\t------------------------------------------";

        //choice=boundedintinput(1, 3, "\n\t\t\tWELCOME TO MENTAL POKER v1.0\n\t\t------------------------------------------\n\t\t1. MENTAL POKER HOST\n\t\t2. CONNECT TO MULTIPLAYER\n\t\t3. QUIT\n\t\t------------------------------------------\n\n\t\tPlease choose an option in (1/2/3): ");

        choice = boundedintinput(1, 3, "\n\t\tPlease choose an option in (1/2/3): ");

        if(choice == 3) return choice;

        cout<<endl<<"\t\t\tWHAT KIND OF NETWORK INTERFACE ARE YOU USING?";
        cout<<endl<<"\t\t------------------------------------------------------";
        cout<<endl<<"\t\t1. ETHERNET LAN";
        cout<<endl<<"\t\t2. WIRELESS LAN";
        cout<<endl<<"\t\t------------------------------------------------------";

        lan = boundedintinput(1, 2, "\n\t\tPlease Select an option in (1/2): ");
        getselfIP(OWNIP, lan-1);
        cout<<endl<<"\t\tYour IP address is: "<<OWNIP;

        return choice;
}


int main()
{
    srand(time(0));

    int choice=gameMenuDisplay();
    switch(choice)
    {
        case 1: {   ORDER=1;
                    gameHost();
                    break;
                }
        case 2: {   ORDER=2;
                    gameConnect();
                    break;
                }
    }



    //system("netstat -tulnp");
    //actserver();
    //cout<<endl<<endl<<"Continue? ";
    //cin.get();
    //actclient(REMOTEIP, "Hello from QT!");
    /*cout<<endl<<"SERVER: listening ... ";
    buffer = (char *)calloc(MAXBUF, sizeof(char));
    bytesrcvd = receivedata(sfd, buffer);

    close(sfd);*/

return 0;
}
