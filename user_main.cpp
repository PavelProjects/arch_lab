#include "./user_service/user_service.h"

int main(int argc, char*argv[]) {
    HTTPUserWebServer app;
    return app.run(argc, argv);
}