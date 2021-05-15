#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;
#define MAXDATASIZE 1000

void arg_parsing(char* hostname,char* port,char* path,char* arg);

int main(int argc, char *argv[])
{
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo;
    char hostname[100];
    char path[100] = "/";
    char port[50];
    int rv;
    string s1,s2,s3= " ";
    
    //인자의 갯수를 확인
    if(argc != 2){
        fprintf(stderr, "usage: http_client http://hostname[:port][/path/to/file]\n");
        exit(1);
    }

    // http://를 이용한 조건에 맞는지 확인.
    if(strncmp(argv[1],"http://",7) != 0){
        fprintf(stderr, "usage: http_client http://hostname[:port][/path/to/file]\n");
        exit(1);
    }

    //parsing arguments
    arg_parsing(hostname,port,path,argv[1]);
    
//  printf("%s\n",port);
//  printf("%s\n",path);
    memset(&hints, 0,sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if((rv = getaddrinfo(hostname,port,&hints,&servinfo)) != 0){
        fprintf(stderr,"getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    //create socket
    if((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1){
            perror("client: socket");
            return 2;
    }
    
    //connect to server
    if(connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1){
        close(sockfd);
        perror("connect");
        exit(1);
    }

    // free allocated memory for servinfo
    freeaddrinfo(servinfo);
    
    //request meassage 생성
    s1 = "GET ";
    s2 = path;
    s3 = " HTTP/1.1\r\n";
    s1 = s1 + s2 + s3;
    s2 = "Host: ";
    s3 = hostname;
    s1 += s2 + s3;
    s2 = port;
    s1 += ":" + s2 + "\r" + "\n" + "\r" + "\n";
    strcpy(buf,s1.c_str());

    //send request message
    if(send(sockfd, buf,strlen(buf),0) == -1){
        perror("send");
        close(sockfd);
        exit(1);
    }


    char tmp[50];
    int H_flag = 0,size = 1;
    int D_flag = 0;
    int C_flag = 0;
    int num = 0;
    //creating out file 
    FILE* fp=fopen("20161560.out","w");
    if(fp == NULL){
        printf("ERROR : FAIL TO CREAT 20161560.out\n");
    }
    //Content-Length와 20161560.out에 입력한 문자의 갯수를 비교하여 일치할때까지 recv를 통해 response를 받아온다.
    while(num < size){
        if((numbytes = recv(sockfd,buf,sizeof buf,0)) == -1){
            perror("recv");
            close(sockfd);
            exit(1);
        }

        //response받은 문자의 갯수만큼 한글자씩 반복문을 통해 확인
        for(int i = 0;i < numbytes;i++){
            //D_flag를 통해 Header part인지 Data part인지 구분
            if(!D_flag){
                //Header part인 경우 첫줄의 내용을 저장하고 출력.
                if(buf[i] == '\r' && buf[i+1] == '\n'){
                    if(!H_flag){//Header의 첫줄을 아직 저장하지 않았을 겨우. H_flag = 0
                        strncpy(tmp,buf,i+2);
                        tmp[i+2] = '\0';
                        printf("%s",tmp);
                        H_flag = 1;//Header의 첫줄을 저장했음을 flag = 1로 저장.
                    }
                    else{
                        //Content-Length를 대소문자 구분없이 인식하여 size에 저장.
                        if(strncmp(buf+i+2,"Content-Length",13) == 0 || strncmp(buf+i+2,"content-length",13) == 0 || strncmp(buf+i+2,"Content-length",13) == 0){
                            sscanf(buf+i+17,"%d",&size);
                            printf("%d bytes written to 20161560.out\n",size);
                            
                            C_flag = 1;//Content-Length field가 존재함
                        }
                        if(buf[i+2] == '\r' && buf[i+3] == '\n') {
                            //Content-Length field가 존재하지 않는경우 이에 대한 정보를 출력.
                            if(!C_flag){
                                printf("Content-Length not specified\n");
                                break;
                            }
                            D_flag = 1;
                            i += 3;
                        }
                    }
                }
            }
            //Data part의 경우.
            else{
                fprintf(fp,"%c",buf[i]);//20161560.out에 모두 저장.
                num++;//문자는 하나당 1byte이므로 갯수가 곧 content-length를 나타낸다.
            }
        }
        //Content-Length field가 없는 경우 종료.
        if(!C_flag) break;
    }
    fclose(fp);
    close(sockfd);
    return 0;
}

//parsing function
void arg_parsing(char* hostname,char* port,char* path,char* arg){
    strcpy(port,"80");//port number를 80으로 초기화
    int len = strlen(arg);
    int i;
    int port_point = -1, path_point;
    path_point = len;
    for(i = 7;i<len;i++){
        if(arg[i] == ':') {
            port_point = i;
        }
        if(arg[i] == '/') {
            path_point = i;
            break;
        }
    }
    //port number가 적혀있는 argument였다면, port넘버와 path가 시작되는 문자의 위치를 기준으로 계산하여 각 문자열을 parsing함.
    if(port_point != -1){
        strncpy(port,arg+port_point+1,path_point - port_point-1);
        strncpy(hostname,arg+7,port_point-7);
        port[path_point-port_point-1] = '\0';
        hostname[port_point-7] = '\0';
        strcpy(path+1,arg+path_point+1);
    }
    //port number가 없었다면 hostname과 path만 추출함.
    else{
        if(path_point != len){
           strcpy(path+1,arg+path_point+1);
        }
        strncpy(hostname,arg+7,path_point-7);
        hostname[path_point-7] ='\0';
    }
}
