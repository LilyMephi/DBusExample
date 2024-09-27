#include "SharingService.h"
#include <iostream>
#include <string>
#include <fstream>
#include <QCoreApplication>
//#include <QDBusConnection>

using namespace std;

int main(int argc, char *argv[]){
	QCoreApplication app(argc, argv);
	// Убедитесь, что имя файла передается как аргумент
        if (argc < 2) {
             cout << "Usage:" << argv[0] << " <file_path>" << endl;;
            return -1;
        }
	string path = argv[1];
	const std::vector<std::string> supportedFormats = { "odt", "txt" };
	const auto onOpenFile = [](const string path){
		ifstream file(path);
		if(!file){
			cerr << "Can not open file!" << endl;
			exit(-1);
		}
		string line;
    		while (getline(file, line)) {  // Читаем файл построчно
       			 cout << line << endl;  // Выводим каждую строку на экран
   		 }
		file.close();
	};
	SharingService::SharingService service("com.example.text",supportedFormats,onOpenFile);
	cout << "Service register correctly" << endl;
	service.start(path);
	cout << "Open file correctly" << endl;
	return app.exec();
}


