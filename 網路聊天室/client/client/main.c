#include<stdlib.h>
#include<sys/socket.h>  //提供socket函數
#include<sys/types.h>       //數據類型定義
#include<stdio.h>
#include<netinet/in.h>      //sockaddr_in
#include<arpa/inet.h>       //htonl、htons
#include<string.h>          //memset
#include<unistd.h>          //close、write、read

#define SOCK_PORT 1111
#define BUF_SIZE 1024  //接收與傳送的字數

int main()
{
    int sockfd;
    struct sockaddr_in server_addr;
    char send_buf[BUF_SIZE];
    char recv_buf[BUF_SIZE];
    memset(send_buf, 0, BUF_SIZE);
    memset(recv_buf, 0, BUF_SIZE);

 //socket create
    sockfd = socket(AF_INET,SOCK_STREAM, 0);
    if(sockfd == -1){
        
        fprintf(stderr,"socket error!\n");
        exit(1);
    }

    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  //填入本機地址
    server_addr.sin_family = AF_INET;  //TCP/IP
    server_addr.sin_port = htons(SOCK_PORT); //隨機選一個未使用的
 
    //connect
    if(connect(sockfd,(struct sockaddr *)(&server_addr),sizeof(server_addr)) == -1){
        
        fprintf(stderr,"Connect error! \n");
        exit(1);
    }
    //name feature
    char name[100];
    printf("請輸入名字");
    gets(name);
    if(write(sockfd, name, 100) == -1){
        perror("name send failed");
        exit(-1);
    }
    while(1){
        //message input
        printf("輸入你想說的話(輸入 \"quit\" 離開):\n");
        char mesg[256];
        gets(mesg);
        if(strcmp(mesg,"quit") == 0){
            strcat(send_buf, mesg);

        }
        else{
            strcat(send_buf, name);
            strcat(send_buf, ": ");
            strcat(send_buf, mesg);
            
        }
        //check
        if(write(sockfd, send_buf, BUF_SIZE) == -1){
            
            perror("write error");
            exit(1);
            
    }
  
    //exit
    if(strcmp(send_buf, "quit") == 0){
        break;
    }
    else{
            
        if(read(sockfd, recv_buf, BUF_SIZE) == -1){
            perror("read failed");
        }
        printf("%s\n", recv_buf);
        memset(send_buf, 0, BUF_SIZE);
        memset(recv_buf, 0, BUF_SIZE);
        }
    }
 
 
 //close
    int ret = shutdown(sockfd, SHUT_WR);
    if(ret == -1){
        perror("shut down failed");
        exit(1);
    }
    return 0;
}
