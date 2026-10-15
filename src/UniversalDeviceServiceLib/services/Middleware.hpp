#pragma once

#include <string>

#include <crow.h>
#include <crow/app.h>
#include <crow/common.h>
#include <crow/http_request.h>
#include <crow/http_response.h>

#include "AccountManager.hpp"
#include "Constants.hpp"
#include "Defines.hpp"

struct Authorization {
    struct context {};

    void before_handle(crow::request& request, crow::response& response, context& context) {
        if (request.url == API_VERSION)
            return;

        // not authorized - NOT allowed
        // client -  ALL allowed
        // device or any other (direct request) - NOT allowed
        const std::string authorization = request.get_header_value("Authorization");
        const auto [valid, login, _] = AccountManager::Instance()->ParseAuthorizationString(authorization);
        if (!valid || login != Constants::LoginService) {
            response.code = crow::status::UNAUTHORIZED;
            response.end();
        }
    }

    void after_handle(crow::request& request, crow::response& response, context& context) {}
};

using CrowApp = crow::App<Authorization>;
