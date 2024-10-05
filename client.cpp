#include "SharingService.h"
#include <iostream>
#include <string>
#include <fstream>
#include <QCoreApplication>
//#include <QDBusConnection>

using namespace std;

int main(int argc, char *argv[]){
	QCoreApplication app(argc, argv);	
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
        SharingService service("com.example.text",supportedFormats,onOpenFile);
	service.start();
	return app.exec();
}


