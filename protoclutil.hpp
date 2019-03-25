#pragma once 
#include<sstream>
#include<vector>
#include<iostream>
#include<string>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdlib.h>
#include"log.hpp"
#include"json/json.h"
#include<sys/socket.h>
#define backlog 5
#define message_size 1024
class Request
{
public:
	std::string method;//方法三个     注册 登陆  
	std::string content_length;//"content-length:89"
	std::string blank;//空格
	std::string text;//正文
	//怎么接收？？  按行接收    
public:
	Request():blank("\n"){}
	~Request(){}
};
class util
{
public:
	static bool registerenter(std::string &n_,std::string &s_,std::string &passwd)
	{
		std::cout<<"please enter nick name:";
                std::cin>>n_;
                std::cout<<"please enter nick school:";
                std::cin>>s_;
                std::cout<<"please enter nick passwd:";
                std::cin>>passwd;
                std::string again;
                std::cout<<"please enter nick passwd again:";
                std::cin>>again;	
		if(again==passwd)
		{
			return true;
		}
		return false;
	}
	static bool loginenter(unsigned int &id,std::string &passwd)
	{
		std::cout<<"please enter your id:";
		std::cin>>id;
		std::cout<<"please enter your passwd:";
		std::cin>>passwd;
		return true;
	}	
	static void seralizer(Json::Value&root,std::string &outstring)//序列化
	{
		Json::FastWriter w;
		outstring=w.write(root);
	}
	static void unseralizer(std::string &instring,Json::Value &root)//反序列化
	{
		Json::Reader r;
		r.parse(instring,root,false);
	}
	static std::string inttostring(int x)
	{
		std::stringstream ss;
		ss<<x;
		return ss.str();
	}
	static int stringtoint(std::string &str)//str转int
	{
		std::stringstream ss(str);
		int x;
		ss>>x;
		return x;
	}
	static void recvoneline(int sock,std::string &outstring)
	{
		char c='x';
		while(c!='\n')
		{
			ssize_t s=recv(sock,&c,1,0);
			if(s>0)
			{
				if(c=='\n')
				{
					break;
				}
				outstring.push_back(c);
				
			}
			else
			{
				break;
			}
		}	
	}
	static void recvrequest(int &sock,Request&rq)
	{
		recvoneline(sock,rq.method);
		recvoneline(sock,rq.content_length);
		recvoneline(sock,rq.blank);//读正文，ta有多长?
		std::string cl=rq.content_length;//有多长？不能按行读  content_length: 88
		//在content_length: 88
		ssize_t pos=cl.find(": ");
		if(std::string::npos==pos)//没找见  第一个和最后一个相遇
		{
			return ;
		}
		std::string sub=cl.substr(pos+2);
		int size=stringtoint(sub);
		char c;
		for(auto i=0;i<size;i++)
		{
			recv(sock,&c,1,0);//读一个字符
			(rq.text).push_back(c);
		}
		
		//recvoneline(sock,rq.text);		
	}
	static void sendrequest(int sock,Request&rq)
	{

		std::string &m_=rq.method;
		std::string &cl_=rq.content_length;
		std::string &b_=rq.blank;
		std::string &text_=rq.text;
		send(sock,m_.c_str(),m_.size(),0);
		send(sock,cl_.c_str(),cl_.size(),0);
		send(sock,b_.c_str(),b_.size(),0);
		send(sock,text_.c_str(),text_.size(),0);
		
	}
	static void recvmessage(int sock, std::string &message,struct sockaddr_in &peer)
	{
		char msg[message_size];
		socklen_t len=sizeof(peer);
		ssize_t s=recvfrom(sock,msg,sizeof(msg)-1,0,(struct sockaddr*)&peer,&len); 
		if(s<0)
		{
			LOG("recv message error",WARNING);
		}
		else
		{	
			message=msg;
			
		}
	}
	static void sendmessage(int sock,const std::string &message,struct sockaddr_in &peer)
	{	
		sendto(sock,message.c_str(),message.size(),0,(struct sockaddr*)&peer,sizeof(peer));
	}
	static void addonline(std::vector<std::string>&online,std::string&f)
	{
		for(auto it=online.begin();it!=online.end();it++)
		{	
			if(*it==f)
				return ;//没找见
		}
		online.push_back(f);
	}	
};
class socketapi
{
public:

	static int Socket(int type)
	{
       		int sock=socket(AF_INET,type,0);
		if(sock<0){
			LOG("socket error",ERROR);
			exit(2);			
		}
		
	}
	static void Bind(int sock,int port)//
	{
		struct sockaddr_in local;
		local.sin_family=AF_INET;
		local.sin_addr.s_addr=htonl(INADDR_ANY);
		local.sin_port=htons(port);
		if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0)
		{
			LOG("sock error",ERROR);
			exit(3);
		}
	}
	static void Listen(int sock)
	{
		if(listen(sock,backlog)<0)//
		{
			LOG("listen error",ERROR);
			exit(4);
		}
	}
	static int Accept(int listen_sock,std::string &out_ip,int &out_port)
	{
		struct sockaddr_in peer;
		socklen_t len=sizeof(peer);
		int sock=accept(listen_sock,(struct sockaddr*)&peer,&len);
		if(sock<0)
		{
			LOG("accept error",WARNING);
			return -1;
		}
		out_ip=inet_ntoa(peer.sin_addr);//wangluo  ip  4zijie  yao naodao  dianfen shijinzhi
		//	std::cout<<out_port<<std::endl;
		out_port=htons(peer.sin_port);
		//	std::cout<<out_port<<std::endl;
		return sock;
	}
	static bool Connect(const int &sock,std::string peer_ip,const int &port)//连接函数
	{
		struct sockaddr_in peer;
		peer.sin_family=AF_INET;
		peer.sin_addr.s_addr=inet_addr(peer_ip.c_str());//字符换 转成  4字节
		peer.sin_port=htons(port);
		if(connect(sock,(struct sockaddr*)&peer,sizeof(peer))<0)
		{
		//	std::cout<<  <<std::endl;
			LOG("connect error",WARNING);
			return false;
		}
		return true;
	}
	static void Send(int sock,Request &rq)
	{
		std::string &m_=rq.method;
		std::string &cl_=rq.content_length;
		std::string &b_=rq.blank;
		std::string &text_=rq.text;
		send(sock,m_.c_str(),m_.size(),0);
		send(sock,cl_.c_str(),cl_.size(),0);
		send(sock,b_.c_str(),b_.size(),0);
		send(sock,text_.c_str(),text_.size(),0);
	}
};






