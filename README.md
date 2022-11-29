# myconsole

A minimal, zero-config, MIT licensed, REPL used in Windows/Linux, supports connecting via Windows terminal, Linux shell or UDP socket.

## Param

- `char *prompt`:             the command prompt
- `int write_fd`:             File descriptor to send result
- `int read_fd`:              File descriptor to read command
- `char* command`:            command with no spaces
- `char* func`:               the callback fuction
- `char* help`:               prompt text returned after send the [help] command
- `const char* content`:      The result need sending to the write_fd

```cpp
//if you want to run it in a Windows terminal, Linux shell
writefd = fileno(stdout);
readfd = fileno(stdin);

//or connect it via UDP socket
if( (writefd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ){
    perror("socket(writefd) error");
    exit(1);
}
if( (readfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ){
    perror("socket(readfd) error");
    exit(1);
}
struct sockaddr_in readAddr, writeAddr;

writeAddr.sin_family = AF_INET;
writeAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
writeAddr.sin_port = htons(1234);

readAddr.sin_family = AF_INET;
readAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
readAddr.sin_port = htons(1235);

if (bind(writefd, (sockaddr *)&writeAddr, sizeof(writeAddr)) == -1) {
    perror("bind failed: writefd");
}

if (bind(readfd, (sockaddr *)&readAddr, sizeof(readAddr)) == -1) {
    perror("bind failed: readfd");
}
```

## The API

### Init

```cpp
int myconsole_init(char *prompt,int write_fd);
```

- Initialize myconsole
- add command [help] and set default prompt

### Add Command and Callback

```cpp
typedef int (*CONSOLE_COMMAND_FP)(int socket_id, int argc, char **argv);

int myconsole_add_command_func(char* command, CONSOLE_COMMAND_FP func, char* help);

int myprintf(int write_fd, const char* content);
```

- Register a command and its callback function
- The callback function needs to conform to the above template

```cpp
int test_callback(int socket_id, int argc, char **argv)
{
    if( argc == 1){
        //func
    }else if(argc == 2 ){
        //func
    }
    }else{
        myprintf(socket_id,"test usage:");
        myprintf(socket_id,"\ttest [param1] [param2] \t--func info");
    }
    return 0;
}
```

### Recv command

```cpp
int myconsole_recv_command(int read_fd,int write_fd);
```

- Main API(a loop) for receiving and processing commands.
- The file descriptor should be a udp socket or standard output.
