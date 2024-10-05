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
 1. Для простoты примера имя регестрируемого сервиса задано 
 2. Убидитесь что вы установили `SharingService` из  одноименного репозитория
 3. Также Нужно в файле `client.pro` изменить путь до расположение библиотеки `SharingService`
 4. Для чтения файла есть пример в репозитории `ServiceOpen`

