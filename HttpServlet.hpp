//
// Created by 이태형 on 2023-11-15.
//

#ifndef ASSIGNMENT2B_HTTPSERVLET_HPP
#define ASSIGNMENT2B_HTTPSERVLET_HPP

#pragma once

class HttpServlet {

protected:

    void doGet(HttpRequest &request, HttpResponse &response) = 0;

    void doPost(HttpRequest &request, HttpResponse &response) = 0;
};

#endif //ASSIGNMENT2B_HTTPSERVLET_HPP
