# Для Загрузки
```bash
   git clone https://github.com/LilyMephi/DBusExample/
   cd DBusExample
   qmake project.pro
   make
   qmake client.pro
   make
```
### Для запуска в разных терминалах нужно запустить
```bash
  ./ClientService
  ./SharingService
```
### P.S 
Для простаты примера имя регестрируемого сервиса задано 
Убидитесь что вы установили `SharingService` из  одноименного репозитория
Также Нужно в файле `client.pro` изменить путь до расположение библиотеки `SharingService`
Для чтения файла есть пример в репозитории `ServiceOpen`

