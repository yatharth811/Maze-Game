#include<bits/stdc++.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL_net.h>
using namespace std;

int main(int argc, char** argv){
    SDL_Init(SDL_INIT_EVERYTHING);
    SDLNet_Init();

    IPaddress ip;
    SDLNet_ResolveHost(&ip, NULL, 1234);


    TCPsocket server = SDLNet_TCP_Open(&ip);
    TCPsocket client1,client2;
    int data1[7], data2[7];
    bool flag1=false,flag2=false;
    while(true){
        client1 = SDLNet_TCP_Accept(server);
        if (client1){
            cout << "connection made with p1\n";
            flag1 = true;
            break;
        }
    }

    while(true){
        client2 = SDLNet_TCP_Accept(server);
        if (client2){
            cout << "connection made with p2\n";
            flag2 = true;
            break;
        }
    }

    while(flag1 && flag2){
        SDLNet_TCP_Recv(client1,data1,28);
        SDLNet_TCP_Recv(client2,data2,28);
        SDLNet_TCP_Send(client1, data2, 28);
        SDLNet_TCP_Send(client2, data1, 28);
        if( data1[0] == -1 || data2[0] == -1){
            SDLNet_TCP_Close(client1);
            SDLNet_TCP_Close(client2);
            break;
        }
    }


    SDLNet_TCP_Close(server);
    SDLNet_Quit();
    SDL_Quit();
}
