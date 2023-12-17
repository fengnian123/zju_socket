#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
using namespace std;

// Function to receive and process messages from the connected client
void rec_message(int stream) {
    cout << "Someone is requesting a call. Input a number to choose an option:\n1. Accept the call\n2. Reject the call" << endl;
    // Read user's choice
    int c;
    cin >> c;
    char buf[1000];
    if (c == 1) {
        const char* s = "yes";
        // Send the response "yes" to accept the call
        send(stream, s, 3, 0);
        string mes;
        while (1) {
            cout << "Enter a message (enter 0 to quit sending messages): ";
            // Read the user's message input
            cin >> mes;
            if (mes[0] == '0') {
                // If the user entered '0', send a termination signal and exit
                buf[0] = '0';
                send(stream, buf, 1000, 0);
                cout << "You have exited." << endl;
                break;
            }
            // Convert the message string to a C-style string
            s = (char*)mes.c_str();
            // Send the message to the connected client
            send(stream, s, mes.size(), 0);
            // Receive the response from the connected client
            recv(stream, buf, 1000, 0);
            // Print the received response
            cout << buf << endl;
        }
    } else {
        // If the user chose option 2, send a rejection signal to the connected client
        buf[0] = '*';
        send(stream, buf, 1, 0);
    }
}


int main() {
new_pro:
    // Display menu options
    cout << "Function menu:" << endl;
    cout << "1. Connect" << endl << "2. Quit" << endl << "Enter a number to choose an option: ";
    // Input the choice
    int cho_0;
    cin >> cho_0;
    sockaddr_in ser_addr;
    memset(&ser_addr, 0, sizeof(sockaddr_in));
    if (cho_0 == 1) {
        // Create a new socket
        int stream = socket(AF_INET, SOCK_STREAM, 0);
        if (stream == -1) cerr << "Incorrect stream" << endl;
        ser_addr.sin_family = AF_INET;
        cout << "Enter the server IP address: ";
        string ip_str;
        cin >> ip_str;
        cout << "Enter the server port: ";
        string port_str;
        cin >> port_str;
        // Set up server address structure
        ser_addr.sin_port = htons(stoi(port_str));
        ser_addr.sin_addr.s_addr = inet_addr(ip_str.c_str());
        // Connect to the server
        if (connect(stream, (struct sockaddr*)&ser_addr, sizeof(sockaddr)) == -1) cerr << "Failed to connect" << endl;
        // Display the menu options on successful connection
        cout << "Connection successful!" << endl << endl << "Function menu:" << endl;
        cout << "1. Get time" << endl << "2. Get name" << endl << "3. Get client list" << endl
            << "4. Send message" << endl << "5. Disconnect" << endl << "6. Quit" << endl << "Enter a number to choose an option: ";
        int cho_1;
        char buf[1000];
        while (1) {
    cin >> cho_1;
    if (cho_1 == 1) {
        for (int i = 0; i < 100; i++) {
            cout << "The time is: " << endl;
            // Clear the buffer
            memset(buf, 0, 1000);
            // Set the message type to '1' to request the server's time
            buf[0] = '1';
            // Send the request to the server
            send(stream, buf, 1, 0);
            // Receive the response from the server
            recv(stream, buf, 1000, 0);
            // Check if the server wants to initiate a call ('#')
            if (buf[0] == '#') rec_message(stream);
            // Print the received response (time)
            cout << buf << endl;
        }
    } else if (cho_1 == 2) {
        cout << "The host name is: " << endl;
        // Clear the buffer
        memset(buf, 0, 1000);
        // Set the message type to '2' to request the server's hostname
        buf[0] = '2';
        // Send the request to the server
        send(stream, buf, 1, 0);
        // Receive the response from the server
        recv(stream, buf, 1000, 0);
        // Check if the server wants to initiate a call ('#')
        if (buf[0] == '#') rec_message(stream);
        // Print the received response (hostname)
        cout << buf << endl;
    } else if (cho_1 == 3) {
        // Clear the buffer
        memset(buf, 0, 1000);
        // Set the message type to '3' to request the list of connected clients
        buf[0] = '3';
        // Send the request to the server
        send(stream, buf, 1, 0);
        // Receive the response from the server
        recv(stream, buf, 1000, 0);
        // Check if the server wants to initiate a call ('#')
        if (buf[0] == '#') rec_message(stream);
        // Print the received response (client list)
        cout << buf << endl;
    } else if (cho_1 == 4) {
        // Set the message type to '4' to indicate sending a message
        buf[0] = '4';
        // Send the request to the server
        send(stream, buf, 1, 0);
        string s1;
        int s2;
        cout << "Enter the destination IP: " << endl;
        cin >> s1;
        cout << "Enter the destination port: " << endl;
        cin >> s2;
        // Create the message in the format "<destination IP>/<destination port>"
        char* s = (char*)((s1 + '/' + to_string(s2)).c_str());
        // Copy the constructed message to the buffer
        memcpy(buf, s, 1000);
        // Send the message to the server
        send(stream, buf, 1000, 0);
        // Clear the buffer
        memset(buf, 0, 1000);
        // Receive the response from the server
        recv(stream, buf, 1000, 0);
        // Check if the server wants to initiate a call ('#')
        if (buf[0] == '#') rec_message(stream);
        // Check if the server rejected the call ('*')
        if (buf[0] == '*') {
            cout << "User rejected talking to you" << endl;
        } else {
            char* b;
            while (1) {
                cout << "Enter a message (enter 0 to quit sending messages): ";
                cin >> s1;
                memset(buf, 0, 1000);
                if (s1[0] == '0') {
                    // If the user entered '0', send a termination signal and exit
                    buf[0] = '0';
                    send(stream, buf, 1000, 0);
                    cout << "You have exited." << endl;
                    break;
                }
                // Convert the message string to a C-style string
                b = (char*)s1.c_str();
                // Send the message to the server
                send(stream, b, s1.size(), 0);
                // Receive the response from the server
                recv(stream, buf, 1000, 0);
                // Print the received response
                cout << buf << endl;
            }
        }
    } else if (cho_1 == 5) {
        // Clear the buffer
        memset(buf, 0, 1000);
        // Set the message type to '5' to indicate disconnection
        buf[0] = '5';
        // Send the request to the server
        send(stream, buf, 1, 0);
        // Go back to the beginning of the program
        goto new_pro;
    } else if (cho_1 == 6) {
        // Clear the buffer
        memset(buf, 0, 1000);
        // Set the message type to '6' to indicate quitting
        buf[0] = '6';
        // Send the request to the server
        send(stream, buf, 1, 0);
        // Exit the program
        return 0;
    }  
}

}}
