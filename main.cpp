/* Maxim Nelyubin st132907@student.spbu.ru
    Lab-1
*/

#include "ImD.h"

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
    /*
    ImD Base;
    Base.reader(file_name,C,ConC,Gaus);
    Base.applyGaussianFilter(file_name);
    */
    
    ImD *Base= new ImD;
    Base->reader(file_name,C,ConC,Gaus);
    Base->rotate90ContClockwise(file_name + "_rotContC.bmp");
    Base->reader(file_name,C,ConC,Gaus);
    Base->rotate90Clockwise(file_name+"_rotC.bmp",true);
    ImD *test= new ImD(*Base);
    delete Base;
    test->applyGaussianFilter(file_name);
    delete test;	
    
    return 0;
}
