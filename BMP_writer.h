/* Maxim Nelyubin st132907@student.spbu.ru
    Lab-1
*/
#ifndef BMP_WRITER
#define BMP_WRITER

#include <iostream>
#include <fstream>

#include "string" 
#include "vector"

int a;

void writer(std::string fileName, unsigned char dat)
{
    std::ofstream f_w(fileName, std::ofstream::binary|std::ios::app);
    f_w<<dat;
}
#endif