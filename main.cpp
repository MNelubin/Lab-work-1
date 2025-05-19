/* Maxim Nelyubin st132907@student.spbu.ru
    Lab-1
*/

#include "ImD.h"
#include <chrono>

//"examples_BMP/bmp_24.bmp"

//"examples_BMP/sample1.bmp"

int main()
{
    std::string file_name;
    bool C;
    bool ConC;
    bool Gaus = false;
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


    ImD *Base= new ImD;
    Base->reader(file_name,C,ConC,Gaus);

    // Sequential methods timing
    auto start_seq = std::chrono::high_resolution_clock::now();
    Base->rotate90ContClockwise(file_name + "_rotContC.bmp");
    Base->reader(file_name,C,ConC,Gaus);
    Base->rotate90Clockwise(file_name+"_rotC.bmp",true);
    ImD *test= new ImD(*Base);
    test->applyGaussianFilter(file_name);
    auto end_seq = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_seq = end_seq - start_seq;
    std::cout << "Sequential methods time: " << duration_seq.count() << " seconds\n";
    delete Base;
    // Parallel methods timing
    Base = new ImD;
    Base->reader(file_name,C,ConC,Gaus);
    auto start_par = std::chrono::high_resolution_clock::now();

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            ImD* img1 = new ImD(*Base);
            img1->rotate90ContClockwise_parallel(file_name + "_rotContC_parallel.bmp");
            delete img1;
        }

        #pragma omp section
        {
            ImD* img2 = new ImD(*Base);
            img2->rotate90Clockwise_parallel(file_name+"_rotC_parallel.bmp",true);
            delete img2;
        }

        #pragma omp section
        {
            ImD* img3 = new ImD(*Base);
            img3->applyGaussianFilter_parallel(file_name + "_WGaus_parallel.bmp");
            delete img3;
        }
    }

    auto end_par = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_par = end_par - start_par;
    std::cout << "Parallel methods time: " << duration_par.count() << " seconds\n";
    delete Base;

    return 0;
}
