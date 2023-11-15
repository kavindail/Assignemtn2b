#include <fstream>
#include <iostream>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

class FormUpload {
public:
  static void *sendTCP(int client_socket) {
    std::cout << "In Form Upload\n";

    std::string htmlContent =
        "<!DOCTYPE html>"
        "<html>"
        "<body>"
        "<p>Please fill out the form below to upload your file.</p>"
        "<form action='/upload' method='post' enctype='multipart/form-data'>"
        "Caption: <input type='text' name='caption'><br><br>"
        "Date: <input type='date' name='date'><br><br>"
        "File: <input type='file' name='file'><br><br>"
        "<input type='submit' value='Upload'>"
        "</form>"
        "</body>"
        "</html>";

    std::string contentLength = std::to_string(htmlContent.length());

    std::string httpResponse = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: text/html; charset=UTF-8\r\n"
                               "Content-Length: " +
                               contentLength + "\r\n" + "\r\n" + htmlContent;

    int send_res =
        send(client_socket, httpResponse.c_str(), httpResponse.length(), 0);
    if (send_res == -1) {
      perror("error while sending form");
    } else {
      std::cout << "form send success\n";
    }
    close(client_socket);
    return nullptr;
  }
};
