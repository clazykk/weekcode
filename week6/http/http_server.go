package main

import (
	"log"
	"net/http"
	"os"
	"os/signal"
)

var server *http.Server

func main() {
	quit := make(chan os.Signal)
	signal.Notify(quit, os.Interrupt)

	mux := http.NewServeMux()
	mux.Handle("/welcome", &myHandler{user: "kk"})
	mux.HandleFunc("/exit", exitServer)
	mux.HandleFunc("/", test)

	server = &http.Server{
		Addr:    ":8080",
		Handler: mux,
	}

	go func() {
		// 接收退出信号
		<-quit
		if err := server.Close(); err != nil {
			log.Fatal("Close server:", err)
		}
	}()

	log.Println("Starting httpserver")
	err := server.ListenAndServe()
	// http.ListenAndServe(":8080", mux)
	if err != nil {
		// 正常退出
		if err == http.ErrServerClosed {
			log.Fatal("Server closed")
		} else {
			log.Fatal("Server closed err:", err)
		}
	}
	log.Fatal("Server exited")

}

type myHandler struct {
	user string
}

func (*myHandler) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	w.Write([]byte("this is http server"))
}

func test(w http.ResponseWriter, r *http.Request) {
	w.Write([]byte("hello world"))
}

func exitServer(w http.ResponseWriter, r *http.Request) {
	err := server.Shutdown(nil)
	if err != nil {
		log.Fatal("shutdown the server err:", err)
	}
}
