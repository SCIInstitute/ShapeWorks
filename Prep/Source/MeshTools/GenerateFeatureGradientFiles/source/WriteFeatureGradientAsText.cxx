#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>

#define EXIT_FAILURE 1

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " in_file out_file" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream infile(argv[1]);
    std::ofstream outfile(argv[2]);
    if (!infile.is_open())
    {
        std::cout << "inFile Not Found" << std::endl;
        return EXIT_FAILURE;
    }
    if (!outfile.is_open())
    {
        std::cout << "outFile cannot be created" << std::endl;
        return EXIT_FAILURE;
    }

    // read # vertices
    unsigned int numVert;
    infile.read(reinterpret_cast<char *>(&numVert), sizeof(unsigned int));

    std::ostringstream ss;
    ss.flush();
    for (int i = 0; i < 100; i++)
    {
        float value;
        for (int j = 0; j < 3; j ++)
        {
            infile.read( reinterpret_cast<char *>(&value), sizeof(float) );
            float val = (float) value;
            int val1 = (int) round(val*1000);
            float val2 = (float) val1/1000.0;
            ss << val2 << "  ";
        }
        ss << std::endl;
    }
    outfile << ss.str();
    std::cout << ss.str() << std::endl;
    infile.close();
    outfile.close();
    return 0;
}
