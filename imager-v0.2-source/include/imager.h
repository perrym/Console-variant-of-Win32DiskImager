#include"disk.h"
#include<string>
#include<iostream>
#include <fstream>

class Imager
{
    private: 
        char* m_volumeID; //Идентиикатор тома
        ULONG m_deviceID;//Идентиикатор устройства
        HANDLE m_hVolume;//дескриптор тома 
        HANDLE m_hFile;//дескриптор файла
        HANDLE m_hRawDisk;//дескриптор диска
        unsigned long long m_Sectorsize; //размер сектора
        int m_Status;//статус операции
        char *m_SectorData;  //данные сектора
        char * m_Arg1;//буква диска
        char * m_Arg2;//пусть к образу для нарезки
		std::ofstream m_file;
		char inp;
        
        //набор статусов
        enum Status {STATUS_IDLE=0, STATUS_READING, STATUS_WRITING, STATUS_EXIT, STATUS_CANCELED}; 
    
   private:         
        char * getLogicalDrives();//функция для получения буквы диска
        bool isFileExist(char* filePath);//функция проверки существования файла
	
        
    public:
        Imager();
        ~Imager();
        bool checkInputParameters(int argc, char**argv);//функция проверки парметров командной строки
        bool writeImageToFlashDrive();//функция записи образа
        
};
  