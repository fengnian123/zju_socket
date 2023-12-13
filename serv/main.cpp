#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/un.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<thread>
#include<vector>
#include<iostream>
using namespace std;

void process(){
    int soc = socket(AF_INET,SOCK_DGRAM,0);

    sockaddr_in s_addr;
    sockaddr_in c_addr;
    memset(&s_addr,0,sizeof(sockaddr_in));

    s_addr.sin_family=PF_INET;
    s_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    s_addr.sin_port=htons(1234);

    bind(soc,(sockaddr*)&s_addr,sizeof(s_addr));

    listen(soc,8);
    socklen_t c_len=sizeof(sockaddr_in);
    int c_fd=accept(soc,(sockaddr*)(&c_addr),&c_len);

    if(c_fd==-1) cerr<<"accept wrong"<<endl;

    char buf[1000];
    while(1){
        if(recv(c_fd,buf,1000,0)==-1) cerr<<"wrong recv"<<endl;
        cout<<buf<<endl;
    }

}

int main(){

    thread  works[8];

    for(int i=0;i<8;i++) works[i]=thread(process);

    for(int i=0;i<8;i++) works[i].join();

    return 0;
}
