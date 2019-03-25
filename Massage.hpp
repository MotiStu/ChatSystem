#pragma once
#include<iostream>
#include<string>
#include"protoclutil.hpp"
#include"json/json.h"
#define LOGIN_TYPE 1
#define NORMAL_TYPE 0

class Message
{
public:
	std::string nick_name;
	unsigned int id;
	std::string school;
	std::string text;
	unsigned int type;
public:
	Message(){}
	Message(const  std::string &n_, const std::string &s_, const std::string &t_,const  unsigned int &id_,const unsigned int type_=NORMAL_TYPE)
		:nick_name(n_)
		,school(s_)
		,text(t_)
		,id(id_)
		,type(type_)
	{}
	void  tosendstring(std::string &sendstring)
	{
		Json::Value root;
		root["name"]=nick_name;
		root["school"]=school;
		root["text"]=text;
		root["id"]=id;
		root["type"]=type;
		util::seralizer(root,sendstring);
		
	}
	void torecvvalue(std::string &recvstring)
	{
		Json::Value root;
		util::unseralizer(recvstring,root);
		nick_name=root["name"].asString();
		school=root["school"].asString();
		text=root["text"].asString();
		id=root["id"].asInt();
		type=root["type"].asInt();
	}
	const std::string &Nick_name()
	{
		return nick_name;
	}
	const std::string &School()
	{
		return school;
	}
	const std::string &Text()
	{
		return text;
	}
	const unsigned int &Id()
	{
		return id;
	}
	const unsigned int &Type()
	{
		return type;
	}
	~Message(){}

};
