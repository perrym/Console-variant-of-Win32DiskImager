/* 
 * File:   main.cpp
 * Author: Gornitsky
 *
 * Created on 22 Mar 2013 , 02:17
 */

#include "include/imager.h"

/*
 * 
 */
int main(int argc, char** argv) 
{
    Imager *imgr = new Imager();
    if(imgr->checkInputParameters(argc, argv))
    {
        imgr->writeImageToFlashDrive();
     }
    delete imgr;
}

