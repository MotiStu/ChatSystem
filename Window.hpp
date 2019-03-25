#pragma once
#include<unistd.h>
#include<vector>
#include<cstring>
#include<iostream>
#include<ncurses.h>
#include<pthread.h>
#include<curses.h>
class window
{

	public:
		WINDOW *header;
		WINDOW *output;
		WINDOW *online;
		WINDOW *input;
		pthread_mutex_t lock;
	public:
		window()
		{
			initscr();	
			curs_set(0);
			pthread_mutex_init(&lock,NULL);		
		}
		void safefresh(WINDOW* w)
		{
			pthread_mutex_lock(&lock);
			wrefresh(w);
			pthread_mutex_unlock(&lock);	
		}

		void drawheader()
		{
			int h=LINES*0.2,w=COLS,y=0,x=0;
			header=newwin(h,w,y,x);
			box(header,0,0);
			safefresh(header);
		}
		void drawoutput()
		{
			int h=LINES*0.6,w=COLS*0.75,y=LINES*0.2,x=0;
			output=newwin(h,w,y,x);
			box(output,0,0);
			safefresh(output);
		}
		void drawonline()
		{
			int h=LINES*0.6,w=COLS*0.25,y=LINES*0.2,x=COLS*0.75;
			online=newwin(h,w,y,x);
			box(online,0,0);
			safefresh(online);
		}
		void drawinput()
		{
			int h=LINES*0.2,w=COLS,y=LINES*0.8,x=0;
			input=newwin(h,w,y,x);
			box(input,0,0);
			const std::string tips="please enter:#   ";

			PUTstringinwin(input,2,2,tips);
			safefresh(input);//刷新窗口
			
		}
		void getstringfrominput(std::string &message)
		{
			char buff[1024];
			memset(buff,0,sizeof(buff));
			wgetnstr(input,buff,sizeof(buff));
			message=buff;
			delwin(input);
			drawinput();
		}
		void putmessagetooutput(const std::string &message)
		{
			static int line=1;
			int y,x;
			getmaxyx(output,y,x);
			if(line>y-2)
			{
				delwin(output);
				drawoutput();	
				line=1;
			}
			PUTstringinwin(output,line++,2,message);
		}
		void PUTstringinwin(WINDOW* win_,int y_,int x_,const std::string &string_)

		{
			mvwaddstr(win_, y_, x_, string_.c_str());         
			safefresh(win_); //¸¶]            pthread_mutex_unlock(&lock);        
		}
		void putusertoonline(std::vector<std::string>&online_user)
		{
			int size=online_user.size();
			for(auto i=0;i<size;i++)
			{	
				PUTstringinwin(online,i+1,2,online_user[i]);
			}	
		}
		void welcome()
		{	
			const std::string welcome = "welcome to my chat system!!!";
			int num = 1;
			int x,y;
			int dir = 0;
			for (;;)
			{
				drawheader();
				getmaxyx(header, y, x);//得到header的高度和宽度
				PUTstringinwin(header, y/2, num, welcome);
				if(num>(x-welcome.size()-3))
					dir=1;
				if(num<=1)
					dir=0;
				if(dir==0)
					num++;
				else
					num--;
				usleep(50000);
			}
		}
		~window()
		{
			endwin();
			pthread_mutex_destroy(&lock);
		}
};



