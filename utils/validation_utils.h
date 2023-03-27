#include <string>
#include "Poco/Net/HTMLForm.h"
#include "../database/user.h"
#include "../database/product.h"

bool check_name(const std::string &name, std::string &reason) {
    if (name.length() < 3) {
        reason = "Name must be at least 3 signs;";
        return false;
    }

    return true;
};

bool check_email(const std::string &email, std::string &reason) {
    bool res = true;
    if (email.find('@') == std::string::npos) {
        reason = "Email must contain @;";
        res = false;
    }

    if (email.find(' ') != std::string::npos) {
        reason = "EMail can't contain spaces;";
        res = false;
    }

    if (email.find('\t') != std::string::npos) {
        reason = "EMail can't contain spaces;";
        res = false;
    }

    return res;
};


bool validate_user(database::User &user, std::string &message) {
    bool result = true;
    std::string reason;

    if (!check_email(user.get_email(), reason)) {
        message += reason;
        result = false;
    }

    if (!check_name(user.get_name(), reason)) {
        message += reason;
        result = false;
    }

    if (user.get_login().length() < 3) {
        message += "login must be at least 3 signs;";
        result = false;
    }

    if (user.get_password().length() < 3) {
        message += "password must be at least 3 signs;";
        result = false;
    }

    return result;
};

bool validate_product(database::Product &product, std::string &message) {
    bool result = true;

    if (product.get_name().length() < 3) {
        result = false;
        message += "name should be at least 3 signs;";
    }

    if (product.get_description().length() < 3) {
        result = false;
        message += "description should be at least 3 digits;";
    }

    if (product.get_cost() < 0) {
        result = false;
        message += "cast can't be negetive;";
    }

    return result;
}