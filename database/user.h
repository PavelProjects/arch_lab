#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"

namespace database {
    class User {
        private:
            long _id;
            std::string _login;
            std::string _name;
            std::string _email;
            std::string _password;

        public:
            static User fromJson(const std::string &json);
            Poco::JSON::Object::Ptr toJson() const;

            long get_id() const;
            const std::string &get_name() const;
            const std::string &get_email() const;
            const std::string &get_login() const;
            const std::string &get_password() const;

            long& id();
            std::string &name();
            std::string &email();
            std::string &login();
            std::string &password();

            static long auth(std::string &login, std::string &password);
            void save_to_db();
    };
}

#endif