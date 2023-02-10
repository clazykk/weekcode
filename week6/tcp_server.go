package main

import (
	"fmt"
	"net"
	"strings"
)

func handleConnect(conn net.Conn) {
	defer conn.Close()
	//获取连接的客户端 addr
	addr := conn.RemoteAddr()
	fmt.Println(addr, "客户端成功连接！")
	//读取客户端发送数据
	buf := make([]byte, 4096)
	for {
		n, err := conn.Read(buf)
		if "exit\n" == string(buf[:n]) || "exit\r\n" == string(buf[:n]) {
			fmt.Println("服务器接收到客户端的退出请求，断开连接！！！")
			return
		}
		if n == 0 {
			fmt.Println("服务器检测到客户端关闭，断开连接！！！")
			return
		}
		if err != nil {
			fmt.Println("conn.Read err:", err)
			return
		}
		fmt.Println("服务器读到数据：", string(buf[:n]))
		//小写转大写，回发给客户端
		conn.Write([]byte(strings.ToUpper(string(buf[:n]))))
	}
}

func main() {
	//创建监听套接字
	listener, err := net.Listen("tcp", "127.0.0.1:7777")
	if err != nil {
		fmt.Println("listen err:", err)
		return
	}
	defer listener.Close()
	//监听客户端连接请求
	for {
		fmt.Println("服务器等待客户端连接...")
		conn, err := listener.Accept()
		if err != nil {
			fmt.Println("Accept err:", err)
			return
		}

		go handleConnect(conn)
	}

}
