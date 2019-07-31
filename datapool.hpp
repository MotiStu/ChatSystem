//这是数据池的接口
//生产者消费者模型向数据池中写数据和读数据。所谓的数据池也就是一个环形队列而已。 
#pragma once 
#include <iostream>
#include <string>
#include <vector> 
#include <semaphore.h>
#define MESSAGE_POOL_SIZE 512
class datapool
{
	private:
		const int pool_size;   
		std::vector<std::string> pool;     
		sem_t product_sem;  //产品信号量  
		sem_t consume_sem;  //消费信号量
		int p_step;   
		int c_step;   
	public: 
		datapool(int size_ = MESSAGE_POOL_SIZE	)
			:pool_size(size_)
			 ,pool(size_)      
		{
			p_step = c_step = 0;    
			sem_init(&product_sem, 0, size_);   
			sem_init(&consume_sem, 0, 0);      
		}      
		bool getmessage(std::string &message_) //获取消息   
		{ 
			sem_wait(&consume_sem);   //消费sem-- 
			message_ = pool[c_step];   
			sem_post(&product_sem);     
			c_step++;    
			c_step %= pool_size;   
		}      
		bool putmessage(const std::string &message_)    //放消息
		{      
			sem_wait(&product_sem);    
			pool[p_step] = message_;       
			sem_post(&consume_sem);   
			p_step++;    
			p_step %= pool_size;   
		}        
		~datapool()
		{     
			sem_destroy(&product_sem);     
			sem_destroy(&consume_sem);    
		} 
}; 
