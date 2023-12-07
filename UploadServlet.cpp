#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <regex>
#include <unistd.h>
#include <sstream>
#include <sys/stat.h> // For mkdir
#include <dirent.h>   // For DIR, opendir, readdir, closedir
#include <algorithm>  // For std::sort
#include <cstring>    // For strerror
#include <errno.h>    // For errno

const int bufferSize = 2048;
char buf[bufferSize];
int bytesRead;
std::string Data;
int count = 0;

class UploadServlet {
public:

    void *doGet(int client_socket) {
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

    // Function to split a string by a delimiter into a vector
    std::vector<std::string> split(const std::string &s, const std::string &delimiter) {
        std::vector<std::string> tokens;
        size_t start = 0;
        size_t end = s.find(delimiter);
        while (end != std::string::npos) {
            tokens.push_back(s.substr(start, end - start));
            start = end + delimiter.length();
            end = s.find(delimiter, start);
        }
        tokens.push_back(s.substr(start, end));
        return tokens;
    }

// Function to trim whitespace from both ends of a string
    std::string trim(const std::string& line) {
        const char* whiteSpace = " \t\n\r\f\v";
        size_t start = line.find_first_not_of(whiteSpace);
        size_t end = line.find_last_not_of(whiteSpace);
        return start == std::string::npos ? "" : line.substr(start, end - start + 1);
    }

// Function to save the file
    void saveFile(const std::string& content, const std::string& filename) {
        // Define the directory where files will be saved
        const std::string directory = "./image/";

        // Create the directory if it doesn't exist
        if (mkdir(directory.c_str(), 0777) == -1) {
            if (errno != EEXIST) {
                perror("Error creating directory");
                return; // Don't proceed if directory can't be created
            }
        }

        // Create the full file path
        std::string fullPath = directory + filename;

        // Create and open a text file
        std::ofstream MyFile(fullPath);

        // Check if the file is open
        if (!MyFile.is_open()) {
            perror("Error opening file");
            return;
        }

        // Write to the file
        MyFile << content;

        // Close the file
        MyFile.close();
    }

    void sendDirectoryListing(int client_socket) {
        std::cout << "Starting to send directory listing..." << std::endl;

        const std::string directory = "./image/";
        DIR *dir;
        struct dirent *ent;
        std::vector<std::string> files;

        // Open directory
        if ((dir = opendir(directory.c_str())) != nullptr) {
            // Read directory entries
            std::cout << "Reading directory entries..." << std::endl;
            while ((ent = readdir(dir)) != nullptr) {
                std::string file_name = ent->d_name;
                if (file_name != "." && file_name != "..") {
                    files.push_back(file_name);
                }
            }
            closedir(dir);
        } else {
            perror("Could not open directory");
            return;
        }

        // Sort file names
        std::sort(files.begin(), files.end());
        std::cout << "Directory entries sorted." << std::endl;

        // Build HTML content
        std::string htmlContent = "<!DOCTYPE html><html><body>";
        htmlContent += "<h2>Uploaded Files</h2><ul>";
        for (const auto &file_name : files) {
            htmlContent += "<li>" + file_name + "</li>";
        }
        htmlContent += "</ul></body></html>";

        std::cout << "HTML content built." << std::endl;

        // Prepare HTTP response
        std::string httpResponse = "HTTP/1.1 200 OK\r\n"
                                   "Content-Type: text/html; charset=UTF-8\r\n"
                                   "Content-Length: " + std::to_string(htmlContent.length()) + "\r\n\r\n" +
                                   htmlContent;

        // Send the response
        if (send(client_socket, httpResponse.c_str(), httpResponse.length(), 0) == -1) {
            perror("Error sending directory listing");
        } else {
            std::cout << "Directory listing sent successfully." << std::endl;
        }
    }

// Main parsing function
    void parseMultipartFormData(const std::string& data) {
        std::string boundary = "------WebKitFormBoundary"; // You'll need to extract this from the Content-Type header
        std::string delimiter = boundary;
        std::vector<std::string> parts = split(data, delimiter);

        std::string caption, date, filename, fileContent;
        std::regex contentDispositionRegex("Content-Disposition: form-data; name=\"([^\"]+)\"(?:; filename=\"([^\"]+)\")?");
        std::regex contentTypeRegex("Content-Type: (.*)");

        for (auto& part : parts) {
            std::istringstream partStream(part);
            std::string line, disposition, contentType;
            std::smatch matches;

            // Process headers
            while (std::getline(partStream, line) && line != "\r") {
                if (std::regex_search(line, matches, contentDispositionRegex)) {
                    disposition = matches[1];
                    if (matches.size() == 3) {
                        filename = matches[2];
                    }
                } else if (std::regex_search(line, matches, contentTypeRegex)) {
                    contentType = matches[1];
                }
            }

            // Process content
            if (!filename.empty()) {
                // File content, read the rest of the stream
                std::ostringstream contentStream;
                contentStream << partStream.rdbuf();
                fileContent = contentStream.str();
                // Remove the last CRLF from the content
                if (!fileContent.empty()) {
                    fileContent.erase(fileContent.size() - 2);
                }
            } else if (!disposition.empty()) {
                // Field value, read the next line
                std::string value;
                std::getline(partStream, value);
                value = trim(value); // Remove the CRLF at the end
                if (disposition == "caption") {
                    caption = value;
                } else if (disposition == "date") {
                    date = value;
                }
            }
        }

        std::string fileType = getFileExtension(filename);

        // Build new filename and save the file
        std::string newFilename = caption + "_" + date + fileType;
        saveFile(fileContent, newFilename);
    }

    std::string getFileExtension(const std::string& filename) {
        size_t dotPos = filename.find_last_of('.');
        if (dotPos != std::string::npos && dotPos < filename.length()) {
            return filename.substr(dotPos); // get the substring after the last dot
        }
        return std::string(); // return empty string if no dot is found or it is the last character
    }

    void *doPost(int socket) {
        std::cout << "Read buffer called in Upload Server " << std::endl;

        while ((bytesRead = read(socket, buf, bufferSize)) > 0) {

            Data += std::string(buf, bytesRead);

            std::string bufferString(buf, bytesRead);

            if (bufferString.find("--") != std::string::npos) {
                count++;
                if (count == 2) {
                    break;
                }
            }
        }

        std::cout << Data << std::endl;

        parseMultipartFormData(Data);
        sendDirectoryListing(socket);
        close(socket);
        return nullptr;
    }
};
