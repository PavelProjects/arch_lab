#include "user.h"
#include "database.h"
#include "../config/config.h"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <sstream>
#include <exception>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database {
    long User::auth(std::string &login, std::string &password) {
        try {
            std::cout << "Trying to auth " << login << "::" << password << std::endl;
            Poco::Data::Session session = database::Database::get().create_session();
            std::cout << "Session created" << std::endl;
            Poco::Data::Statement select(session);
            long id;
            select << "SELECT id FROM _user where login=? and password=?",
                into(id),
                use(login),
                use(password),
                range(0, 1);

            select.execute();
            Poco::Data::RecordSet rs(select);
            if (rs.moveFirst()) return id;
        } catch (Poco::Data::DataException &e) {
            std::cout << "Exception: " << e.what() << " :: " << e.message() << std::endl;
            return -1;
        }
        return 0;
    }

    void User::save_to_db() {
        try {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            // todo check existance

            insert << "INSERT INTO _user (name, email, login, password) VALUES(?, ?, ?, ?)",
                use(_name),
                use(_email),
                use(_login),
                use(_password);

            insert.execute();

            Poco::Data::Statement select(session);
            select << "SELECT LAST_INSERT_ID()",
                into(_id),
                range(0, 1);

            if (!select.done()){
                select.execute();
            }
            std::cout << "inserted:" << _id << std::endl;
        } catch (Poco::Data::MySQL::ConnectionException &e) {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        } catch (Poco::Data::MySQL::StatementException &e) {
            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    Poco::JSON::Object::Ptr User::toJson() const {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("name", _name);
        root->set("email", _email);
        root->set("login", _login);
        root->set("password", _password);

        return root;
    }

    template<typename T>
    T getOrDefault(Poco::JSON::Object::Ptr object, std::string field, T defaultValue) {
        if (object->has(field)) {
            return object->getValue<T>(field);
        }
        return defaultValue;
    }

    User User::fromJson(const std::string &str) {
        User user;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();
        
        user.id() = getOrDefault<long>(object, "id", 0);
        user.name() = getOrDefault<std::string>(object, "name", "");
        user.email() = getOrDefault<std::string>(object, "email", "");
        user.login() = getOrDefault<std::string>(object, "login", "");
        user.password() = getOrDefault<std::string>(object, "password", "");

        return user;
    }

    const std::string &User::get_login() const {
        return _login;
    }

    const std::string &User::get_password() const {
        return _password;
    }

    std::string &User::login() {
        return _login;
    }

    std::string &User::password() {
        return _password;
    }

    long User::get_id() const {
        return _id;
    }

    const std::string &User::get_name() const {
        return _name;
    }

    const std::string &User::get_email() const {
        return _email;
    }

    long &User::id() {
        return _id;
    }

    std::string &User::name() {
        return _name;
    }

    std::string &User::email() {
        return _email;
    }
}