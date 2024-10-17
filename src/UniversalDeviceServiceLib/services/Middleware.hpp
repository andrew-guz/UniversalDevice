#pragma once

#include <crow.h>

#include "AccountManager.hpp"
#include "Defines.hpp"

struct Authorization {
    struct context {};

    void before_handle(crow::request& request, crow::response& response, context& context) {
        if (request.url == API_VERSION)
            return;

        auto authorization = request.get_header_value("Authorization");
        if (authorization.empty() || !AccountManager::Instance()->IsValidUser(authorization)) {
            response.code = crow::status::UNAUTHORIZED;
            response.end();
        }
    }

    void after_handle(crow::request& request, crow::response& response, context& context) {}
};

using CrowApp = crow::App<Authorization>;
