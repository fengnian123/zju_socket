#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/un.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<iostream>
using namespace std;

int main(){
new_pro:
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
        ser_addr.sin_port=htons(3302);//stoi(port_str)

        //inet_aton ("127.0.0.1",&ser_addr.sin_addr);
         ser_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//ip_str.c_str()
        if(connect(stream,(struct sockaddr *)&ser_addr,sizeof(sockaddr))==-1)  cerr<<"wrong connect"<<endl;

        cout<<"连接成功！"<<endl<<endl<<"功能菜单："<<endl;
        cout<<"1.获取时间"<<endl<<"2.获取名字"<<endl<<"3.获取客户端列表"<<endl
        <<"4.发送消息"<<endl<<"5.断开连接"<<endl<<"6.退出"<<endl<<"请输入序号选择功能"<<endl;

        int cho_1;
        char buf[1000];
        while(1){
            cin>>cho_1;
            if(cho_1==1){
                cout<<"时间如下："<<endl;
                memset(buf,0,1000);
                buf[0]='1';
                send(stream,buf,1,0);
                recv(stream,buf,1000,0);
                cout<<buf<<endl;
            }
            if(cho_1==2){
                cout<<"主机名称如下："<<endl;
                memset(buf,0,1000);
                buf[0]='2';
                send(stream,buf,1,0);
                recv(stream,buf,1000,0);
                cout<<buf<<endl;
            }
            if(cho_1==3){
                memset(buf,0,1000);
                buf[0]='3';
                send(stream,buf,1,0);
                recv(stream,buf,1000,0);
                cout<<buf<<endl;
            }
            if(cho_1==4){
                buf[0]='4';
                send(stream,buf,1,0);
                string s1;
                int s2;
                cout<<"请输入目的ip：\n"<<endl;
                cin>>s1;
                cout<<"请输入目的端口：\n"<<endl;
                cin>>s2;
                char * s=(char*)((s1+'/'+to_string(s2)).c_str());
                memcpy(buf,s,1000);
                send(stream,buf,1000,0);
                char* b;
                while(1){
                    cout<<"请输入消息(退出发送消息请输入0)："<<endl;
                    cin>>s1;
                    memset(buf,0,1000);
                    if(s1[0]=='0') {
                        buf[0]='0';
                        send(stream,buf,1000,0);
                        cout<<"已退出"<<endl;
                        break;
                    }
                    b=(char*)s1.c_str();
                    memcpy(buf,&b,sizeof(b));
                    cout<<b<<endl;
                    send(stream,b,s1.size(),0);
                    //recv(stream,buf,1000,0);
                }
            }
            if(cho_1==5){
                memset(buf,0,1000);
                buf[0]='5';
                send(stream,buf,1,0);
                goto new_pro;
            }
            if(cho_1==6){
                buf[0]='6';
                send(stream,buf,1,0);
                return 0;
            } 
            
        }
    }

    if(cho_0==2) return 0;
}
