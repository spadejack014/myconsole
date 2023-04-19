 /**
 * myconsole.cpp -- VERSION 1.0
 *
 * A minimal, zero-config, MIT licensed, REPL used in Windows/Linux,
 * supports connecting via Windows terminal, Linux shell or UDP socket.
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

#include "myconsole.h"
#include <sstream>
#include <unistd.h>
typedef struct command_struct{
    CONSOLE_COMMAND_FP func;
    string help;
}COMMAND;

map<string,COMMAND> command_map;
struct sockaddr_in from;
socklen_t len=sizeof (from);
string myconsole_prompt;

int myconsole_help(int write_fd, int argc, char **argv);

int myconsole_caption(int write_fd, int argc, char **argv);

int myconsole_prase_command(int write_fd, int argc, char **argv);

void myconsole_prase_in_console(int read_fd, int write_fd);

void myconsole_prase_in_network(int read_fd, int write_fd);

int myconsole_init(char *prompt,int write_fd)
{
    myconsole_prompt = string(prompt)+"> ";
    myconsole_add_command_func("help",myconsole_help,"show command help");
    myconsole_add_command_func("caption",myconsole_caption,"show how to connect to this program");
    if(write_fd==fileno(stdout)){
        myconsole_caption(write_fd,1,nullptr);
        myprintf(write_fd, myconsole_prompt.c_str());
    }
    return 0;
}

int myconsole_add_command_func(char *command, CONSOLE_COMMAND_FP func, char *help)
{
    if(command_map.find(command)!=command_map.end()){
        return -1;
    }
    COMMAND tmp;
    tmp.func=func;
    tmp.help=help;
    command_map[command]=tmp;
    return 0;
}

int myconsole_recv_command(int read_fd,int write_fd)
{
    timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    fd_set fds;
    while(true){
        FD_ZERO(&fds);
        FD_SET(read_fd,&fds);
        if(select(read_fd+1,&fds,nullptr,nullptr,nullptr)>0){
            if(FD_ISSET(read_fd,&fds)!=0){
                if(read_fd==fileno(stdin)){
                    myconsole_prase_in_console(read_fd,write_fd);
                }else{
                    myconsole_prase_in_network(read_fd,write_fd);
                }
            }
        }
    }
    return 0;
}

int myprintf(int write_fd, const char* content)
{
    if(write_fd == 0)
        return -1;
    else if (write_fd==fileno(stdout)){
        fputs(content,stdout);
        if(0!=strcmp(content,myconsole_prompt.c_str())){
            fputs("\n",stdout);
        }
        return 0;
    }
    else{
        int ret = sendto(write_fd,content,strlen(content),0,(sockaddr*)&from,sizeof(from));
        return ret;
    }
}

int myconsole_help(int write_fd, int argc, char **argv)
{
    if(argc == 1){
        map<string,COMMAND>::iterator it = command_map.begin();
        for(;it!=command_map.end();it++){
            string tmp_command = it->first;
            if(it->first=="caption")
                continue;
            string tmp_help = it->second.help;
            string tmp = '\t'+tmp_command+":  "+tmp_help;
            myprintf(write_fd,tmp.c_str());
        }
    }else if( argc == 2){
        argc--;
        argv++;
        string tgt_command = *argv;
        if(command_map.find(tgt_command)==command_map.end()){
            string tmp = "no such command:\t" + tgt_command;
            myprintf(write_fd,tmp.c_str());
            return 0;
        }
        CONSOLE_COMMAND_FP tgt_func = command_map[tgt_command].func;

        char **tmpargv;
        tmpargv=(char **) malloc( 2*sizeof(char *) );
        tmpargv[0]=(char *)malloc(100);
        tmpargv[1]=(char *)malloc(100);
        strcpy(tmpargv[0],tgt_command.c_str());
        strcpy(tmpargv[1],"-h");
        tgt_func(write_fd,2,tmpargv);
        free(tmpargv[0]);
        free(tmpargv[1]);
        free(tmpargv);

    }else{
        myprintf(write_fd,"help usage:");
        myprintf(write_fd,"\thelp  \t\t--show command help");
        myprintf(write_fd,"\thelp [command]  \t\t--show concrete command help");
    }
    return 0;
}

int myconsole_caption(int write_fd, int argc, char **argv)
{
    if(argc == 1){
        if(write_fd==fileno(stdout))
            myprintf(write_fd,"connected by console!!!");
        else
            myprintf(write_fd,"connected by udp!!!");
    }else{
        myprintf(write_fd,"caption dose not need any param!!!");
    }
    return 0;
}

int myconsole_prase_command(int write_fd, int argc, char **argv)
{
    if(argc >= 1) {
        string tgt_command = *argv;
        if(command_map.find(tgt_command)==command_map.end()){
            string tmp = "no such command:\t" + tgt_command;
            myprintf(write_fd,tmp.c_str());
            myprintf(write_fd, myconsole_prompt.c_str());
            return 0;
        }
        CONSOLE_COMMAND_FP tgt_func = command_map[tgt_command].func;
        tgt_func(write_fd,argc,argv);
        if(write_fd==fileno(stdout))
            myprintf(write_fd, myconsole_prompt.c_str());
        return 0;
    }else{
        return -1;
    }
}


void myconsole_prase_in_console(int read_fd, int write_fd)
{
    string command,word;
    char buf[300]={0};
    int n = read(read_fd,buf,300);
    command = buf;
    if(command == "\n"){
        myprintf(write_fd,myconsole_prompt.c_str());
        return;
    }
    command.erase(0,command.find_first_not_of(" "));
    command.erase(command.find_last_not_of(" ") + 1);
    int count = 1;
    for (int i = 0; (i = command.find(' ', i)) != std::string::npos; i++) {
        count++;
    }
    char **argv;
    argv=(char **) malloc( count*sizeof(char *) );
    int i = 0;
    for ( std::istringstream is( command ); is >> word; )
    {
        argv[i]=(char *)malloc(100);
        strcpy(argv[i],word.c_str());
        i++;
    }
    int ret = myconsole_prase_command(write_fd,count,argv);
    if(ret == 0){
        for( i=0;i<count;i++ ) //释放内存
                free(argv[i]);
        free(argv);
    }
}

void myconsole_prase_in_network(int read_fd, int write_fd)
{
    char buf[1024]={0};
    int ret = recvfrom(read_fd,buf,1024,0,(sockaddr *)&from,&len);
    if(ret <= 0)
        return;
    string command = buf;
    if(command == "\n"){
        myprintf(write_fd,myconsole_prompt.c_str());
        return;
    }
    command.erase(0,command.find_first_not_of(" "));
    command.erase(command.find_last_not_of(" ") + 1);
    string word;
    int count = 1;
    for (int i = 0; (i = command.find(' ', i)) != std::string::npos; i++) {
        count++;
    }
    char **argv;
    argv=(char **) malloc( count*sizeof(char *) );
    int i = 0;
    for ( std::istringstream is( command ); is >> word; )
    {
        argv[i]=(char *)malloc(100);
        strcpy(argv[i],word.c_str());
        i++;
    }
    int iret = myconsole_prase_command(write_fd,count,argv);
    if(iret == 0){
        for( i=0;i<count;i++ ) //释放内存
                free(argv[i]);
        free(argv);
    }
}
