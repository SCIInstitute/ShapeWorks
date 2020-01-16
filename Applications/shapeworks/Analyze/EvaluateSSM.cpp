//
// Created by karthik on 1/16/20.
//

#include "EvaluateSSM.h"

namespace shapeworks {
void EvaluateSSM::buildParser()
{
    const std::string prog = "evaluatessm";
    const std::string desc = "Evaluates a SSM based on generality, specificity and compactness";
    parser.prog(prog).description(desc);

    parser.add_option("--filelist").action("store").type("string").set_default("").help("A file containing a list of shape particle filenames separated by newlines");

    Command::buildParser();
}

int EvaluateSSM::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
    const std::string fname_filelist = static_cast<std::string>(options.get("filelist"));
    std::cout << "File list: " << fname_filelist << std::endl;
}
}


