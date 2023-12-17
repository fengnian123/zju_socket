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

struct clients{//store ip address and port
    in_addr ip;
    in_port_t port;
    int cl_fd; //socket_fd
};
vector<clients> cli;

void process(int cli_fd){
    if(cli_fd==-1) cerr<<"accept wrong"<<endl; // If socket_fd==-1, it is invalid, return

    char buf[1000]; // Data buffer
    while(1){
        memset(buf,0,1000);
        if(recv(cli_fd,buf,1000,0)==-1) cerr<<"wrong recv"<<endl; // Bad receive results
        
        // Check different options based on the received input
        if(buf[0]=='1'){ // Client sends time request
            memset(buf,0,1000);
            
            // Get current time
            time_t currentTime = time(nullptr);
            std::strftime(buf, 80, "%Y-%m-%d %H:%M:%S", localtime(&currentTime));
            
            cout<<"用户请求时间："<<endl<<buf<<endl;
            
            // Send the current time to the client
            send(cli_fd,buf,1000,0);
            memset(buf,0,1000);
            
        }
        if(buf[0]=='2'){ // Client requests server hostname
            memset(buf,0,1000);
            
            // Get the hostname
            gethostname(buf,80);
            
            cout<<"用户请求服务器名称："<<endl;
            cout<<buf<<endl;
            
            // Send the hostname to the client
            send(cli_fd,buf,1000,0);
            memset(buf,0,1000);    
        }
        if(buf[0]=='3'){ // Client requests a list of connected clients
            memset(buf,0,1000);
            cout<<"用户请求客户端列表："<<endl;
            string s="客户端列表如下：\n";
            
            // Iterate through the client list and construct the response message
            int k=1;
            char* temp;
            for(auto i=cli.begin();i!=cli.end();i++,k++){
                temp=inet_ntoa(i->ip);
                s+=to_string(k)+". ip: "+string(temp)+" port: "+to_string(i->port)+"\n";                
            }
            temp=(char*)s.c_str();
            memcpy(buf,temp,1000);
            cout<<s;
            
            // Send the client list to the requesting client
            send(cli_fd,buf,1000,0);
            memset(buf,0,1000);
        }
        if(buf[0]=='4'){ // Client requests to send a message
            cout<<"用户请求发消息: hello"<<endl;
            cout<<"发送成功"<<endl;
            memset(buf,0,1000);
            
            // Receive the target client's address and port from the sender
            recv(cli_fd,buf,1000,0);
            string s(buf);
            
            mutex lock;
            lock.lock();
            auto it=cli.end();
            
            // Find the target client in the client list
            for(auto i=cli.begin();i!=cli.end();i++){
                if(inet_addr(s.substr(0,s.find_first_of('/')).c_str())==i->ip.s_addr && 
                        i->port==stoi(s.substr(s.find_first_of('/')+1,s.find_first_of('0')-s.find_first_of('/')-1))){
                        it=i;
                        break;
                }
            }
            
            buf[0]='#';
            
            // Send a notification to the target client that someone wants to communicate
            send(it->cl_fd,buf,1000,0);
            recv(it->cl_fd,buf,1000,0);
            cout<<buf<<endl;
            
            // If the target client rejects or if it was not found, notify the sender
            if(buf[0]=='*'||it==cli.end()){
                buf[0]='*';
                send(cli_fd,buf,1000,0);
            }
            else{
                while(1){
                    memset(buf,0,1000);
                    
                    // Receive message from the sender
                    if(recv(cli_fd,buf,1000,0)==-1) break;;
                    cout<<"用户发来消息：\n"<<buf<<endl;
                    if(buf[0]=='0') break;
                    
                    // Send the received message to the target client
                    send(it->cl_fd,buf,1000,0);

                    memset(buf,0,1000);
                    
                    // Receive response from the target client
                    if(recv(it->cl_fd,buf,1000,0)==-1) break;;
                    cout<<"用户发来消息：\n"<<buf<<endl;
                    if(buf[0]=='0') break;
                    
                    // Send the response back to the sender
                    send(cli_fd,buf,1000,0);
                }
            }
            memset(buf,0,1000);
            lock.unlock();
        }
        if(buf[0]=='5' || buf[0]=='6'){ // Client requests disconnection or quitting
            mutex lock;
            lock.lock();
            
            // Remove the client from the client list
            for(auto i=cli.begin();i!=cli.end();i++){
                if(i->cl_fd==cli_fd){
                       cli.erase(i);
                       break;
                }
            }  
            lock.unlock();
            
            // Close the client socket and break from the loop
            close(cli_fd);
            break;
        }
    }
    return;
}


int main(){
    int soc = socket(AF_INET,SOCK_STREAM,0); // Create a socket

    sockaddr_in s_addr;
    sockaddr_in c_addr;
    memset(&s_addr,0,sizeof(sockaddr_in));

    s_addr.sin_family=AF_INET;
    s_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    s_addr.sin_port=htons(8000);

    bind(soc,(sockaddr*)&s_addr,sizeof(s_addr)); // Bind the socket to the specified IP address and port

    listen(soc,8); // Start listening for incoming connections
    socklen_t c_len=sizeof(sockaddr_in);
    thread work[8];
    int i=0;
    cout<<"服务器已启动"<<endl; // Server is running notification
    while(1){
        int c_fd=accept(soc,(sockaddr*)(&c_addr),&c_len); // Accept a client connection
        if(c_fd==-1) cout<<"ac wrong\n";
        
        clients cl;
        cl.ip=c_addr.sin_addr;
        cl.port=c_addr.sin_port;
        cl.cl_fd=c_fd;
        cli.push_back(cl); // Add the client information to the client list
        cout<<"添加客户端，ip: "<<inet_ntoa(c_addr.sin_addr)<<" 端口: "<<c_addr.sin_port<<endl;
        work[i++]=thread(process,c_fd); // Spin off a thread to handle the client connection

    }
    for(int i;i<8;i++) work[i].join(); // Wait for all threads to finish
    return 0;
}

