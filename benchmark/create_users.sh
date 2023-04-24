#!/bin/bash

for i in {1..10}
do
    body="{\"name\": \"test$i\",\"password\": \"password\",	\"email\": \"test@tes$i.com\",	\"login\": \"test_login$i\"}"
    echo $body
    curl -X POST 'http://localhost:8081/auth/sign/up' -H  'accept: application/json' -H  'Content-Type: application/json' -d "$body"
done