#include <iostream>
#include <crow.h>

int main()
{
    std::cout << "This is Crow library test" << std::endl;
    
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([]()
    {
        return "Hello world";
    });

    app.port(8080).multithreaded().run();
}
