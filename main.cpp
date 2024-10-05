#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDebug>
#include <QFile>
#include <QtWidgets> 
#include <QVector>
#include <QDBusReply>
#include <QSettings>
#include <QObject>
#include <QDBusPendingCall>

class SharingService : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.system.sharing");
public:
    explicit SharingService(QObject* parent = nullptr): QObject(parent) {
    
        // Регистрация сервиса на потоке D-Bus
        if (QDBusConnection::sessionBus().registerService("com.system.sharing")) {
            qInfo() << "Service registered successfully";
        } else {
            qDebug() << QDBusConnection::sessionBus().lastError().message();
            qFatal("Failed to register service.");
        }

        // Регистрация интерфейса  корневым путем "/"
        if (QDBusConnection::sessionBus().registerObject("/", this, QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals)) {
            qInfo() << "Object registered successfully";
		} else {
            qDebug()  << QDBusConnection::sessionBus().lastError().message();
            qFatal("Failed to register object.");
        }
    }

 public slots:
    void RegisterService(QString name,QStringList supportedFormats){
            
	    QSettings settings("config.ini",QSettings::IniFormat);
	    // Проверяем, существует ли сервис
            if (!settings.contains(name)) {
            // Если сервис не найден, добавляем его
            	settings.setValue(name + "/formats",supportedFormats.join(","));
       	        qInfo() << name << " registered successfully with formats:" << supportedFormats.join(", ");
            } else {
           	 // Если сервис уже существует, выводим его форматы
            	QString registeredFormats = settings.value(name + "/formats").toString();
                qInfo() << name << "is already registered with formats" << registeredFormats;
    }

    // Сохраняем изменения
    settings.sync();
   emit serviceRegistered(name);
   }
   void OpenFile(QString path){
	  QSettings settings("config.ini", QSettings::IniFormat); 
	  QFileInfo fileInf(path);
	  //Проверяем на существование файла
	  if(!fileInf.exists()){
		  qFatal("File does not exist");
          }

	  QString formatFile = fileInf.suffix(); // Получаем  формат файлa
	  QString service;
	  bool isFormat = false;
	  // Итерируемся по всем ключам
          QStringList keys = settings.allKeys();
    	  for (const QString &key : keys) {
          	if (key.endsWith("/formats")) {
          		QString registeredFormats = settings.value(key).toString();
                	QStringList formatList = registeredFormats.split(",");

                	// Проверяем, поддерживает ли сервис нужный формат
                	if (formatList.contains(formatFile, Qt::CaseInsensitive)) {
                		service = key.left(key.indexOf("/")); // Возвращаем имя сервиса
				isFormat = true;
				break;
          		}
        	 }
    	  }
	 	   //Проверяем нашли ли мы сервер для открытия файла
	   if(!isFormat){
		   qInfo() << "No D-Bus services available to open the file";
		   return;
           }
	   //Открываем файл с помощью найденного сервиса
	   OpenFileUsingService(path, service);
   }

   void OpenFileUsingService(QString path,QString service){
           //подключаемся к интерфейсу Introspect чтобы найти название интерфейса
	   QDBusInterface ifaceIntrospect(service, "/", QString(), QDBusConnection::sessionBus());
	   QString interfaceName;
           // Получаем информацию о доступных интерфейсах
           QDBusReply<QString> reply = ifaceIntrospect.call("Introspect");
           if (reply.isValid()) {
              interfaceName =  reply.value();
           } else {
               qDebug() << reply.error().message();
	       qFatal("Failed to find out interface name.");
           }
           //Подключаемся к интерфейсу для открытия
	   QDBusInterface iface(service, "/", "", QDBusConnection::sessionBus());
	   if(!iface.isValid()){
		   qFatal("Failed to run interface.");
	   }else{
		   qInfo() << "Conect to interface: " << service;
	   }
	   //Принимаем асинхронное сообщение от фызова функции OpenFile()
	   QDBusPendingCall pcall  = iface.asyncCall("OpenFile", path);
	   auto watcher = new QDBusPendingCallWatcher(pcall, this);
	   QObject::connect(watcher, &QDBusPendingCallWatcher::finished, this,
                     [&](QDBusPendingCallWatcher *w) {
        	     	QDBusPendingReply<void> reply(*w);
			if(reply.isError()){
				qDebug() << reply.error().message();
				qFatal("Error to read the file.");
			}else{
				qInfo() << "Readin the  file was successfule";
			}
           }); 
	   emit fileOpened(path);
   }

signals:
   void serviceRegistered(const QString &service);
   void fileOpened(const QString &path);
private slots:
	void onFileOpened(const QString &path){
		qInfo() <<  "File opened" <<path;
	}

};
 
int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    SharingService service;

    qInfo() << "QDBus wait............";

    return app.exec();
}

#include "main.moc"
