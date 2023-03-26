#include "product_service/product_service.h"

int main(int argc, char*argv[]) {
    HTTPProductWebServer app;
    return app.run(argc, argv);
}