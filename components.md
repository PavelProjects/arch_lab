# Компонентная архитектура

## Компонентная диаграмма
```plantuml
@startuml
!include https://raw.githubusercontent.com/plantuml-stdlib/C4-PlantUML/master/C4_Container.puml

AddElementTag("microService", $shape=EightSidedShape(), $bgColor="CornflowerBlue", $fontColor="white", $legendText="microservice")
AddElementTag("storage", $shape=RoundedBoxShape(), $bgColor="lightSkyBlue", $fontColor="white")

Person(admin, "Администратор")
Person(moderator, "Модератор")
Person(user, "Пользователь")

System_Ext(web_site, "Веб интерфейс", "HTML, CSS, JavaScript, React")

System_Boundary(conference_site, "Сайт заказа услуг") {
   Container(client_service, "Сервис пользователей", "C++", "Сервис автоизации, управления или создания пользователей", $tags = "microService")    
   Container(market_service, "Сервис лотов услуг", "C++", "Сервис для создание/просмотра/редактирования предоставляемых услуг", $tags = "microService") 
   Container(payment_service, "Сервис оплаты", "C++", "Сервис для произведения оплаты купленных услуг", $tags = "microService")   
   ContainerDb(db, "База данных", "MySQL", "Хранение данных о услугах, транзациях и пользователях", $tags = "storage")
}


Rel(admin, web_site, "Просмотр, редактирование, добавление и удаление информации о пользователях и услугах.")
Rel(moderator, web_site, "Просмотр и модерирование предоставляемых услуг.")
Rel(user, web_site, "Регистрация, продажа или покупка услуг. Создание и редактирование своего лота, просмотр лотов других пользователей.")

Rel(web_site, client_service, "Работа с пользователями", "localhost/person")
Rel(client_service, db, "INSERT/SELECT/UPDATE", "SQL")

Rel(web_site, market_service, "Работа с услугами (создание, просмотр, редактирование)", "localhost/market")
Rel(market_service, db, "INSERT/SELECT/UPDATE", "SQL")

Rel(web_site, payment_service, "Оплата услуг", "localhost/payment")
Rel(payment_service, db, "INSERT/SELECT/UPDATE", "SQL")

@enduml
```

# Список компонентов
## Сервис пользователей
**REST Api:**
- Создание нового пользователя
   - Метод - POST
   - Тело запроса - логин, пароль, фио, контактные данные
   - Код успешного завершения - 201
- Редактирование пользователя
   - Метод - PUT
   - Тело запроса - логи, фио, контактные данные
- Поиск пользотвалея
   - Метод - GET
   - Тело запроса - логин или фио или контактные данные
   - Тело ответа - ид юзера, логин, фио, контактные данные
   - Код ответа - 200
- Авторизация
   - Метод - POST
   - Тело запроса - логин, пароль
   - Тело ответа - токен
   - Код ответа - 200

## Сервиc лотов услуг
**REST Api:**
- Создание нового лота услуги
   - Метод - Post
   - Тело запроса - название услуги, описание, информация о продавце, цена
   - Код ответа - 201
- Получение лотов услуг
   - Метод - GET
   - Тело ответа - ид услуги, название услуги, описание, продавец, цена, дата создания
   - Код ответа - 200
- Редактирование лота услуги
   - Метод - PUT
   - Тело запроса - ид услуги, название услуга, описание, цена
   - Код ответа - 200

## Сервис оплаты
**REST Api:**
- Оплата услуги
   - Метод - POST
   - Тело запроса - ид лота услуги, ид покупателя
   - Тело ответа - квитанция об оплате
   - Код ответа - 200
- Получение информации о покупках
   - Метод - GET
   - Тело запроса - ид пользователя
   - Тело ответа - ид лота услуги, дата покупки, сумма покупки
   - Код ответа - 200
