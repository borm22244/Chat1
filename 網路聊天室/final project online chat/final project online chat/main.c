#include<stdlib.h>
#include<pthread.h>
#include<sys/socket.h>
#include<sys/types.h>       //pthread_t , pthread_attr_t and so on.
#include<stdio.h>
#include<netinet/in.h>      //structure sockaddr_in
#include<arpa/inet.h>       //Func : htonl; htons; ntohl; ntohs
#include<string.h>          //Func :memset
#include<unistd.h>          //Func :close,write,read

#define SOCK_PORT 1111
#define BUF_SIZE 1024
#define MAX_PEOPLE 20     //MAX connection limit

static void Data_trans(void * sock_fd);   //Only can be seen in the file

int main()
{
    int serverfd;
    int newfd;
    int temp;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int client_length;

    serverfd = socket(AF_INET, SOCK_STREAM, 0);  //ipv4,TCP
    
    if(serverfd == -1){
        perror("socket failed\n");
    }
    
    //before bind(), set the attr of structure sockaddr.
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SOCK_PORT);
    
    
    if(bind(serverfd, (struct sockaddr *)(&server_addr), sizeof(server_addr)) == -1){
        
        perror("bind failed\n");
        exit(1);
    }
    
    if(listen(serverfd, MAX_PEOPLE) == -1){
        
        perror("listen failed\n");
        exit(1);
    }
    
    printf("伺服器已建立，等待連接\n");
    
    for(;;){

        pthread_t thread_id;
        client_length = sizeof(client_addr);
        newfd = accept(serverfd, (struct sockaddr *)(&client_addr), (socklen_t *)(&client_length));
        
        if(newfd == -1)
        {
            perror("accept failed");                      //ignore current socket ,continue while loop.
        }
        if(pthread_create(&thread_id, NULL, (void *)(&Data_trans), (void *)(&newfd)) == -1)
        {
            perror("pthread create failed");
            exit(1);
        }
    }

    return 0;
}

static void Data_trans(void * sock_fd){
    
    int fd = *((int *)sock_fd);
    int recv_flag;
    char recv_buff[BUF_SIZE];
    const char * data_send = "傳送成功!\n";
    
    char name[100];
    if(read(fd, name, 100) == -1){
        perror("read name failed");
    }
    printf("歡迎%s來到聊天室!\n", name);
    
    for(;;){
        
        memset(recv_buff,0,BUF_SIZE);

        recv_flag = read(fd, recv_buff, BUF_SIZE);
        if(recv_flag == 0){
            printf("Maybe the client has closed\n");
            break;
        }
        if(recv_flag == -1){
            fprintf(stderr,"read error!\n");
            break;
        }
        if(strcmp(recv_buff,"quit") == 0){
            printf("Quit\n");
            break;
        }
        printf("%s\n", recv_buff);
        if(write(fd,data_send,strlen(data_send)) == -1){
            perror("write failed");
            exit(-1);
        }
    }

    //Clear
    printf("關閉此客戶端...\n");
    close(fd);            //close a file descriptor.
    pthread_exit(NULL);   //terminate calling thread!
}
