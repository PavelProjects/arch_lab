# Описание скриптов
- cache_test.lua - скрипт получения по id
- cache_test_search.lua - скрипт тестирования поиска

# Результаты
Для создания тестовых пользователей был реализован скрипт ./create_users.sh, однако работает с ошибками и после его работы необходимо перегружать контейнер. Ошибку установить пока не удалось.

Скрипт тестирования - benchmark.sh. Тестирует с помощью двух выше указанных скриптов с разным количеством потоков и соединений.

Для отключения кешей надо убрать переменные окружения CACHE_HOST и CACHE_PORT в docker-compose для user_service.

Прирост в скорости ответа лучше всего заметен при тестировании поиска, даже при учете плохой реализации (бесмысленный парсинг данных из кеша).

# Без кеша
```
Script: ./cache_test.lua, threads: 1, connections: 1
Running 10s test @ http://localhost:8082/
  1 threads and 1 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.04ms  287.74us   8.37ms   93.67%
    Req/Sec     0.96k    75.55     1.07k    77.00%
  Latency Distribution
     50%    0.99ms
     75%    1.03ms
     90%    1.14ms
     99%    2.11ms
  9567 requests in 10.00s, 3.38MB read
Requests/sec:    956.30
Transfer/sec:    345.57KB

Script: ./cache_test.lua, threads: 2, connections: 2
Running 10s test @ http://localhost:8082/
  2 threads and 2 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.01ms  196.56us   9.95ms   98.23%
    Req/Sec     0.99k    34.89     1.03k    81.50%
  Latency Distribution
     50%    0.98ms
     75%    1.02ms
     90%    1.07ms
     99%    1.29ms
  19747 requests in 10.00s, 6.97MB read
Requests/sec:   1974.57
Transfer/sec:    713.50KB

Script: ./cache_test.lua, threads: 5, connections: 5
Running 10s test @ http://localhost:8082/
  5 threads and 5 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.38ms  391.86us  13.86ms   96.52%
    Req/Sec   727.81     34.91   780.00     79.60%
  Latency Distribution
     50%    1.32ms
     75%    1.43ms
     90%    1.56ms
     99%    2.65ms
  36227 requests in 10.01s, 12.78MB read
Requests/sec:   3620.65
Transfer/sec:      1.28MB

Script: ./cache_test.lua, threads: 10, connections: 10
Running 10s test @ http://localhost:8082/
  10 threads and 10 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     2.10ms    1.55ms  42.22ms   98.39%
    Req/Sec   496.15     28.00   560.00     80.80%
  Latency Distribution
     50%    1.92ms
     75%    2.18ms
     90%    2.51ms
     99%    4.71ms
  49408 requests in 10.01s, 17.43MB read
Requests/sec:   4937.61
Transfer/sec:      1.74MB

Script: ./cache_test_search.lua, threads: 1, connections: 1
Running 10s test @ http://localhost:8082/
  1 threads and 1 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.33ms  279.34us   8.80ms   96.78%
    Req/Sec   750.34     38.80   818.00     79.00%
  Latency Distribution
     50%    1.30ms
     75%    1.36ms
     90%    1.44ms
     99%    2.04ms
  7472 requests in 10.00s, 2.24MB read
Requests/sec:    746.98
Transfer/sec:    229.78KB

Script: ./cache_test_search.lua, threads: 2, connections: 2
Running 10s test @ http://localhost:8082/
  2 threads and 2 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.32ms  167.32us   6.19ms   93.43%
    Req/Sec   752.15     32.16   797.00     79.00%
  Latency Distribution
     50%    1.29ms
     75%    1.35ms
     90%    1.44ms
     99%    1.86ms
  14975 requests in 10.00s, 4.50MB read
Requests/sec:   1496.87
Transfer/sec:    460.46KB

Script: ./cache_test_search.lua, threads: 5, connections: 5
Running 10s test @ http://localhost:8082/
  5 threads and 5 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.82ms  551.44us  15.06ms   96.05%
    Req/Sec   554.65     25.88   616.00     75.20%
  Latency Distribution
     50%    1.73ms
     75%    1.87ms
     90%    2.06ms
     99%    4.07ms
  27629 requests in 10.01s, 8.30MB read
Requests/sec:   2760.20
Transfer/sec:    849.12KB

Script: ./cache_test_search.lua, threads: 10, connections: 10
Running 10s test @ http://localhost:8082/
  10 threads and 10 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     2.73ms    0.90ms  22.33ms   90.78%
    Req/Sec   370.36     24.54   430.00     72.20%
  Latency Distribution
     50%    2.55ms
     75%    2.93ms
     90%    3.43ms
     99%    6.38ms
  36908 requests in 10.01s, 11.09MB read
Requests/sec:   3686.06
Transfer/sec:      1.11MB

```

# С кешем
```
Script: ./cache_test.lua, threads: 1, connections: 1
Running 10s test @ http://localhost:8082/
  1 threads and 1 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.10ms  389.64us  14.51ms   97.40%
    Req/Sec     0.92k    63.18     1.00k    75.00%
  Latency Distribution
     50%    1.05ms
     75%    1.11ms
     90%    1.20ms
     99%    1.94ms
  9105 requests in 10.00s, 3.21MB read
Requests/sec:    910.48
Transfer/sec:    329.01KB

Script: ./cache_test.lua, threads: 2, connections: 2
Running 10s test @ http://localhost:8082/
  2 threads and 2 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.10ms  134.82us   6.59ms   94.23%
    Req/Sec     0.90k    27.98     0.95k    85.00%
  Latency Distribution
     50%    1.08ms
     75%    1.12ms
     90%    1.19ms
     99%    1.50ms
  17972 requests in 10.00s, 6.34MB read
Requests/sec:   1796.62
Transfer/sec:    649.20KB

Script: ./cache_test.lua, threads: 5, connections: 5
Running 10s test @ http://localhost:8082/
  5 threads and 5 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.56ms  520.58us  13.67ms   95.96%
    Req/Sec   646.90     47.45   717.00     79.20%
  Latency Distribution
     50%    1.47ms
     75%    1.61ms
     90%    1.79ms
     99%    3.69ms
  32214 requests in 10.01s, 11.37MB read
Requests/sec:   3218.85
Transfer/sec:      1.14MB

Script: ./cache_test.lua, threads: 10, connections: 10
Running 10s test @ http://localhost:8082/
  10 threads and 10 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     2.24ms  613.45us  18.91ms   87.39%
    Req/Sec   448.46     20.65   500.00     71.20%
  Latency Distribution
     50%    2.13ms
     75%    2.43ms
     90%    2.79ms
     99%    4.27ms
  44674 requests in 10.01s, 15.76MB read
Requests/sec:   4462.22
Transfer/sec:      1.57MB

Script: ./cache_test_search.lua, threads: 1, connections: 1
Running 10s test @ http://localhost:8082/
  1 threads and 1 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     0.85ms  142.66us   5.40ms   95.73%
    Req/Sec     1.17k    65.96     1.27k    86.14%
  Latency Distribution
     50%  828.00us
     75%    0.87ms
     90%    0.92ms
     99%    1.38ms
  11748 requests in 10.10s, 3.53MB read
Requests/sec:   1163.13
Transfer/sec:    357.83KB

Script: ./cache_test_search.lua, threads: 2, connections: 2
Running 10s test @ http://localhost:8082/
  2 threads and 2 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     0.85ms  149.58us   7.92ms   97.82%
    Req/Sec     1.17k    31.21     1.26k    90.05%
  Latency Distribution
     50%  835.00us
     75%    0.87ms
     90%    0.92ms
     99%    1.06ms
  23334 requests in 10.10s, 7.01MB read
Requests/sec:   2310.33
Transfer/sec:    710.75KB

Script: ./cache_test_search.lua, threads: 5, connections: 5
Running 10s test @ http://localhost:8082/
  5 threads and 5 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.16ms  297.97us   9.40ms   96.19%
    Req/Sec     0.86k    25.81     0.91k    72.60%
  Latency Distribution
     50%    1.12ms
     75%    1.22ms
     90%    1.33ms
     99%    1.87ms
  42808 requests in 10.01s, 12.86MB read
Requests/sec:   4277.43
Transfer/sec:      1.29MB

Script: ./cache_test_search.lua, threads: 10, connections: 10
Running 10s test @ http://localhost:8082/
  10 threads and 10 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     2.07ms    4.43ms  91.47ms   99.02%
    Req/Sec   579.45     51.30   646.00     91.80%
  Latency Distribution
     50%    1.62ms
     75%    1.89ms
     90%    2.24ms
     99%    6.31ms
  57716 requests in 10.01s, 17.34MB read
Requests/sec:   5766.81
Transfer/sec:      1.73MB
```