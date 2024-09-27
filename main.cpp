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
#include <QObject>
#include <QDBusPendingCall>

class SharingService : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.system.sharing");
public:
    explicit SharingService(QObject* parent = nullptr): QObject(parent) {
        // Регистрация сервиса на потоке D-Bus
        if (QDBusConnection::sessionBus().registerService("com.system.sharing")) {
            qDebug() << "Service registered successfully";
        } else {
            qDebug() << "Failed to register service:" << QDBusConnection::sessionBus().lastError().message();
            exit(-1);
        }

        // Регистрация интерфейса  корневым путем "/"
        if (QDBusConnection::sessionBus().registerObject("/", this, QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals)) {
            qDebug() << "Object registered successfully";
		} else {
            qDebug() << "Failed to register object:" << QDBusConnection::sessionBus().lastError().message();
            exit(-1);
        }
    }

 public slots:
    void RegisterService(QString name,QStringList supportedFormats){
            
	    //В config.txt  будем сохранят информацию о сервисах
	    QFile configFile("config.txt");

	    //  Открываем файл чтобы считать  отуда информацию
	    //  если файл не открылся выводим информацию об ошибке 
            if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)){
		    qDebug() <<  "Cannot open the file \"config.txt\""; 
		    if (!configFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        	    	qDebug() << "Failed to open file for writen";
        		exit(-1); // Завершение программы с кодом ошибки

   		    }else{ 
		    	//Записываем информацию о сервисе
	    		QTextStream out(&configFile);
	   		out << "Service: "<< name <<"\n";
	   		out << "Formats: ";
	    		out << supportedFormats.join(", ");
	    		out << "\n";
		
           		configFile.close();
			exit(0);
		    }            
	    }
	    //Считываем сервисы чтобы проверить на наличие дубликатов
	    QStringList existingServices;
	    QString line;
	    QTextStream in(&configFile);
	    while(!in.atEnd()){
		    line = in.readLine().trimmed();		  
		    // Проверяем, что строка не пустая и начинается с   "Service: "
                    if (line.startsWith("Service: ", Qt::CaseInsensitive)) {
            	    	existingServices << line.mid(9).trimmed();   
              	    }
            }
	
	    // Проверяем на наличие дубликатов
            if (existingServices.contains(name)) {
            	qDebug() <<  "Service with name: " << name << " already exist";
                configFile.close();
		return;
            }
	    configFile.close();

 	    //  Открываем файл чтобы записать туда информацию
	    //  если файл не открылся выводим информацию об ошибке 
            if (!configFile.open(QIODevice::Append | QIODevice::Text)){
		    qDebug() <<  "Cannot open the file \"config.txt\""; 
                    configFile.close();
		    exit(-1);
            }

	    //Записываем информацию о сервисе
	    QTextStream out(&configFile);
	    out << "Service: "<< name <<"\n";
	    out << "Formats: ";
	    out << supportedFormats.join(", ");
	    out << "\n";
		
           configFile.close();
	   emit serviceRegistered(name);
   }
   void OpenFile(QString path){
	  QFileInfo fileInf(path);

	  //Проверяем на существование файла
	  if(!fileInf.exists()){
		  qDebug() <<  "File does not exist";
		  exit(-1);
          }

	  QString formatFile = fileInf.suffix(); // Получаем  формат файлa
	 
	  QFile configFile("config.txt");
          //  Открываем файл чтобы считать  отуда информацию
	  //  если файл не открылся выводим информацию об ошибке 
	  if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)){
              qDebug() << "Cannot open the file \"config.txt\""; 
              exit(-1);
          }
	  
	  //Проверяем содержит ли данный сервис нужный формат

	   QString service;
	   bool isFormat = false;
	   QTextStream in(&configFile);
	   QString line;

	   //Считываем данные из файла и проверяем есть ли у сервиса нужное расшиерние файла
	    while(!in.atEnd()){
		    QStringList formats;
		    line = in.readLine();		    
		    // Считываем сервис
                    if (line.startsWith("Service: ", Qt::CaseInsensitive)) {
		        service =  line.mid(9).trimmed(); 
		    }
		    // Считываем форматы данного сервиса
                    if (line.startsWith("Formats: ", Qt::CaseInsensitive)) {
			formats << line.mid(9).trimmed().split(", "); 
		    }

		    // если нужное расширение есть добовляем его в список
		    if(formats.contains(formatFile)){
			    isFormat = true;
			    break;
	            }

            }

	   configFile.close();
	   //Проверяем нашли ли мы сервер для открытия файла
	   if(!isFormat){
		   qDebug() << "No D-Bus services available to open the file";
		   exit(-1);
           }
	   //Открываем файл с помощью найденного сервиса
	   OpenFileUsingService(path, service);
   }

   void OpenFileUsingService(QString path,QString service){		   
	   QDBusInterface iface(service, "/", "com.sharing.service", QDBusConnection::sessionBus());
	   if(!iface.isValid()){
		   qDebug() << "Failed to run interface" <<service;
		   exit(-1);
	   }else{
		   qDebug() << "Conect to interface " << service;
	   }
	   QDBusPendingCall pcall  = iface.asyncCall("OpenFile", path);
	   auto watcher = new QDBusPendingCallWatcher(pcall, this);
	   QObject::connect(watcher, &QDBusPendingCallWatcher::finished, this,
                     [&](QDBusPendingCallWatcher *w) {
        	     	QDBusPendingReply<void> reply(*w);
			if(reply.isError()){
				qDebug() << "Error to read the file " << reply.error().message();
			}else{
				qDebug() << "Readin the  file was successfule";
			}
           }); 
	   /*
	   if (reply.type() == QDBusMessage::ErrorMessage) {
		   qDebug() << "Error to open service" << reply.errorMessage();
    	   } else {
           	  qDebug() << "Service" << service <<" is successfully open." ;
    	   }*/
	   emit fileOpened(path);
   }

signals:
   void serviceRegistered(const QString &service);
   void fileOpened(const QString &path);
private slots:
	void onFileOpened(const QString &path){
		qDebug() <<  "File opened" <<path;
	}

};
 
int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    SharingService service;

    qDebug() << "QDBus wait............";

    return app.exec();
}

#include "main.moc"
