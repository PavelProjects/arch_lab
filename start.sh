export DB_HOST=192.168.0.112
export DB_PORT=6033
export DB_LOGIN=stud
export DB_PASSWORD=stud
export DB_DATABASE=archdb

export AUTH_SERVICE_PORT=8081
export USER_SERVICE_PORT=8082
export PRODUCT_SERVICE_PORT=8083

export CACHE_PORT=6379
export CACHE_HOST=192.168.0.112

export JWT_KEY=bigsecretkey

export MAX_SHARD=1
export SEQUENCE_SHARD=2

export CREATE_TEST_USERS=1


export QUEUE_HOST=192.168.0.112:9092
export QUEUE_TOPIC=event_server
export QUEUE_GROUP_ID=0

cd ./build
make user_service
./user_service