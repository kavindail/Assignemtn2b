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
int count = 0;
class UploadServer {
public:
  void *readBuffer(int socket) {
    std::cout << "Read buffer called in Upload Server " << std::endl;

    while ((bytesRead = read(socket, buf, bufferSize)) > 0) {

      data += std::string(buf, bytesRead);

      std::string bufferString(buf, bytesRead);

      if (bufferString.find("--") != std::string::npos) {
        count++;
        if (count == 2) {
          break;
        }
      }
    }

    std::cout << data << std::endl;
    close(socket);
    return nullptr;
  }
};
