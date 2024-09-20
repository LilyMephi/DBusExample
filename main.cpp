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
    QString RegisterService(const QString &name, const QStringList supportedFormats){
        
	    //В config.txt  будем сохранят информацию о сервисах
	    QFile configFile("config.txt");

	    //  Открываем файл чтобы записать туда информацию
	    //  если файл не открылся выводим информацию об ошибке 
            if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)){
		    QString errmess =  "Cannot open the file \"config.txt\""; 
		    qDebug() << errmess;
                    return  errmess;
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
            	QString mess =  "Service with name: "+ name +" already exist";
		qDebug() << mess;
                configFile.close();
		return mess;
            }
	    configFile.close();
 	    //  Открываем файл чтобы записать туда информацию
	    //  если файл не открылся выводим информацию об ошибке 
            if (!configFile.open(QIODevice::Append | QIODevice::Text)){
		    QString errmess =  "Cannot open the file \"config.txt\""; 
		    qDebug() << errmess;
                    configFile.close();
		    return  errmess;
            }

	    //Записываем информацию о сервисе
	    QTextStream out(&configFile);
	    out << "Service: "<< name <<"\n";
	    out << "Formats: ";
	    out << supportedFormats.join(", ");
	    out << "\n";

           configFile.close();
	    return nullptr;
   }
   QString OpenFile(QString path){
	  // QStringList services = 

   }
};

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    SharingService service;

    qDebug() << "QDBus wait............";

    return app.exec();
}

#include "main.moc"
