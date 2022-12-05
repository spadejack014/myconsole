/**
 * myconsole.h -- VERSION 1.0
 *
 * A minimal, zero-config, MIT licensed, REPL used in Windows/Linux,
 * supports connecting via Windows terminal, Linux shell or UDP socket.
 *
 * See myconsole.cpp for more information.
 *
 * ------------------------------------------------------------------------
 * MIT License
 *
 * Copyright (c) 2022 spadejack014
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
#ifndef __MYCONSOLE_H
#define __MYCONSOLE_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
using namespace std;

#ifdef _WIN32
    #include <io.h>
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <winsock2.h>
    #include <WS2tcpip.h>
#else
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
#endif


/**
 * @brief  Initialize myconsole
 * @note   Add command [help] and set default prompt
 * @param  *prompt:     the command prompt
 * @param  write_fd:    File descriptor(socket or console) to send result
 * @retval -1           Error
 *         0            Success
*/
int myconsole_init(char *prompt,int write_fd);

/**
 * @brief  the callback fuction template
 * @note   Pass the callback function into test() as a parameter
*/
typedef int (*CONSOLE_COMMAND_FP)(int socket_id, int argc, char **argv);

/**
 * @brief  Register a command and its callback function
 * @note   The callback function needs to conform to the above template
 * @param  command:     command with no spaces
 * @param  func:        the callback fuction
 * @param  help:        Prompt text returned after the [help] command is read
 * @retval -1           the command has been registered before
 *         0            register successfully
 */
int myconsole_add_command_func(char* command, CONSOLE_COMMAND_FP func, char* help);

/**
 * @brief  Main API(a loop) for receiving and processing commands
 * @note   The file descriptor should be a udp socket or standard output
 * @param  read_fd:     File descriptor to read command
 * @param  write_fd:    File descriptor to send result
 * @retval 0
 */
int myconsole_recv_command(int read_fd,int write_fd);

/**
 * @brief  Sending results to a console or a socket
 * @note   None
 * @param  write_fd:    File descriptor to send result
 * @param  content:     The result need sending to the write_fd
 * @retval < 0          Error
 *         >=0          Success
 */
int myprintf(int write_fd, const char* content);

#endif /* __CROSSLINE_H */
