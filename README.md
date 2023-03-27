# Содержимое проекта
- ./docs - документация по проекту. Тут находится OpenApi.
- ./mariadb - контейнер бд со скриптом создания таблиц (rollout.sql). Скрипт вызывается автоматически при первом запуске контейнера.
- ./config - конфигурация бд
- ./database - менеджер подключений к бд, описание сущностей бд
- ./auth_service - реализация сервиса авторизации
- ./user_service - реалиазция сервиса пользователей
- ./product_service - реализация сервиса работы с продаваемыми услугами
- ./utils - набор утилит для упрощения работы
- .env - переменные окружения для запуска контейнеров
- auth_main.cpp, user_main.cpp и product_main.cpp - точки входа контейнеров сервисов авторизации, пользователей и продаваемы услуг соотвественно
- docker-compose.yaml - конфигурация запуска контейнеров

# Запуск:
```
docker compose build
docker compose up
```

# Api
Конфигурация ендпоинтов описана в ./docs/index.yaml.

Для авторизации можно использовать тестовых пользователей:
- autotest1 / 123
- autotest2 / 123

Или же создать своего.