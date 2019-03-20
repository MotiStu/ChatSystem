#include<iostream>
#include"chatclient.hpp"

static void usage(std::string proc)
{
	std::cout<<"usage:"<<proc<<"peer_ip" <<std::endl;
}
static void Menu(int &s)
{
	std::cout<<"*****************************************************************"<<std::endl;
	std::cout<<"**1.Register                                           2.Login***"<<std::endl;
	std::cout<<"**                            3.Exit                          ***"<<std::endl;
	std::cout<<"*****************************************************************"<<std::endl;
	std::cout<<"please select:>";
	std::cin>>s;
}	
//对外暴漏一个ip
int main(int argc,char*argv[])
{
	if(argc!=2)
	{
		usage(argv[0]);
		exit(1);
	}
	chatclient *cp=new chatclient(argv[1]);
	cp->initclient();
 	//cp->start();
	int select=0;
	while(1)
	{
		Menu(select);
		switch(select)
		{	
			case 1://注册
				cp->Register();
				break;
			case 2://登陆
				if(cp->Login())
				{
					cp->Chat();
				}
				break;
			case 3:
				exit(0);
				break;
			default:
				exit(1);
				break;
		}
	//	chatclient *cp=new chatclient(argv[1]);
		//cp->initclient();
		///if(cp->connectserver())
		//{	
		//	std::cout<<"connect success" <<std::endl;
		//}
		//return 0;
	}
}
