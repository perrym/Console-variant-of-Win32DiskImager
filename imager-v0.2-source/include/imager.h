#include"disk.h"
#include<string>
#include<iostream>
#include <fstream>

class Imager
{
    private: 
        char* m_volumeID; //������������ ����
        ULONG m_deviceID;//������������ ����������
        HANDLE m_hVolume;//���������� ���� 
        HANDLE m_hFile;//���������� �����
        HANDLE m_hRawDisk;//���������� �����
        unsigned long long m_Sectorsize; //������ �������
        int m_Status;//������ ��������
        char *m_SectorData;  //������ �������
        char * m_Arg1;//����� �����
        char * m_Arg2;//����� � ������ ��� �������
		std::ofstream m_file;
		char inp;
        
        //����� ��������
        enum Status {STATUS_IDLE=0, STATUS_READING, STATUS_WRITING, STATUS_EXIT, STATUS_CANCELED}; 
    
   private:         
        char * getLogicalDrives();//������� ��� ��������� ����� �����
        bool isFileExist(char* filePath);//������� �������� ������������� �����
	
        
    public:
        Imager();
        ~Imager();
        bool checkInputParameters(int argc, char**argv);//������� �������� ��������� ��������� ������
        bool writeImageToFlashDrive();//������� ������ ������
        
};
  