#pragma once
#include<iostream>
#include<vector>
// #include"Message.hpp"
#include<string>
#include"Window.hpp"
#include"protoclutil.hpp"
#include"Message.hpp"
// #include"usermanager.hpp"
#define TCP_PORT 8080
#define UDP_PORT 8888
// 功能  注册 登陆  发消息 收消息
// 必须有套接字满足网络需求

class chatclient;
struct parampair{
	window *wp;
	chatclient *Cp;	
};



class chatclient
{
private:
// 网络信息
	int tcp_sock;
	int udp_sock;
	std::string peer_ip;//需要连谁

// 用户信息
	std::string passwd;
	struct sockaddr_in server;
public:
	std::string nick_name;
	std::string school;
	unsigned int id;
public:
	chatclient(std::string ip_)
		:peer_ip(ip_)
	{	
		id=0;
		tcp_sock=-1;
		udp_sock=-1;
		server.sin_family=AF_INET;
		server.sin_port=htons(UDP_PORT);
		server.sin_addr.s_addr=inet_addr(peer_ip.c_str());			
	}
	void initclient()
	{
               // tcp_sock=socketapi::Socket(SOCK_STREAM);
		udp_sock=socketapi::Socket(SOCK_DGRAM);
		// 三个接口
		// sendto connect在server和client中都要用
	}
	bool connectserver()
	{
                tcp_sock=socketapi::Socket(SOCK_STREAM);
		return socketapi::Connect(tcp_sock,peer_ip,TCP_PORT);
	}
	bool Register()//注册
	{
		// 连接
		if(connectserver()&&(util::registerenter(nick_name,school,passwd)))
		{
			Request rq;
			rq.method="REGISTER\n";
			Json::Value root;
			root["name"]=nick_name;
			root["school"]=school;
			root["passwd"]=passwd;
			util::seralizer(root,rq.text);//反序列化
			
		   	rq.content_length="content_length: ";
			rq.content_length+=util::inttostring((rq.text).size());
			rq.content_length+="\n";
			util::sendrequest(tcp_sock,rq);//服务器创建线程  客户端构建rq  服务器收到 rq 两个一样  因为用的是我们自己的协议   
			recv(tcp_sock,&id,sizeof(id),0);//收到了  判断完毕  通信完毕了吗？？？：
			bool res=false;
			if(id>=10000)
			{
				std::cout<<"resgister success!!!!you login id id :"<<id<<std::endl;
				res=true;
			}
			else 
			{
				std::cout<< "register failed!!!!  code is" <<id<<std::endl;
			}
			close(tcp_sock);
			return res;
		}	
	}
	bool Login()
	{	
	// loginenter(int &id,std::string &passwd)i
		if(connectserver()&&(util::loginenter(id,passwd)))
		{
			Request rq;
			rq.method="LOGIN\n";
			Json::Value root;
			root["id"]=id;
			root["passwd"]=passwd;//下线的时候  检查是否上线
			util::seralizer(root,rq.text);//反序列化
			
			rq.content_length="content_length: ";
			rq.content_length+=util::inttostring((rq.text).size());
			rq.content_length+="\n";


			util::sendrequest(tcp_sock,rq);//服务器创建线程  客户端构建rq  服务器收到 rq 两个一样  因为用的是我们自己的协议   
			unsigned int result =0;
			bool res=false;
			// std::cout<<result<<std::endl;
			recv(tcp_sock,&result,sizeof(result),0);//收到了  判断完毕  通信完毕了吗？？？：
			std::cout<<result<<std::endl;
			if(result >=10000)
			{
				res=true;
				std::string name_="none";
				std::string school_="none";
				std::string text_="I am  login!Talk With me......";
				unsigned int type_ =LOGIN_TYPE;
				unsigned int id=result;
				Message  m(name_,school_,text_,id,type_);
				std::string sendstring;
				m.tosendstring(sendstring);
				udpSend(sendstring);

				std::cout<<"login success!!!!"<<std::endl;
			}
			else 
			{
				std::cout<< "login failed!!!!  code is" <<result<<": "<<std::endl;///浏览器 的  404
			}
			close(tcp_sock);
			return res;
		}	
	}
	void udpSend(const std::string &message)
	{
		// struct sockaddr_in peer;
		// peer.sin_family=AF_INET;
		// peer.sin_port=htons(UDP_SORT);
		// peer.sin_addr.s_addr=inet_s
		util::sendmessage(udp_sock,message,server);
	}
	void udprecv( std::string &message)
	{
		struct sockaddr_in peer;	
		util::recvmessage(udp_sock,message,peer);
	}
	static void *Welcome(void*arg)
	{
		pthread_detach(pthread_self());
		window*wp=(window*) arg;
		wp->welcome();
	}
	static void *INPUT(void*arg)
	{
		pthread_detach(pthread_self());
		struct parampair *pptr=(struct parampair*)arg;
		window *wp=pptr->wp;
		chatclient *cp=pptr->Cp;
		wp->drawinput();
		
		std::string text;
		for(;;)
		{
			wp->getstringfrominput(text);
			Message msg(cp->nick_name,cp->school,text,cp->id);

			std::string sendstring;
			msg.tosendstring(sendstring);
			cp->udpSend(sendstring);
		}
	}		
	void Chat()
	{
		window w;
		pthread_t h,m,l;
		struct parampair pp={&w,this};
		pthread_create(&h,NULL,Welcome,&w);	
		pthread_create(&l,NULL,INPUT,&pp);
		w.drawoutput();
		w.drawonline();	
		std::string recvstring;
		std::string showstring;
		std::vector <std::string> online;
		for(;;)
		{
			Message msg;
			udprecv(recvstring);
			msg.torecvvalue(recvstring);
			if(msg.Id()==id&&msg.Type()==LOGIN_TYPE)
			{
				nick_name=msg.Nick_name();
				school=msg.School();
			}
			showstring=msg.Nick_name();
			showstring+="-";
			showstring+=msg.School();
			std::string f=showstring;
			util::addonline(online,f);//增加在线列表
			showstring+="#";
			showstring+=msg.Text();
			w.putmessagetooutput(showstring);
			w.putusertoonline(online);
		}	
/*		Message msg;
		std::string n_;
		std::string s_;
		std::cout<<"please enter your name:";
		std::cin>>n_;
		std::string text_;
		std::cout<<"please enter your school:";
		std::cin>>s_;
		while(1)
		{
			std::string text_;
			std::cout<<"please enter:";
			std::cin>>text_;
			Message msg(n_,s_,text_,id);	
			std::string sendstring;
			msg.tosendstring(sendstring);
			util::sendmessage(udp_sock,sendstring,server);
			std::string recvstring;
			struct sockaddr_in peer;
			util::recvmessage(udp_sock,recvstring,peer);
			std::cout<<"debug"<<recvstring<<std::endl;
			std::cout<<"debug"<<recvstring<<std::endl;
			msg.torecvvalue(recvstring);		
			std::cout<<"name:"<<msg.Nick_name()<<std::endl;
			std::cout<<"school:"<<msg.School()<<std::endl;
			std::cout<<"text:"<<msg.Text()<<std::endl;
			std::cout<<"id:"<<msg.Id()<<std::endl;
		
*/			
			
		// }

	}
	~chatclient()
	{}
};




















