#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/un.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<iostream>
using namespace std;
struct packet{

};

int main(){

    cout<<"功能菜单："<<endl;
    cout<<"1.连接"<<endl<<"2.退出"<<endl<<"请输入序号选择功能"<<endl;
    int cho_0;
    cin>>cho_0;
    sockaddr_in ser_addr;
    
    memset(&ser_addr,0,sizeof(sockaddr_in));
    if(cho_0==1){
        int stream=socket(AF_INET,SOCK_STREAM,0);
        if(stream==-1) cerr<<"wrong stream"<<endl;
        ser_addr.sin_family=AF_INET;
        
        cout<<"请输入服务器IP地址:"<<endl;
        string ip_str;
        cin>>ip_str;
        cout<<"请输入服务器端口："<<endl;
        string port_str;
        cin>>port_str;
        //link begin
        ser_addr.sin_port=htons(stoi(port_str));

        inet_aton (ip_str.c_str(),&ser_addr.sin_addr);
 
        if(connect(stream,(struct sockaddr *)&ser_addr,sizeof(struct sockaddr))==-1)  cerr<<"wrong connect"<<endl;

        cout<<"连接成功！"<<endl<<endl<<"功能菜单："<<endl;
        cout<<"1.获取时间"<<endl<<"2.获取名字"<<endl<<"3.获取客户端列表"<<endl
        <<"4.发送消息"<<endl<<"5.断开连接"<<endl<<"6.退出"<<endl<<"请输入序号选择功能"<<endl;

        int cho_1;
        char buf[1000];
        while(1){
            cin>>cho_1;
            if(cho_1==1){
                buf[0]='122';
                send(stream,buf,1,0);
            }
            if(cho_1==2){
                buf[0]='2';
                send(stream,buf,1,0);
            }
            if(cho_1==3){
                buf[0]='3';
                send(stream,buf,1,0);
            }
            if(cho_1==4){
                cout<<"Input:"<<endl;
                cin>>port_str;
                send(stream,&port_str,port_str.size(),0);
            }
            if(cho_1==5){
                send(stream,buf,1,0);
            }
            if(cho_1==6){
                return 0;
            } 
            
        }
    }

    if(cho_0==2) return 0;
}
