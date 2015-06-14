#include "include/imager.h"
#include "include/disk.h"
#include <stdio.h>
#include <string>

Imager::Imager()
{
     m_Status = STATUS_IDLE; 
	
	try{
		char * _out;
		char buffer[MAX_PATH];
		GetModuleFileName( NULL, buffer, MAX_PATH );
		std::string::size_type pos = std::string( buffer ).find_last_of( "\\/" );
		std::string buffer_in = std::string(buffer).substr( 0, pos);
		std::cout << "The imager.exe is run from " << buffer_in << std::endl;
		std::string buffer_out = buffer_in + std::string("\\imager_out.log");
		m_file.open (buffer_out.c_str());
		
		m_file << "Log is created..." << std::endl;
		m_file << buffer_out << std::endl;
	 }
	 catch(...)
	 {
		std::cout << "Unable to create or open log file." << std::endl;
		std::cout << "Press Ctrl+C to close window..." << std::endl;
		std::cin >> inp;
	 }
     
   
}

Imager::~Imager()
{
 if (m_hRawDisk != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hRawDisk);
        m_hRawDisk = INVALID_HANDLE_VALUE;
    }
    if (m_hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
    }
    if (m_hVolume != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hVolume);
        m_hVolume = INVALID_HANDLE_VALUE;
    }
    if (m_SectorData != NULL)
    {
        delete m_SectorData;
        m_SectorData = NULL;
    }
	  m_file.close();
}


char * Imager::getLogicalDrives()
{
    // GetLogicalDrives returns 0 on failure, or a bitmask representing
    // the drives available on the system (bit 0 = A:, bit 1 = B:, etc)
    unsigned long driveMask = GetLogicalDrives();
    int i=0;
    while (driveMask != 0)
    {
        if (driveMask & 1)
        {
                // the "A" in drivename will get incremented by the # of bits
                // we've shifted
        
                char drivename[] = "\\\\.\\A:\\";
                drivename[4]+=i;
                if (checkDriveType(drivename, &m_deviceID))
                {
                    m_volumeID =  drivename;
                    return m_volumeID;
                }
        }
        driveMask >>= 1;
        ++i;
    }
    return 0;
} 

bool Imager::isFileExist(char* filePath)
{
	//This will get the file attributes bitlist of the file
	DWORD fileAtt = GetFileAttributesA(filePath);

	//If an error occurred it will equal to INVALID_FILE_ATTRIBUTES
	if(fileAtt == INVALID_FILE_ATTRIBUTES)
		//So lets throw an exception when an error has occurred
            return false;

	//If the path referers to a directory it should also not exists.
	return ( ( fileAtt & FILE_ATTRIBUTE_DIRECTORY ) == 0 ); 
}

  bool Imager::checkInputParameters(int argc, char **argv){
        try{
              m_Arg1 = argv[1];
              m_Arg2 = argv[2];
              char  driveLetter;   
              char * drivePath = this->getLogicalDrives();
              if(drivePath!=0)
              {
                  driveLetter = *strlwr(&drivePath[4]);
              }
              else
              {
                std::cout << "*** The flash drive is not found" << std::endl;
  				std::cout << "Press Ctrl+C to close window..." << std::endl;
				std::cin >> inp;
                  return false;
               }
               if(argc != 3) 
               {
                   std::cout << "*** Arguments are not set!" << std::endl;
                   std::cout << "The command format    :  imager <flash_drive_name> <path_to_image>" << std::endl;
                   std::cout << "The example of command:  imager d c:/folder1/file1.img" << std::endl;
				   std::cout << "Press Ctrl+C to close window..." << std::endl;
				   std::cin >> inp;
				   
                   return false;
               }
               else if(!isalpha(*m_Arg1) || strlen(m_Arg1)>1)
               {
                   std::cout << "*** The first argument length > 1 or it is not from range [A-Za-z]" << std::endl;
				   std::cout << "Press Ctrl+C to close window..." << std::endl;
				   std::cin >> inp;
                   return false;
               }

               else if(driveLetter != *strlwr(m_Arg1))
               {
                   std::cout << "*** The flash drive letter is '" << driveLetter <<"' but the first argument is '"<< m_Arg1 <<"'" << std::endl;
				   std::cout << "Press Ctrl+C to close window..." << std::endl;
				   std::cin >> inp;
                   return false;
               }

               else if(!this->isFileExist(m_Arg2))
               {
                   std::cout << "*** The image file is not exist or path to file is incorrect." << std::endl;
				   std::cout << "Press Ctrl+C to close window..." << std::endl;
				   std::cin >> inp;
                   return false;
               }
	      std::cout << "1.The flash drive " << driveLetter << ": is present..." << std::endl;
		  m_file << "1.The flash drive " << driveLetter << ": is present..." << std::endl;
		  
		  
		  
              return true;
    }
  catch(std::exception& ex)
    {
        std::cout << ex.what();
		std::cout << "Press Ctrl+C to close window..." << std::endl;
		std::cin >> inp;
        return false;
    }
}
  
   bool Imager::writeImageToFlashDrive()
   {
   
    m_hVolume = INVALID_HANDLE_VALUE;
    m_hFile = INVALID_HANDLE_VALUE;
    m_hRawDisk = INVALID_HANDLE_VALUE; 
    m_SectorData = NULL;
    m_Sectorsize = 0ul;
   
            m_Status = STATUS_WRITING;
            //double mbpersec;
            unsigned long long i, lasti, availablesectors, numsectors;
            int volumeID = *m_Arg1 - 'A';
            int deviceID = static_cast<int>(m_deviceID);
            
            
            m_hVolume = getHandleOnVolume(volumeID, GENERIC_WRITE);
            if (m_hVolume == INVALID_HANDLE_VALUE)
            {
                m_Status = STATUS_IDLE;
                printf("*** Error: Invalid volume handle");
				std::cout << "Press Ctrl+C to close window..." << std::endl;
				std::cin >> inp;
                return false;
            }
            if (!getLockOnVolume(m_hVolume))
            {
                CloseHandle(m_hVolume);
                m_Status = STATUS_IDLE;
                m_hVolume = INVALID_HANDLE_VALUE;
                printf("*** Error: Unable to lock volume\n");
				std::cout << "Press Ctrl+C to close window..." << std::endl;
				std::cin >> inp;
                return false;
            }
            if (!unmountVolume(m_hVolume))
            {
                removeLockOnVolume(m_hVolume);
                CloseHandle(m_hVolume);
                m_Status = STATUS_IDLE;
                m_hVolume = INVALID_HANDLE_VALUE;
                printf("*** Error: Unable to unmount volume\n");
				std::cout << "Press Ctrl+C to close window..." << std::endl;
				std::cin >> inp;
                return false;
            }
            m_hFile = getHandleOnFile(m_Arg2, GENERIC_READ);
            if (m_hFile == INVALID_HANDLE_VALUE)
            {
                removeLockOnVolume(m_hVolume);
                CloseHandle(m_hVolume);
                m_Status = STATUS_IDLE;
                m_hVolume = INVALID_HANDLE_VALUE;
                printf("*** Error: Invalid file handle\n");
				std::cout << "Press Ctrl+C to close window..." << std::endl;
				std::cin >> inp;
           
                return false;
            }
            m_hRawDisk = getHandleOnDevice(deviceID, GENERIC_WRITE);
            if (m_hRawDisk == INVALID_HANDLE_VALUE)
            {
                removeLockOnVolume(m_hVolume);
                
                CloseHandle(m_hFile);
                CloseHandle(m_hVolume);
                m_Status = STATUS_IDLE;
                m_hVolume = INVALID_HANDLE_VALUE;
                m_hFile = INVALID_HANDLE_VALUE;
                printf("*** Error: Invalid rawdisk handle\n");
				std::cout << "Press Ctrl+C to close window..." << std::endl;
				std::cin >> inp;
                return false;
            }
            availablesectors = getNumberOfSectors(m_hRawDisk, &m_Sectorsize);
            numsectors = getFileSizeInSectors(m_hFile, m_Sectorsize);
            if (numsectors > availablesectors)
            {
              
                printf("*** Error: Not enough space on disk: Size: %1 sectors  Available: %2 sectors  Sector size: %3",numsectors,availablesectors,m_Sectorsize);
				std::cout << "Press Ctrl+C to close window..." << std::endl;
				std::cin >> inp;
                removeLockOnVolume(m_hVolume);
                CloseHandle(m_hRawDisk);
                CloseHandle(m_hFile);
                CloseHandle(m_hVolume);
                m_Status = STATUS_IDLE;
                m_hVolume = INVALID_HANDLE_VALUE;
                m_hFile = INVALID_HANDLE_VALUE;
                m_hRawDisk = INVALID_HANDLE_VALUE;
                return false;
            }
          
            lasti = 0ul;
            std::cout << "2.The image is being written to flash drive..." << std::endl;
			m_file << "2.The image is being written to flash drive..." << std::endl;
            for (i = 0ul; i < numsectors && m_Status == STATUS_WRITING; i += 1024ul)
            {
                m_SectorData = readSectorDataFromHandle(m_hFile, i, (numsectors - i >= 1024ul) ? 1024ul:(numsectors - i), m_Sectorsize);
                    
                if (m_SectorData == NULL)
                {
                    delete m_SectorData;
                    removeLockOnVolume(m_hVolume);
                    CloseHandle(m_hRawDisk);
                    CloseHandle(m_hFile);
                    CloseHandle(m_hVolume);
                    m_Status = STATUS_IDLE;
                    m_SectorData = NULL;
                    m_hRawDisk = INVALID_HANDLE_VALUE;
                    m_hFile = INVALID_HANDLE_VALUE;
                    m_hVolume = INVALID_HANDLE_VALUE;
                    return false;
                }
                if (!writeSectorDataToHandle(m_hRawDisk, m_SectorData, i, (numsectors - i >= 1024ul) ? 1024ul:(numsectors - i), m_Sectorsize))
                {
                    delete m_SectorData;
                    removeLockOnVolume(m_hVolume);
                    CloseHandle(m_hRawDisk);
                    CloseHandle(m_hFile);
                    CloseHandle(m_hVolume);
                    m_Status = STATUS_IDLE;
                    m_SectorData = NULL;
                    m_hRawDisk = INVALID_HANDLE_VALUE;
                    m_hFile = INVALID_HANDLE_VALUE;
                    m_hVolume = INVALID_HANDLE_VALUE;
                    return false;
                }
                delete m_SectorData;
                m_SectorData = NULL;
              
            }
            removeLockOnVolume(m_hVolume);
            CloseHandle(m_hRawDisk);
            CloseHandle(m_hFile);
            CloseHandle(m_hVolume);
            m_SectorData = NULL;
            m_hRawDisk = INVALID_HANDLE_VALUE;
            m_hFile = INVALID_HANDLE_VALUE;
            m_hVolume = INVALID_HANDLE_VALUE;
            std::cout << "3.Success!" << std::endl;
			m_file << "3.Success!" << std::endl;
       return true;
   }
   
