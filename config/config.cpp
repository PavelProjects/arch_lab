#include "config.h"
#include <iostream>

Config::Config() {
    _host = std::getenv("DB_HOST");
    _port = std::getenv("DB_PORT");
    _login = std::getenv("DB_LOGIN");
    _password = std::getenv("DB_PASSWORD");
    _database = std::getenv("DB_DATABASE");

    _queue_host = std::getenv("QUEUE_HOST");
    _queue_topic = std::getenv("QUEUE_TOPIC");
    _queue_group_id  = std::getenv("QUEUE_GROUP_ID");

    if (std::getenv("CACHE_HOST") != nullptr) {
        _cache_host = std::getenv("CACHE_HOST");
    }
    if (std::getenv("CACHE_PORT") != nullptr) {
        _cache_port = std::getenv("CACHE_PORT");
    }

    std::cout << "Using config: " << std::endl;
    std::cout << "DB_HOST :: " << _host << std::endl;
    std::cout << "DB_PORT :: " << _port << std::endl;
    std::cout << "DB_LOGIN :: " << _login << std::endl;
    std::cout << "DB_PASSWORD :: " << _password << std::endl;
    std::cout << "DB_DATABASE :: " << _database << std::endl;
    std::cout << "QUEUE_HOST :: " << _queue_host << std::endl;
    std::cout << "QUEUE_TOPIC :: " << _queue_topic << std::endl;
    std::cout << "QUEUE_GROUP_ID :: " << _queue_group_id << std::endl;
    std::cout << "CACHE_HOST :: " << _cache_host << std::endl;
    std::cout << "CACHE_PORT :: " << _cache_port << std::endl;
}

Config &Config::get() {
    static Config _instance;
    return _instance;
}

const std::string &Config::get_cache_host() const {
    return _cache_host;
}

const std::string &Config::get_cache_port() const {
    return _cache_port;
}

const std::string &Config::get_port() const {
    return _port;
}

const std::string &Config::get_host() const {
    return _host;
}

const std::string &Config::get_login() const {
    return _login;
}

const std::string &Config::get_password() const {
    return _password;
}
const std::string &Config::get_database() const {
    return _database;
}

std::string &Config::port() {
    return _port;
}

std::string &Config::host() {
    return _host;
}

std::string &Config::login() {
    return _login;
}

std::string &Config::password() {
    return _password;
}

std::string &Config::database() {
    return _database;
}

std::string &Config::queue_group_id()
{
    return _queue_group_id;
}

const std::string &Config::get_queue_group_id() const
{
    return _queue_group_id;
}
std::string &Config::queue_host()
{
    return _queue_host;
}

std::string &Config::queue_topic()
{
    return _queue_topic;
}

const std::string &Config::get_queue_host() const
{
    return _queue_host;
}

const std::string &Config::get_queue_topic() const
{
    return _queue_topic;
}