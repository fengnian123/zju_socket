#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/un.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<thread>
#include<vector>
#include<ctime>
#include<unistd.h>
#include<mutex>
#include<iostream>
using namespace std;

struct clients{
    in_addr ip;
    in_port_t port;
    int cl_fd;
};
vector<clients> cli;

void process(int cli_fd){
    if(cli_fd==-1) cerr<<"accept wrong"<<endl;

    char buf[1000];
    while(1){
        // cout<<"1.获取时间"<<endl<<"2.获取名字"<<endl<<"3.获取客户端列表"<<endl
        // <<"4.发送消息"<<endl<<"5.断开连接"<<endl<<"6.退出"<<endl<<"请输入序号选择功能"<<endl;

        if(recv(cli_fd,buf,1000,0)==-1) cerr<<"wrong recv"<<endl;
        if(buf[0]=='1'){
            memset(buf,0,1000);
            time_t currentTime = time(nullptr);
            std::strftime(buf, 80, "%Y-%m-%d %H:%M:%S", localtime(&currentTime));
            send(cli_fd,buf,80,0);
        }
        if(buf[0]=='2'){
            memset(buf,0,1000);
            gethostname(buf,80);
            send(cli_fd,buf,80,0);
        }
        if(buf[0]=='3'){
            memset(buf,0,1000);
            string s="客户端列表如下：\n";
            mutex lock;
            lock.lock();
            int k=1;
            char* temp;
            for(auto i=cli.begin();i!=cli.end();i++,k++){
                temp=inet_ntoa(i->ip);
                s+=to_string(k)+". ip: "+string(temp)+" port: "+to_string(i->port)+"\n";                
            }
            temp=(char*)s.c_str();
            memcpy(buf,temp,sizeof(s));
            cout<<s<<endl;
            send(cli_fd,buf,sizeof(s),0);
            lock.unlock();

        }
        if(buf[0]=='4'){
            memset(buf,0,1000);
            recv(cli_fd,buf,80,0);
            string s(buf);
            mutex lock;
            lock.lock();
            auto it=cli.begin();
            for(auto i=cli.begin();i!=cli.end();i++){
                if(inet_addr(s.substr(0,s.find_first_of('/')).c_str())==i->ip.s_addr && 
                        i->port==stoi(s.substr(s.find_first_of('/')+1,s.find_first_of('0')-s.find_first_of('/')-1))){
                        it=i;
                        break;
                }
            }
            lock.unlock();
            while(1){
                memset(buf,0,1000);
                recv(cli_fd,buf,1000,0);
                if(buf[0]=='6') break;
                send(it->cl_fd,buf,1000,0);
            }
        }
        if(buf[0]=='5' || buf[0]=='6'){
            mutex lock;
            lock.lock();
            for(auto i=cli.begin();i!=cli.end();i++){
                if(i->cl_fd==cli_fd){
                       cli.erase(i);
                }
            }  
            lock.unlock();
            close(cli_fd);
        }
    }

}

int main(){
    int soc = socket(AF_INET,SOCK_STREAM,0);

    sockaddr_in s_addr;
    sockaddr_in c_addr;
    memset(&s_addr,0,sizeof(sockaddr_in));

    s_addr.sin_family=AF_INET;
    s_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    s_addr.sin_port=htons(8080);

    bind(soc,(sockaddr*)&s_addr,sizeof(s_addr));

    listen(soc,8);
    socklen_t c_len=sizeof(sockaddr_in);

    while(1){
        int c_fd=accept(soc,(sockaddr*)(&c_addr),&c_len);
        clients cl;
        cl.ip=c_addr.sin_addr;
        cl.port=c_addr.sin_port;
        cl.cl_fd=c_fd;
        cli.push_back(cl);
        thread work=thread(process,c_fd);

    }

    return 0;
}
