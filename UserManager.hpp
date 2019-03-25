#pragma once
#include"chatserver.hpp"
#include"chatclient.hpp"
#include<iostream>
#include<string>
#include<unordered_map>
class User//用户的属性
{	
private:
	std::string nick_name;
	std::string school;
	std::string passwd;
public:
	User(){}
	User(const std::string& nick_name_,const std::string &school_,const std::string& passwd_)
		:nick_name(nick_name_)
		,school(school_)
		,passwd(passwd_)
	{}
	bool ispasswdok(const std::string &passwd_)
	{	
		return passwd==passwd_?true:false;
	}
	std::string &getnick_name()
	{
		return nick_name;
	}
	std::string &getschool()
	{
		return school;
	}
	~User(){}
};
class usermanager//用户
{
private://把用户存起来    用什么容器？
	unsigned assgin_id;
	std::unordered_map<unsigned int ,User> users;//全部的人
	std::unordered_map<unsigned int ,struct sockaddr_in>online_users; //在线列表  消费线程用 ，给所有的在线用户发送
	pthread_mutex_t lock;
	void Lock()
	{
		pthread_mutex_lock(&lock);
	}
	void Unlock()
	{
		pthread_mutex_unlock(&lock);
	}
public:
	usermanager():assgin_id(10000)
	{	
		pthread_mutex_init(&lock,NULL);
	}
	unsigned int Insert(const std::string &n_,const std::string &s_,const std::string &p_)
	{
		Lock();
		unsigned int id =assgin_id++;
		User u(n_,s_,p_);
		//找人
		if(users.find(id)==users.end())
		{
			users.insert({id,u});
			Unlock();
			return id;
			
			//std::cout<<id<<std::endl;
		}
		Unlock();
		return 1;//注册错误状态是1
	}
	void AddOnlineuser (const unsigned int &id,struct sockaddr_in&peer)
	{
		Lock();
		auto it=online_users.find(id);
		if(it==online_users.end())
		{
			online_users.insert({id,peer});
		}	
		Unlock();
	}
	void getuserinfo(const unsigned int &id,std::string &name_,std::string &school_)
	{
		Lock();	
		name_=users[id].getnick_name();
		school_=users[id].getschool();
		Unlock();
	}
	unsigned int  Check(unsigned int id,const std::string passwd)
	{
		Lock();
		auto it=users.find(id);
		if(it!=users.end())
		{
			User &u=it->second;
			if(u.ispasswdok(passwd))
			{
				Unlock();
				return id;
			}
		}	
		Unlock();
		return 2;
	}
	std::unordered_map<unsigned int ,struct sockaddr_in>onlineuser()
	{
		Lock();
		auto online=online_users;
		Unlock();
		return online;
	}
	~usermanager()
	{
		pthread_mutex_destroy(&lock);
	}
};
