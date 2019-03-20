#include"chatserver.hpp"

//两个端口号   tcp_port udp_port
static void usage(std::string proc)
{
	std::cout<<"usage:"<<proc<<"tcp_port udp_port" <<std::endl;
}
void *runproduct(void *arg)
{
	pthread_detach(pthread_self());
	chatserver *sp=(chatserver*)arg;
	for(;;)
	{
		sp->Product();
	}
}
void *runconsume(void *arg)
{
	pthread_detach(pthread_self());
	chatserver *sp=(chatserver*)arg;
	for(;;)
	{
		sp->Consume();
	}
}
int main(int argc,char*argv[])
{
	if(argc!=3)//传参有问题
	{
		usage(argv[0]);
		exit(1);
	}
	int tcp_port=atoi(argv[1]);
	int udp_port=atoi(argv[2]);
	chatserver*sp=new chatserver(tcp_port,udp_port);
	sp->initserver();
	pthread_t  c,p;
	pthread_create(&p,NULL,runproduct,(void*)sp   );
	pthread_create(&c,NULL,runconsume,(void*) sp   );
	sp->start();
	return 0;
}
	
