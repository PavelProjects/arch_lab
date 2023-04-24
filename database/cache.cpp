#include "cache.h"
#include "../config/config.h"

#include <exception>
#include <mutex>

#include <redis-cpp/stream.h>
#include <redis-cpp/execute.h>

namespace database {
    std::mutex _mtx;
    Cache::Cache() {
        std::string host = Config::get().get_cache_host();
        std::string port = Config::get().get_cache_port();

        if (host.empty() || port.empty()) {
            std::cout << "Cache host or port parameter are missing, cache disabled!" << std::endl;
            _enabled = false;
        } else {
            std::cout << "Cache host:port" << host <<":" << port << std::endl;
            std::cout << "Cache enabled!" << std::endl;
            _stream = rediscpp::make_stream(host, port);
            _enabled = true;
        }
    }

    Cache Cache::get() {
        static Cache instance;
        return instance;
    }

    void Cache::put([[maybe_unused]] std::string key, [[maybe_unused]] const std::string &val) {
        if (_enabled) {
            std::cout << "Putting to cache " << key << " " << val << std::endl;
            std::lock_guard<std::mutex> lck(_mtx);
            rediscpp::value response = rediscpp::execute(*_stream, "set",
                                                        key,
                                                        val, "ex", "60"); // TODO env variable
        }
    }

    bool Cache::get([[maybe_unused]] std::string key, [[maybe_unused]] std::string &val) {
        if (_enabled) {
            std::cout << "Getting from cache by key " << key << std::endl;

            std::lock_guard<std::mutex> lck(_mtx);
            rediscpp::value response = rediscpp::execute(*_stream, "get", key);

            if (response.is_error_message())
                return false;
            if (response.empty())
                return false;

            val = response.as<std::string>();
            return true;
        }
        return false;
    }

    bool Cache::is_cache_enabled() const{
        return _enabled;
    }
}