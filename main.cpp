#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDebug>
#include <QFile>
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
            if (!configFile.open(QIODevice::Append | QIODevice::Text)){
		    QString errmess =  "Cannot open the file \"config.txt\""; 
		    qDebug() << errmess;
		    return  errmess;
            }
	    
	    //Считываем сервисы чтобы проверить на наличие дубликатов
	    QStringList existingServices;
	    QString line;
	    while (line = configFile.readLine(), !line.isNull()) {
           	 if (line.startsWith("Service Name: ")) {
                	existingServices << line.mid(14).trimmed();
           	 }
            }

	    // Проверяем на наличие дубликатов
            if (existingServices.contains(name)) {
            	QString mess =  "Сервис с именем"+name +" уже зарегистрирован.";
		qDebug() << mess;
		return mess;
            }

            // Перемещаем указатель в конец файла
            configFile.seek(0);

	    //Записываем информацию о сервисе
	    QTextStream out(&configFile);
	    out << "Supported service: "<< name <<"\n";
	    out << "Supported formats : ";
	    for( const QString& str : supportedFormats){
		 out << str << ",";
	    }
	    out << "\n";
           
	    return nullptr;
   }
};

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    SharingService service;

    qDebug() << "QDBus сервис запущен. Ожидание вызовов...";

    return app.exec();
}

#include "main.moc"
