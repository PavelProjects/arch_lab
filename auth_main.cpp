#include "auth_service/auth_service.h"

int main(int argc, char*argv[]) {
    HTTPAuthWebServer app;
    return app.run(argc, argv);
}