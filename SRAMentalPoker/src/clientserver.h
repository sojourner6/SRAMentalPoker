/*
 * clientserver.h
 *
 *  Created on: Dec 30, 2014
 *      Author: sojourner
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#ifndef CLIENTSERVER_H
#define CLIENTSERVER_H

#endif // CLIENTSERVER_H

using namespace std;

// ************* www.beej.us ***********************
// *************************************************
/*
struct addrinfo {
    int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
    int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
    int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
    int              ai_protocol;  // use 0 for "any"
    size_t           ai_addrlen;   // size of ai_addr in bytes
    struct sockaddr *ai_addr;      // struct sockaddr_in or _in6
    char            *ai_canonname; // full canonical hostname

    struct addrinfo *ai_next;      // linked list, next node
};
*/
// *************************************************
// ************* www.beej.us ***********************

#define SERVERPORT "11111"    // the port users will be connecting to
#define PORT1 "11111"
#define MAXBUF 1024
#define IP_LEN 15
#define STR_LEN 64

static int rcvcount;
static int sercount;
int ORDER;
int SPECIALFLAGS, SUPPRESSDATA;

char* REMOTEIP = (char *)calloc(IP_LEN, sizeof(char));

char* OWNIP = (char *)calloc(IP_LEN, sizeof(char));

char* P1IP = (char *)calloc(IP_LEN, sizeof(char));

char* P2IP = (char *)calloc(IP_LEN, sizeof(char));
char* BUFFER = (char *)calloc(MAXBUF, sizeof(char));

void getselfIP(char* rcvdmem, int n)
{
    char ipcommand[2][MAXBUF]={"ifconfig | grep -A4 eth0 | grep \"inet addr\" | awk -F' ' '{print $2}' | awk -F':' '{print $2}'",
                               "ifconfig | grep -A4 wlan0 | grep \"inet addr\" | awk -F' ' '{print $2}' | awk -F':' '{print $2}'"};
    char cliop[MAXBUF], c;
    FILE *pf;
    if(pf=popen(ipcommand[n],"r"))
    {
        for(int i=0 ; !feof(pf) ;i++ )
        {
            c=fgetc(pf);
            if(c=='\n' || c=='\0') { cliop[i]='\0'; break; }
            cliop[i] = c;
        }
    }
    pclose(pf);
    strcpy(rcvdmem, cliop);

}

void *get_in_addr(struct sockaddr *sa)
{
        if (sa->sa_family == AF_INET){ return &(((struct sockaddr_in*)sa)->sin_addr); }

        return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void displayconnectioninfo(addrinfo &a)
{
        cout<<"\n\t\tSERVER FAMILY: "        <<a.ai_family;
        cout<<"\n\t\tSERVER SOCKET TYPE: "   <<a.ai_socktype;
        cout<<"\n\t\tSERVER PROTOCOL: "      <<a.ai_protocol;
        cout<<"\n\t\tSERVER ADDRESS LENGTH: "<<a.ai_addrlen;
}

int gethostinfo(const char* host, const char* port, addrinfo &hints,addrinfo* &servinfo) { return getaddrinfo(host, port, &hints, &servinfo); }

int formsocket(addrinfo *a) { return socket(a->ai_family, a->ai_socktype, a->ai_protocol); }

int bindsocket(int n, addrinfo* a) { if(bind(n, a->ai_addr, a->ai_addrlen) == -1){ perror("bind"); exit(1); } else return 0;}

int receivedata(int fd, char* buf)
{
    int bytes;
    char s[INET6_ADDRSTRLEN];
    char* bufr = (char *)calloc(MAXBUF, sizeof(char));
    sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);

    if((bytes = recvfrom(fd, bufr, MAXBUF , 0, (struct sockaddr *)&addr, &addr_len)) == -1)
    {perror("recvfrom");exit(1);}
    bufr[bytes]='\0';

    strcpy(REMOTEIP, inet_ntop(addr.ss_family,
                              get_in_addr((struct sockaddr *)&addr),
                              s, sizeof s));
    strcpy(buf, bufr);
    return bytes;

}

void inferhostresult(const char* host, int result, addrinfo a)
{ if( result < 0 ){ cout<<host<<" "<<gai_strerror(result); exit(1); } }

int infersocketresult(addrinfo* va)
{
    int s;
    if((s=socket(va->ai_family, va->ai_socktype, va->ai_protocol)) == -1)
    { cout<<endl<<"Error: "<<s; perror("socket"); exit(1); }
    else return s;
}

int actserver()
{
        int sfd;
        struct addrinfo a, *ainfo, *b;
        memset(&a, 0, sizeof(a));
        int bytesrcvd;
        char* buffer;

        a.ai_family = AF_UNSPEC;
        a.ai_socktype = SOCK_DGRAM;
        a.ai_flags = AI_PASSIVE;

        inferhostresult(OWNIP, gethostinfo(/*OWNIP*/NULL, PORT1, a, ainfo), a);

        for(b = ainfo; b != NULL; b=b->ai_next){ bindsocket((sfd = infersocketresult(b)), b);break; }

        if (b == NULL) {fprintf(stderr, "SERVER: failed to bind socket\n"); return 2;}
        freeaddrinfo(ainfo);

        printf("\n\t\tPLAYER%d: Listening...\n", ORDER);
        buffer = (char *)calloc(MAXBUF, sizeof(char));

        bytesrcvd = receivedata(sfd, buffer);

        if(sercount == 0)
        {
            strcpy(REMOTEIP, buffer);
            //printf("\n\t\tPLAYER%d: packet is %d bytes long\n", ORDER, bytesrcvd);
            printf("\n\t\tPLAYER%d: %s has joined the Game\n", ORDER, buffer);
        }
        else
        {
            //printf("\n\t\tPLAYER%d: packet is %d bytes long\n", ORDER, bytesrcvd);
            if(SPECIALFLAGS == 1)
            {
                //BUFFER = (char *)calloc(MAXBUF, sizeof(char));
                strcpy(BUFFER, buffer); SPECIALFLAGS = 0;
            }

//            if(SPECIALFLAGS == 2)
//            {
//                //BUFFER = (char *)calloc(MAXBUF, sizeof(char));
//                strcpy(BUFFER, buffer); SPECIALFLAGS = 0;
//            }
            if(SUPPRESSDATA == 1)
            {
                SUPPRESSDATA = 0;
                printf("\n\t\tPLAYER%d: Confidential data received from other player!\n", ORDER);
            }
            else
            {
                printf("\n\t\tPLAYER%d: %s\n", ORDER, buffer);
            }
        }
        sercount++;
        //printf("\nReceived data: %s\n",buffer);

        close(sfd);

return 0;
}

int actclient(/*char* server,*/ char* msg)
{
        int cfd;
        struct addrinfo hst, *servinfo, *p;
        //int rv;
        int numbytes;

        memset(&hst, 0, sizeof(hst));
        hst.ai_family = AF_UNSPEC;
        hst.ai_socktype = SOCK_DGRAM;
        hst.ai_flags = AI_PASSIVE; // *********** recheck ******

        //inferhostresult(server, gethostinfo(server, PORT1, hst, servinfo), hst);
        inferhostresult(REMOTEIP, gethostinfo(REMOTEIP, PORT1, hst, servinfo), hst);
        /*
        if ((rv = getaddrinfo(server, PORT1, &hst, &servinfo)) != 0)
        {fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv)); return 1;}
        */

        for(p = servinfo; p != NULL; p=p->ai_next){ cfd = infersocketresult(p);break; }
        /*
        for(p = servinfo; p != NULL; p = p->ai_next)
        {
                if ((cfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
                { perror("talker: socket");  continue; }
                break;
        }
        */
        if (p == NULL) { fprintf(stderr, "talker: failed to bind socket\n");  return 2; }
        freeaddrinfo(servinfo);

        if(rcvcount == 0)
        {
            if ((numbytes = sendto(cfd, OWNIP, strlen(OWNIP), 0,p->ai_addr, p->ai_addrlen)) == -1)
            {perror("talker: sendto"); exit(1); }
        }
        else
        {
            if ((numbytes = sendto(cfd, msg, strlen(msg), 0,p->ai_addr, p->ai_addrlen)) == -1)
            {perror("talker: sendto"); exit(1); }
        }

        //printf("talker: sent %d bytes to %s\n", numbytes, server);
        //printf("\n\t\ttalker: sent %d bytes to %s\n", numbytes, REMOTEIP);
        close(cfd);
            //int  res = becomeServer();
        rcvcount++;
    return 0;
}


