#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

const int bufferSize = 2048;
char buf[bufferSize];
int bytesRead;
std::string data;

class UploadServer {
public:
  void *readBuffer(int socket) {
    std::cout << "Read buffer called in Upload Server " << std::endl;

    std::string delimiterPrefix = "------WebKitFormBoundary";

    while ((bytesRead = read(socket, buf, bufferSize)) > 0) {

      if (bytesRead == 0) {
        break;
      }

      data += std::string(buf, bytesRead);
    }

    std::cout << data << std::endl;
    data.clear();

    close(socket);
    return nullptr;
  }
};
