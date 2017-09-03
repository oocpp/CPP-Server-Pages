//
// Created by lg on 17-8-22.
//

#pragma once

#include <vector>
#include <sys/epoll.h>
#include <unistd.h>
#include<cassert>
#include "DebugPrint.h"
#include<string>

//EPOLLIN ：表示对应的文件描述符可以读（包括对端SOCKET正常关闭）；
//EPOLLOUT：表示对应的文件描述符可以写；
//EPOLLPRI：表示对应的文件描述符有紧急的数据可读（这里应该表示有带外数据到来）；
//EPOLLERR：表示对应的文件描述符发生错误；
//EPOLLHUP：表示对应的文件描述符被挂断；
//EPOLLET： 将EPOLL设为边缘触发(Edge Triggered)模式，这是相对于水平触发(Level Triggered)来说的。
//EPOLLONESHOT：只监听一次事件，当监听完这次事件之后，如果还需要继续监听这个socket的话，需要再次把这个socket加入到EPOLL队列里

    class Epoll final{
    public:

        typedef uint32_t EventType;
        static constexpr EventType NONE=0;
        static constexpr EventType READ=EPOLLIN|EPOLLPRI;
        static constexpr EventType WRITE=EPOLLOUT;

        Epoll(std::string s=std::string()) :_epollfd(::epoll_create1(EPOLL_CLOEXEC)),name(s)
        {
            if(_epollfd<0){
                dout<<"epoll 失败";
            }
        }

        ~Epoll(){
            ::close(_epollfd);
        }

        void eventAdd(int fd,EventType et,void*ptr=nullptr){
            assert(fd>=0);
            assert(et==NONE||et==READ||et==WRITE);

            epoll_event event;
            event.events=et;
            event.data.ptr=ptr;

            if (::epoll_ctl(_epollfd, EPOLL_CTL_ADD,fd, &event) < 0)
            {
                dout << "epoll_ctl op =" << "EPOLL_CTL_DEL"<< " fd =" << fd;
            }
        }

        void eventDel(int fd){
            //assert(fd>=0);
            if (::epoll_ctl(_epollfd, EPOLL_CTL_DEL,fd, nullptr) < 0)
            {
                dout << "epoll_ctl op =" << "EPOLL_CTL_DEL"<< " fd =" << fd;
            }
        }

        void eventUpdate(int fd,EventType et,void*ptr= nullptr){
            assert(fd>=0);
            assert(et==NONE||et==READ||et==WRITE);

            epoll_event event;
            event.events=et;
            event.data.ptr=ptr;

            if (::epoll_ctl(_epollfd, EPOLL_CTL_MOD, fd, &event) < 0)
            {
                dout << "epoll_ctl op =" << "EPOLL_CTL_DEL"<< " fd =" << fd;
            }
        }

        bool wait(int timeoutMs,std::vector<epoll_event> & activeEvents) {
            int numEvents = ::epoll_wait(_epollfd, activeEvents.data(), static_cast<int>(activeEvents.size()), timeoutMs);

            if (numEvents == 0) {
                dout << "nothing happended";
            }
            else if(numEvents<0){
                if (errno != EINTR) {
                    dout <<numEvents<<name<< " poll()";
                }
                numEvents=0;
            }

            activeEvents.resize(numEvents);
            return numEvents > 0;
        }

        bool waitAccept(int timeout){
            epoll_event event;
            int numEvents = ::epoll_wait(_epollfd, &event, 1, timeout);


            if (numEvents == 0) {
               // dout << "nothing happended";
            }
            else if(numEvents<0){
                if (errno != EINTR) {
                    dout <<numEvents<<name<<  "  accept::poll()";
                }

                numEvents=0;
            }

            return numEvents>0;
        }

    private:
        int _epollfd;
        std::string name;
    };