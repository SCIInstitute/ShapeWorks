#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkResampleImageFilter.h"
#include "itkIdentityTransform.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "string.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>


#include "OptionParser.h"

optparse::OptionParser buildParser()
{
    const std::string usage = "%prog [OPTION]";
    const std::string version = "%prog 0.1";
    const std::string desc = "A command line tool that o compute largest bounding box size given a set of images...";
    //const std::string epilog = "note: --numthreads 0 means use system default (usually max number supported).\n";
    const std::string epilog = "";

    optparse::OptionParser parser = optparse::OptionParser()
            .usage(usage)
            .version(version)
            .description(desc)
            .epilog(epilog);

    parser.add_option("--inFilename").action("store").type("string").set_default("").help("A text file with the file names for which the largest size has to be computed.");
    parser.add_option("--outPrefix").action("store").type("string").set_default("").help("Output prefix to be used to save the parameters for the estimated bounding box.");
    parser.add_option("--paddingSize").action("store").type("int").set_default(0).help("Number of extra voxels in each direction to pad the largest bounding box, checks agains min image size is performed to make sure that this padding won't get out of bounds for the smallest image in the file names provides");

    return parser;
}



//this has been added for image region iteration as the image was not updated even after set
template<typename TImage>
void DeepCopy(typename TImage::Pointer input, typename TImage::Pointer output)
{
    output->SetRegions(input->GetLargestPossibleRegion());
    output->Allocate();

    itk::ImageRegionConstIterator<TImage> inputIterator(input, input->GetLargestPossibleRegion());
    itk::ImageRegionIterator<TImage> outputIterator(output, output->GetLargestPossibleRegion());

    while(!inputIterator.IsAtEnd())
    {
        outputIterator.Set(inputIterator.Get());
        ++inputIterator;
        ++outputIterator;
    }
}

template<typename InImageType, typename OutImageType>
void DeepCopy2(typename InImageType::Pointer input, typename OutImageType::Pointer output)
{
    output->SetRegions(input->GetLargestPossibleRegion());
    output->Allocate();

    itk::ImageRegionConstIterator<InImageType> inputIterator(input, input->GetLargestPossibleRegion());
    itk::ImageRegionIterator<OutImageType> outputIterator(output, output->GetLargestPossibleRegion());

    while(!inputIterator.IsAtEnd())
    {
        outputIterator.Set(inputIterator.Get());
        ++inputIterator;
        ++outputIterator;
    }
}

template<typename InImageType, typename OutImageType>
void ThresholdImage(typename InImageType::Pointer input, typename OutImageType::Pointer output, double threshold)
{
    output->SetRegions(input->GetLargestPossibleRegion());
    output->Allocate();

    itk::ImageRegionConstIterator<InImageType> inputIterator(input, input->GetLargestPossibleRegion());
    itk::ImageRegionIterator<OutImageType> outputIterator(output, output->GetLargestPossibleRegion());

    while(!inputIterator.IsAtEnd())
    {
        if (inputIterator.Get() > threshold)
            outputIterator.Set(1);
        else
            outputIterator.Set(0);
        ++inputIterator;
        ++outputIterator;
    }
}

int main( int argc, char * argv[] )
{
    optparse::OptionParser parser = buildParser();
    optparse::Values & options = parser.parse_args(argc,argv);
    std::vector<std::string> args = parser.args();

    if(argc < 3)
    {
        parser.print_help();
        return EXIT_FAILURE;
    }

    std::string inFilename    = (std::string) options.get("inFilename");
    std::string outPrefix   = (std::string) options.get("outPrefix");
    int         paddingSize   = (int) options.get("paddingSize");


    std::ifstream myfile;
    std::string line;
    myfile.open (inFilename.c_str());

    int bb[3]={0,0,0};
    int smallestIndex[3] = {1e6,1e6,1e6};
    int largestIndex[3]  = {0,0,0};

    std::vector<int> smallestIndex0_store;  smallestIndex0_store.clear();
    std::vector<int> smallestIndex1_store;  smallestIndex1_store.clear();
    std::vector<int> smallestIndex2_store;  smallestIndex2_store.clear();

    std::vector<int> largestIndex0_store;  largestIndex0_store.clear();
    std::vector<int> largestIndex1_store;  largestIndex1_store.clear();
    std::vector<int> largestIndex2_store;  largestIndex2_store.clear();

    std::vector<int> bb0_store;  bb0_store.clear();
    std::vector<int> bb1_store;  bb1_store.clear();
    std::vector<int> bb2_store;  bb2_store.clear();

    std::vector<int> volume_store;  volume_store.clear();

    std::vector<std::string> filenames ; filenames.clear();

    int minXsize = 1e6, minYsize = 1e6, minZsize = 1e6; // for padding

    if (myfile.is_open())
    {

        while ( getline (myfile,line) )
        {

            int cur_bb[3]={0,0,0};
            int cur_smallestIndex[3] = {1e6,1e6,1e6};
            int cur_largestIndex[3]  = {0,0,0};

            //typedef   unsigned char InputPixelType;
            //typedef   unsigned char OutputPixelType;

            // ANTs output float-type images even with nearest neighbor resampling
            typedef   float InputPixelType;
            typedef   unsigned char OutputPixelType;

            const     unsigned int    Dimension = 3;
            typedef itk::Image< InputPixelType,    Dimension >   InputImageType;
            typedef itk::ImageFileReader< InputImageType  >  ReaderType;
            ReaderType::Pointer reader = ReaderType::New();
            std::cout<<"Processing: " << line<<std::endl;
            reader->SetFileName( line );

            filenames.push_back(line);
            try
            {
                reader->Update();
            }
            catch( itk::ExceptionObject & excep )
            {
                std::cerr << "Exception caught!" << std::endl;
                std::cerr << excep << std::endl;
            }

            int curXsize = reader->GetOutput()->GetLargestPossibleRegion().GetSize()[0];
            int curYsize = reader->GetOutput()->GetLargestPossibleRegion().GetSize()[1];
            int curZsize = reader->GetOutput()->GetLargestPossibleRegion().GetSize()[2];

            minXsize = std::min(minXsize, curXsize);
            minYsize = std::min(minYsize, curYsize);
            minZsize = std::min(minZsize, curZsize);

            InputImageType::Pointer inputImage = reader->GetOutput();
            itk::ImageRegionIteratorWithIndex<InputImageType> imageIterator(inputImage, inputImage->GetLargestPossibleRegion());

            int cur_volume = 0;
            while(!imageIterator.IsAtEnd())
            {
                InputPixelType val = imageIterator.Get();

                if(val == 1)
                {
                    cur_smallestIndex[0] = std::min(cur_smallestIndex[0], (int) imageIterator.GetIndex()[0]);
                    cur_smallestIndex[1] = std::min(cur_smallestIndex[1], (int) imageIterator.GetIndex()[1]);
                    cur_smallestIndex[2] = std::min(cur_smallestIndex[2], (int) imageIterator.GetIndex()[2]);

                    cur_largestIndex[0] = std::max(cur_largestIndex[0], (int)imageIterator.GetIndex()[0]);
                    cur_largestIndex[1] = std::max(cur_largestIndex[1], (int)imageIterator.GetIndex()[1]);
                    cur_largestIndex[2] = std::max(cur_largestIndex[2], (int)imageIterator.GetIndex()[2]);

                    cur_volume++;
                }

                ++imageIterator;
            }

            smallestIndex[0] = std::min(smallestIndex[0], cur_smallestIndex[0]);
            smallestIndex[1] = std::min(smallestIndex[1], cur_smallestIndex[1]);
            smallestIndex[2] = std::min(smallestIndex[2], cur_smallestIndex[2]);

            largestIndex[0] = std::max(largestIndex[0], cur_largestIndex[0]);
            largestIndex[1] = std::max(largestIndex[1], cur_largestIndex[1]);
            largestIndex[2] = std::max(largestIndex[2], cur_largestIndex[2]);

            cur_bb[0] = cur_largestIndex[0] - cur_smallestIndex[0];
            cur_bb[1] = cur_largestIndex[1] - cur_smallestIndex[1];
            cur_bb[2] = cur_largestIndex[2] - cur_smallestIndex[2];

            smallestIndex0_store.push_back(cur_smallestIndex[0]);
            smallestIndex1_store.push_back(cur_smallestIndex[1]);
            smallestIndex2_store.push_back(cur_smallestIndex[2]);

            largestIndex0_store.push_back(cur_largestIndex[0]);
            largestIndex1_store.push_back(cur_largestIndex[1]);
            largestIndex2_store.push_back(cur_largestIndex[2]);

            bb0_store.push_back(cur_bb[0]);
            bb1_store.push_back(cur_bb[1]);
            bb2_store.push_back(cur_bb[2]);

            volume_store.push_back(cur_volume);
        }

        // padding
        smallestIndex[0] = std::max(0,smallestIndex[0] - paddingSize);
        smallestIndex[1] = std::max(0,smallestIndex[1] - paddingSize);
        smallestIndex[2] = std::max(0,smallestIndex[2] - paddingSize);

        largestIndex[0] = std::min(largestIndex[0] + paddingSize, minXsize-1);
        largestIndex[1] = std::min(largestIndex[1] + paddingSize, minYsize-1);
        largestIndex[2] = std::min(largestIndex[2] + paddingSize, minZsize-1);

        bb[0] = largestIndex[0] - smallestIndex[0];
        bb[1] = largestIndex[1] - smallestIndex[1];
        bb[2] = largestIndex[2] - smallestIndex[2];

        std::cout<<"largest bounding box is: "<<"bb[0]:"<<bb[0]<<" bb[1]:"<<bb[1]<<" bb[2]:"<<bb[2]<<std::endl;
        std::cout<<"Smallest index is: "<<"smallestIndex[0]:"<<smallestIndex[0]<<" smallestIndex[1]:"<<smallestIndex[1]<<" smallestIndex[2]:"<<smallestIndex[2]<<std::endl;
        std::cout<<"Largest index is: "<<"largestIndex[0]:"<<largestIndex[0]<<" largestIndex[1]:"<<largestIndex[1]<<" largestIndex[2]:"<<largestIndex[2]<<std::endl;

        myfile.close();

        std::ofstream outfile;

        outfile.open(std::string(outPrefix + "_per_file_wo_padding.txt" ).c_str());
        outfile << "filename" << ","
                << "bb0" << "," << "bb1" << "," << "bb2" << ","
                << "smallestIndex0" << "," << "smallestIndex1" << "," << "smallestIndex2" << ","
                << "largestIndex0" << "," << "largestIndex1" << "," << "largestIndex2" << "," << "volume" << "\n";
        for(unsigned int ii = 0 ; ii < filenames.size(); ii++)
            outfile << filenames[ii] << ","
                    << bb0_store[ii] << "," << bb1_store[ii] << "," << bb2_store[ii] << ","
                    << smallestIndex0_store[ii] << "," << smallestIndex1_store[ii] << "," << smallestIndex2_store[ii] << ","
                    << largestIndex0_store[ii] << "," << largestIndex1_store[ii] << "," << largestIndex2_store[ii] << ","
                    << volume_store[ii] << "\n";
        outfile.close();


        outfile.open(std::string(outPrefix + "_bb0.txt" ).c_str());
        outfile << bb[0];
        outfile.close();

        outfile.open(std::string(outPrefix + "_bb1.txt" ).c_str());
        outfile << bb[1];
        outfile.close();

        outfile.open(std::string(outPrefix + "_bb2.txt" ).c_str());
        outfile << bb[2];
        outfile.close();

        outfile.open(std::string(outPrefix + "_smallestIndex0.txt" ).c_str());
        outfile << smallestIndex[0];
        outfile.close();

        outfile.open(std::string(outPrefix + "_smallestIndex1.txt").c_str());
        outfile << smallestIndex[1];
        outfile.close();

        outfile.open(std::string(outPrefix + "_smallestIndex2.txt" ).c_str());
        outfile << smallestIndex[2];
        outfile.close();

        outfile.open(std::string(outPrefix + "_largestIndex0.txt" ).c_str());
        outfile << largestIndex[0];
        outfile.close();

        outfile.open(std::string(outPrefix + "_largestIndex1.txt" ).c_str());
        outfile << largestIndex[1];
        outfile.close();

        outfile.open(std::string(outPrefix + "_largestIndex2.txt" ).c_str());
        outfile << largestIndex[2];
        outfile.close();

        outfile.open(std::string(outPrefix + "_bb_params.txt" ).c_str());
        outfile<<"largest bounding box is: "<<"bb[0]:"<<bb[0]<<" bb[1]:"<<bb[1]<<" bb[2]:"<<bb[2]<<std::endl;
        outfile<<"Smallest index is: "<<"smallestIndex[0]:"<<smallestIndex[0]<<" smallestIndex[1]:"<<smallestIndex[1]<<" smallestIndex[2]:"<<smallestIndex[2]<<std::endl;
        outfile<<"Largest index is: "<<"largestIndex[0]:"<<largestIndex[0]<<" largestIndex[1]:"<<largestIndex[1]<<" largestIndex[2]:"<<largestIndex[2]<<std::endl;
        outfile.close();

    }

    return EXIT_SUCCESS;

}
