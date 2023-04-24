#ifndef CACHE_H
#define CACHE_H

#include <string>
#include <iostream>
#include <memory>

namespace database {
    class Cache {
        private:
            std::shared_ptr<std::iostream> _stream;
            bool _enabled = false;
            Cache();
        public:
            static Cache get();
            void put(std::string id, const std::string& val);
            bool get(std::string id, std::string& val);
            bool is_cache_enabled() const;
    };
}

#endif