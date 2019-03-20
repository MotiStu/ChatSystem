
#pragma once
#include"datapool.hpp"
#include<iostream>
#include<string>
#include<pthread.h>
#include<unistd.h>
#include"log.hpp"
#include<sys/socket.h>
#include"Message.hpp"
#include"datapool.hpp"
#include"protoclutil.hpp"
#include"usermanager.hpp"
class chatserver;
class param//为了传参
{
	public:
		chatserver* sp;
		int sock;

		std::string ip;
		int port;
	public:
		param(chatserver*sp_,int &sock_,const std::string &ip_,const int &port_)
			:sp(sp_)
			 ,sock(sock_)
			 ,ip(ip_)
			 ,port(port_)
	{}
		~param()
		{}
};
class chatserver
{
	private:
		int tcp_listen_sock;
		int tcp_port;
		int udp_work_sock;
		int udp_port;
		usermanager um;//这是一个用户
		datapool pool;
	public:
		chatserver(int tcp_port_=8080,int udp_port_=8888):
			tcp_port(tcp_port_),
			tcp_listen_sock(-1),
			udp_work_sock(-1),
			udp_port(udp_port_)
	{}
		void initserver()//chuangjain  2sock
		{
			tcp_listen_sock=socketapi::Socket(SOCK_STREAM);
			socketapi::Bind(tcp_listen_sock,tcp_port);
			udp_work_sock=socketapi::Socket(SOCK_DGRAM);
			socketapi::Bind(udp_work_sock,udp_port);

			//std::cout<<"init"<<tcp_port<<std::endl;
			socketapi::Listen(tcp_listen_sock);
			//chuangjian sange xiancheng
			//std::cout<<"init"<<tcp_port<<std::endl;
		}
		unsigned int registeruser(const std::string&name,const std::string &school,const std::string &passwd)
		{
			return um.Insert(name,school,passwd);
		}
		unsigned int loginuser(const int &id, const std::string&passwd,const std::string &ip,int port)
		{
			//std::cout<<"id"<<id<<std::endl;
			unsigned int result=um.Check(id,passwd);
			//std::cout<<result<<endl;
			//只有登陆成功的时候才能上线  添加online列表中
			if(result>=10000)
			{

				//		um.movetoonline(id,ip,port);
			}	
			return result;//聊天用的是udp，
			//客户的端口号完全是随机的   传进来很尴尬
			//后续 确定清楚   port需要二次调整   
		}
		//UDP  协议
		void Product()
		{
			std::string message;
			struct sockaddr_in peer;
			util::recvmessage(udp_work_sock,message,peer);
			std::cout<<"debug:recv message:"<<message<<std::endl;
			if(!message.empty())
			{		
				Message m;
				m.torecvvalue(message);	
				if(m.type==LOGIN_TYPE)
				{
					um.AddOnlineuser(m.Id(),peer);//在登陆的时候把消息插进去
					std::string name_;
					std::string school_;
					um.getuserinfo(m.Id(),name_,school_);//id  name  school  text   type都有
					Message new_msg(name_,school_,m.text,m.Id(),m.Type());
					new_msg.tosendstring(message);
				}
				pool.putmessage(message);
			}
			util::recvmessage(udp_work_sock,message,peer);
			pool.putmessage(message);//完成一次生产
		}	
		void Consume()
		{
			std::string message;
			pool.getmessage(message);//完成一次消费
			std::cout<<"debug:send message:"<<message<<std::endl;
			auto online=um.onlineuser();		
			for(auto it=online.begin();it!=online.end();it++)
			{
				util::sendmessage(udp_work_sock,message,it->second);
			}
		}
		static void *handlerrequest(void *arg)//不需要this指针
		{
			param *p=(param*)arg;
			int sock=p->sock;
			chatserver*sp=p->sp;
			std::string ip=p->ip;
			int port=p->port;
			delete p;
			//std:: <<port<<std::string;
			pthread_detach(pthread_self());//线程分离
			
			Request rq;
			Json::Value root;
			//LOG(rq.text,NORMAL);
			util::recvrequest(sock,rq);//收到
			util::unseralizer(rq.text,root);//////////////////先收到在反序列化。
			LOG(rq.text,NORMAL);
			if(rq.method=="REGISTER")//判断
			{
				std::string name=root["name"].asString();
				std::string school=root["school"].asString();
				std::string passwd =root["passwd"].asString();
				unsigned int id=sp->registeruser(name,school,passwd);
				//注册好了 给被人一个响应
			//	std::cout<<"re   "<<id<<std::endl;
				send(sock,(void *)&id,sizeof(id),0);//注意send的第二个参数
			}
			else if(rq.method=="LOGIN")

			{

				unsigned int id=root["id"].asInt();//虽然客户端没有写id，但是它由 服务端提供了id
				std::string passwd=root["passwd"].asString();//证明登陆  在在线用户中即可
				unsigned int result=sp->loginuser(id,passwd,ip,port);//登陆验证            用户到online列表
			//	std::cout<<result<<std::endl;
				send(sock,&result,sizeof(result),0);
			}
			else
			{

				//logout
			}	
			//recv:sock
			//协议定制  分析和处理
			//响应 
			//短链接   注册完  关闭   登陆完 关闭  注册完之后，登陆的连接都会由，如果很多个人注册，注册完之后没由登陆，就会有很多个线程。
			close(sock);

		}
		void start()
		{
			//pthread_t tid;
			//pthread_create(&tid,usermanager,)
			//pthread_create(&tid,usermanager,)
			//pthread_create(&tid,usermanager,)
			std::string ip;
			int port;
			for(;;)
			{
				int sock=socketapi::Accept(tcp_listen_sock,ip,port);
				//	std::cout<<tcp_port<<std::endl;
				if(sock>0)
				{
					std::cout<< "get a new client"<< ip <<"  "<<"port: "<<port<<std::endl;	
					param *p=new param(this,sock,ip,port);
					pthread_t tid;
					pthread_create(&tid,NULL,handlerrequest,p);

				}
			}
		}
		~chatserver()
		{}

};
