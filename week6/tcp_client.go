package main

import (
	"fmt"
	"net"
	"os"
)

func main() {
	//发起连接请求
	conn, err := net.Dial("tcp", "127.0.0.1:7777")
	if err != nil {
		fmt.Println("net.dial err:", err)
		return
	}
	defer conn.Close()
	//获取用户的键盘输入，将输入数据发送给服务器
	go func() {
		str := make([]byte, 4096)
		for {
			n, err := os.Stdin.Read(str)
			if err != nil {
				fmt.Println("os.Stdin.Read err:", err)
				continue
			}
			//写给服务器,读多少，写多少
			conn.Write(str[:n])
		}
	}()
	//打印服务器回发的大写数据
	buf := make([]byte, 4096)
	for {
		n, err := conn.Read(buf)
		if n == 0 {
			fmt.Println("检测到连接已断开，退出客户端")
			return
		}
		if err != nil {
			fmt.Println("conn.Read err:", err)
			return
		}
		fmt.Println("客户端收到服务器回复：", string(buf[:n]))
	}
}
