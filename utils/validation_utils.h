#include <string>
#include "Poco/Net/HTMLForm.h"
#include "../database/user.h"

bool have_all_user_fields(HTMLForm &form, std::string &missing) {
    bool result = true;
    if (!form.has("login")) {
        missing += "login;"; 
        result = false;
    }
    if (!form.has("password")) {
        missing += "password;"; 
        result = false;
    }
    if (!form.has("email")) {
        missing += "email;"; 
        result = false;
    }
    if (!form.has("name")) {
        missing += "name; are missing"; 
        result = false;
    }
    return result;
};

bool check_name(const std::string &name, std::string &reason) {
    if (name.length() < 3) {
        reason = "Name must be at least 3 signs;";
        return false;
    }

    if (name.find(' ') != std::string::npos) {
        reason = "Name can't contain spaces;";
        return false;
    }

    if (name.find('\t') != std::string::npos) {
        reason = "Name can't contain spaces;";
        return false;
    }

    return true;
};

bool check_email(const std::string &email, std::string &reason) {
    if (email.find('@') == std::string::npos) {
        reason = "Email must contain @;";
        return false;
    }

    if (email.find(' ') != std::string::npos) {
        reason = "EMail can't contain spaces;";
        return false;
    }

    if (email.find('\t') != std::string::npos) {
        reason = "EMail can't contain spaces;";
        return false;
    }

    return true;
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

    if (user.get_login().length() == 3) {
        message += "login must be at least 3 signs;";
        result = false;
    }

    if (user.get_password().length() == 3) {
        message += "password must be at least 3 signs;";
        result = false;
    }

    return result;
};