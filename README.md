# ChatSystem
**这是一个在Linux下用C++编写的基于网络通信协议（TCP/UDP/HTTP）的多人聊天系统：**  
这个系统实现了多名用户发送消息到服务器上，服务器可以将消息转发给在线用户。  
同时实现了引入`Jsoncpp`库对于消息的处理（序列化/反序列化）和`ncurses`库对于聊天界面的描绘。  
  
**一：编写聊天系统，用到了以下的专业知识：**    
1.基础的C++语句，各种模板的操作（string、vector、unordered_map）  
2.`Linux`的基础操作命令，编写`Makefile`  
3.对于网络传输层协议的`socket`网络编程  
4.用到了生产者消费者模型，多线程和互斥锁---主要应用在对消息的处理上 
  
**二：该项目的实现模块：**  
1.客户端与服务端模块  
2.服务器后的一个存放数据的pool  
3.网络层通信：基于`HTTP`编写的协议  
4.用户的管理    
5.使用`ncurses`库构建一个简易的聊天界面  
  
**对于聊天中的客户端与服务端来说要实现两点：用户的登录、注册和消息的处理**    
1.使用了TCP实现了用户的登录、注册，因为需要TCP的可靠性来保证用户的管理  
2.使用了UDP实现了消息的接收和发送，UDP是传输层协议的优点传输效率高、速度快  
3.消息放在数据池中，当作临时资源，因为操作系统的收发数据并不是直接从网络当中获取，而是应该有一块空间共操作系统直接操作这些资源，并且建立数据池也解决了有多条消息被发送时不需要一次次的建立连接，浪费时间/资源。但由此引入了多线程的问题，所以加上互斥锁实现了一个生产者消费者模型。  
4.自定义通信协议，类似于HTTP
  
