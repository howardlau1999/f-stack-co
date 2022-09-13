#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <memory>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "f-stack-co/event_loop.h"
#include "f-stack-co/init.h"
#include "f-stack-co/task.h"
#include "f-stack-co/tcp_listener.h"

char html[] = "HTTP/1.1 200 OK\r\n"
              "Server: F-Stack\r\n"
              "Date: Sat, 25 Feb 2017 09:26:33 GMT\r\n"
              "Content-Type: text/html\r\n"
              "Content-Length: 438\r\n"
              "Last-Modified: Tue, 21 Feb 2017 09:44:03 GMT\r\n"
              "Connection: keep-alive\r\n"
              "Accept-Ranges: bytes\r\n"
              "\r\n"
              "<!DOCTYPE html>\r\n"
              "<html>\r\n"
              "<head>\r\n"
              "<title>Welcome to F-Stack!</title>\r\n"
              "<style>\r\n"
              "    body {  \r\n"
              "        width: 35em;\r\n"
              "        margin: 0 auto; \r\n"
              "        font-family: Tahoma, Verdana, Arial, sans-serif;\r\n"
              "    }\r\n"
              "</style>\r\n"
              "</head>\r\n"
              "<body>\r\n"
              "<h1>Welcome to F-Stack!</h1>\r\n"
              "\r\n"
              "<p>For online documentation and support please refer to\r\n"
              "<a href=\"http://F-Stack.org/\">F-Stack.org</a>.<br/>\r\n"
              "\r\n"
              "<p><em>Thank you for using F-Stack.</em></p>\r\n"
              "</body>\r\n"
              "</html>";

fstackco::task<void> handle_http_connection(fstackco::tcp_connection conn) {
  char buf[1024];
  while (true) {
    auto n = co_await conn.read(buf, sizeof(buf));
    if (n <= 0) {
      co_return;
    }
    n = co_await conn.write(html, sizeof(html) - 1);
    if (n <= 0) {
      co_return;
    }
  }
}

fstackco::task<void> http_server(std::shared_ptr<fstackco::event_loop> loop) {
  fstackco::tcp_listener listener(loop, 8080);
  while (true) {
    auto conns = co_await listener.accept();
    for (auto &&conn : conns) {
      handle_http_connection(std::move(conn)).detach();
    }
  }
}

int main(int argc, char *argv[]) {
  fstackco::init(argc, argv);
  auto loop = std::make_shared<fstackco::event_loop>();
  http_server(loop).detach();
  loop->loop();
  return 0;
}