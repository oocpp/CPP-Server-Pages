//
// Created by lg on 17-5-14.
//

#pragma once
#include <sys/socket.h>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <cassert>
#include <netinet/in.h>
#include"InetAddress.h"
#include"DebugPrint.h"


    class Listener {
    public:

        int createNonBlockAndCloseOnExecSocket(sa_family_t family){
#if VALGRIND
            _sockfd = ::socket(family, SOCK_STREAM, IPPROTO_TCP);
            setNonBlockAndCloseOnExec(_sockfd);
#else
            _sockfd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
#endif
            if (_sockfd < 0) {
                dout << "sockets::createNonblockingOrDie";
            }
            return _sockfd;
        }

        explicit Listener(sa_family_t family=AF_INET) {

            createNonBlockAndCloseOnExecSocket(family);
        }

        int fd()const{
            return _sockfd;
        }
        void close(){
            if (::close(_sockfd) < 0) {
                dout << "sockets::close";
            }
        }

        bool bind(const InetAddress& local_addr){
            if (::bind(_sockfd,local_addr.getSockAddr() , local_addr.size6())< 0) {
                dout << "bind 失败";
                return false;
            }
            return  true;
        }

        bool listen(){
            if (::listen(_sockfd, SOMAXCONN) < 0) {
                dout << "listen 失败";
                return false;
            }
            return  true;
        }

        int accept(InetAddress& peeraddr){
            socklen_t addrlen = peeraddr.size6();

#if VALGRIND || defined (NO_ACCEPT4)
            int connfd = ::accept(_sockfd, peeraddr.getSockAddr(), &addrlen);
            setNonBlockAndCloseOnExec(connfd);
#else
            int connfd = ::accept4(_sockfd, peeraddr.getSockAddr(),
                                   &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
#endif
            return connfd;
        }

        ~Listener(){
            close();
        }

    public:
        int _sockfd;
    };
