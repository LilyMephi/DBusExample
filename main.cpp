#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDebug>
#include <QFile>
#include <QtWidgets> 
#include <QVector>
class SharingService : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.system.sharing");
public:
    SharingService() {
        // Регистрация сервиса на потоке D-Bus
        if (QDBusConnection::sessionBus().registerService("com.system.sharing")) {
            qDebug() << "Service registered successfully";
        } else {
            qDebug() << "Failed to register service:" << QDBusConnection::sessionBus().lastError().message();
            exit(1);
        }

        // Регистрация интерфейса  корневым путем "/"
        if (QDBusConnection::sessionBus().registerObject("/", this, QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals)) {
            qDebug() << "Object registered successfully";
        } else {
            qDebug() << "Failed to register object:" << QDBusConnection::sessionBus().lastError().message();
            exit(1);
        }
    }
public Q_SLOTS:   
    void RegisterService(const QString &name, const QStringList supportedFormats){
            
	    //В config.txt  будем сохранят информацию о сервисах
	    QFile configFile("config.txt");

	    //  Открываем файл чтобы считать  отуда информацию
	    //  если файл не открылся выводим информацию об ошибке 

            if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)){
		    qDebug() <<  "Cannot open the file \"config.txt\""; 
		    exit(-1);
            }
	    
	    //Считываем сервисы чтобы проверить на наличие дубликатов
	    QStringList existingServices;
	    QString line;
	    
	    QTextStream in(&configFile);
	    while(!in.atEnd()){
		    line = in.readLine().trimmed();		  
		    // Проверяем, что строка не пустая и начинается с "Name"
                    if (line.startsWith("Service: ", Qt::CaseInsensitive)) {
            	    	existingServices << line.mid(9).trimmed(); 
			qDebug() << "Считанное имя:" << line.mid(9).trimmed();  
              	    }
            }
	
	    // Проверяем на наличие дубликатов
            if (existingServices.contains(name)) {
            	qDebug() <<  "Service with name: " << name << " already exist";
                configFile.close();
		exit(-1);
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
	   QString services;
	   bool isFormat = false;
	   QTextStream out(&configFile);
	   while(!configFile.atEnd()){
	  	   QString line = out.readLine().trimmed();

		   if(line.startsWith("Service: ")){
			   services  = line.mid(9).trimmed(); 
	           }else if(line.startsWith("Formats: ")){
			   QStringList formats = line.mid(9).trimmed().split(",");
			   if(formats.contains(formatFile)){
				   //
				   isFormat = true;
				   break; 
			   }
		   }
           }
	   configFile.close();
	   //Проверяем нашли ли мы сервер для открытия файла
	   if(!isFormat){
		   qDebug() << "No D-Bus services available to open the file";
		   exit(-1);
           }
	   //Открываем сервис 
	   // Можно добавить список сервисов чтобы если он не открылся
	   // можно было бы использовать другой
	   QDBusInterface ifaceFile(services, "/", services, QDBusConnection::sessionBus());

	   if(!ifaceFile.isValid()){
		   qDebug() << "Invalid interface: "  << ifaceFile.lastError().message();
		   exit(-1);
	   }
           // 
	   // Далее метод который должен открыть файл
	   // QDBusMessage reply = iface.call("SomeFunction", path);
	   // Далее обрабатываем ответ
	   // if(reply.type() == QDBusMessage::ErrorMessage){}
	   // else{}
	   //
	   //Сделать ретерн ошибки а не простого сообщения!!!!
   }

   void OpenFileUsingService(QString path,QString service){	
	   QDBusInterface iface(service, path, service, QDBusConnection::sessionBus());

	   QDBusMessage reply = iface.call("OpenFile", path);
	   if (reply.type() == QDBusMessage::ErrorMessage) {
		   qDebug() << "Error to open service" << reply.errorMessage();
    	   } else {
           	  qDebug() << "Service" << service <<" is successfully open. Name: " ;
    	   }
   }


 
};

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    SharingService service;

    qDebug() << "QDBus wait............";

    return app.exec();
}

#include "main.moc"
