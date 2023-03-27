#ifndef SERVICE_LOT_H
#define SERVICE_LOT_H

#include <string>
#include <vector>
#include "./user.h"
#include "Poco/JSON/Object.h"
#include <ctime>

namespace database {
    struct ProductSearch {
        long id;
        std::string name;
        long seller_id = -1;
        float cost_min = -1;
        float cost_max = -1;
        time_t creation_date_start = time(0);
        time_t creation_date_end = time(0);
    };

    class Product {
        private:
            long _id;
            std::string _name;
            std::string _description;
            float _cost;
            long _seller_id;
            User _seller = User::empty();
            time_t _creation_date;
            Poco::DateTime _testDate;

            void insert_entity();
            void update_entity();
        public:
            static Product empty() {
                Product lot;
                lot._id = -1;
                return lot;
            }
            static Product fromJson(const std::string &json);
            Poco::JSON::Object::Ptr toJSON() const;

            long get_id() const;
            const std::string &get_name() const;
            const std::string &get_description() const;
            float get_cost() const;
            const time_t &get_creation_date() const;
            long get_seller_id() const;
            const User &get_seller() const;

            long &id();
            std::string &name();
            std::string &description();
            float &cost();
            time_t &creattion_date();
            long &seller_id();
            User &seller();

            static std::vector<Product> search(ProductSearch lot_search);
            static Product get_by_id(long id);
            void save_to_db();
    };
}

#endif