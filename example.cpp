/**
 * example.cpp -- VERSION 1.0
 *
 * A example to show how to use myconsole.h
 *
 * You can find latest version at https://github.com/spadejack014/myconsole.
 *
 * ------------------------------------------------------------------------
 * MIT License
 *
 * Copyright (c) 2022 L Ding(spadejack014 github)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/
#include <iostream>
#include "myconsole.h"

using namespace std;

int testlist[3][3] = {{1,2,3},{4,5,6},{7,8,9}};

int test_callback(int socket_id, int argc, char **argv)
{
    if( argc == 3){
        argv++;
        int x = stoi(*argv);
        argv++;
        int y = stoi(*argv);
        string tmp;
        if(x>=0&&x<3&&y>=0&&y<3){
            tmp = "right answer:" + std::to_string(testlist[x][y]);
        }else{
            tmp = "wrong answer";
        }
        myprintf(socket_id,tmp.c_str());
    }else{
        myprintf(socket_id,"test usage:");
        myprintf(socket_id,"\ttest [x] [y] \t--get the value");
    }
    return 0;
}

int showAll_callback(int socket_id, int argc, char **argv)
{
    if( argc == 1){
        string tmp = "1,2,3\n4,5,6\n7,8,9";
        myprintf(socket_id,tmp.c_str());
    }else{
        myprintf(socket_id,"all usage:");
        myprintf(socket_id,"\tall \t--get all value");
    }
    return 0;
}

int main(int argc, char **argv)
{
#ifdef _WIN32
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(sockVersion, &wsaData) != 0)
    {
        std::cout << "Initialize WSA failed" << std::endl;
        return 0;
    }
#endif

    int writefd = 0;
    int readfd = 0;
    if(argc!=1){
        writefd = fileno(stdout);
        readfd = fileno(stdin);
    }else{
        while(argc > 1) {
            argc--;
            argv++;
            /**
            if (!strcmp(*argv,"-d")) {
                daemonize();
            }else if(*argv,"-v"){
                ;
            }else{
                ;
            }
            */
        }

        //should after daemonize();
        if( (readfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ){
            perror("socket(readfd) error");
            exit(1);
        }
        struct sockaddr_in readAddr;

        readAddr.sin_family = AF_INET;
        readAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        readAddr.sin_port = htons(1235);


        if (bind(readfd, (sockaddr *)&readAddr, sizeof(readAddr)) == -1) {
            perror("bind failed: readfd");
        }
        writefd=readfd;

    }
    char *prompt = "test";

    myconsole_init(prompt,writefd);
    myconsole_add_command_func("test",test_callback,"for test");
    myconsole_add_command_func("all",showAll_callback,"for test");
    myconsole_recv_command(readfd,writefd);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
