/* Maxim Nelyubin st132907@student.spbu.ru
    Lab-1
*/

#include "BMP_reader.h"

//"examples_BMP/bmp_24.bmp"

//"examples_BMP/sample1.bmp"

int main()
{
    std::string file_name;
    bool C;
    bool ConC;
    bool Gaus;
    std::string input;
    std::cout<<"What file path?\n";
    std::cin>>file_name;
    std::cout<<"Do you need rotated images? (1/0)\n";
    std::cin>>input;
    if (input=="0")
    {
        ConC=C=0;
    }
    else
    {
        ConC=C=1;
    }
    if (C)
    {
        std::cout<<"Do you need rotated image with Gaus filtration? (1/0)\n";
        std::cin>>input;
        if (input=="0")
        {
            Gaus=0;
        }
        else
        {
            Gaus=1;
        }
    }

    reader(file_name,C,ConC,Gaus);
    return 0;
}