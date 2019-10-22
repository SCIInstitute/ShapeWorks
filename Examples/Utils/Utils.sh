##################################################################################
# File:    Utils.sh
# Authors: Shireen Elhabian
# Date:    Spring 2018
# Company: SCI Institute, Univerity of Utah
# Project: CIBC 
# Purpose: Utility functions that are called from different pipeline scripts 
# Notes:
##################################################################################

imageDimension=3

##############################################################################################################################################
# Util functions ....

function GetFileExtension(){
    local filename=$1
    xbase=${filename##*/}
    xfext=${xbase##*.}
    echo $xfext
}

function GetFilePrefix(){
    local filename=$1
    xbase=${filename##*/}
    xpref=${xbase%.*}
    echo $xpref
}

function GetFilePath(){
    local filename=$1
    xpath=${filename%/*}
    echo $xpath
}

function GetFileName(){
    local filename=$1
    xbase=${filename##*/}
    echo $xbase
}

function RemoveFileExtension(){
    local filename=$1
    echo "${filename%.*}"
}

function RemoveCarriageReturn(){
    local X=$(echo $1|tr -d '\r')
    echo $X
}

function RemoveNewLine(){
    local X=$(echo $1|tr -d '\n')
    echo $X
}

function TrimSpaces(){
    local X=${1%% }
    echo $X
}

function ReadFirstLine(){
    # head takes the first lines from a file, and the -n parameter can be used to specify how many lines should be extracted:
    line=$(head -n 1 $1)
    echo $line
}

function GzipNrrdFile(){

    local nrrdfilename=$1
    echo $nrrdfilename
    
    # check if the nrrd is a raw encoding file
    unuout="$(unu head $nrrdfilename)"
    #echo $unuout
    
    # find the substring that is preceded by encoding: and sufixed by space
    substring=$(expr "$unuout" : '.*encoding:\([^space]*\).*')
    #echo $substring
    
    if [ $substring == 'r' ]
    then
        echo Found a raw-encoded file: $nrrdfilename
        
        unu save --input $nrrdfilename --output $nrrdfilename --format nrrd --endian little --encoding gzip
    fi
} 

function StringContainsOnlyNumbers(){
    if [ -z `echo $1 | tr -d "[:digit:]"` ]; then
        #echo "Input contains only numeric characters"
        echo 1;
    else
        #echo "Error: input contains non-numeric characters"
        echo 0;
    fi
}

function StringContainsOnlyCharacters(){
    if [ -z `echo $1 | tr -d "[:alpha:]"` ]; then
        #echo "Input contains alpha characters"
        echo 1;
    else
        #echo "Error: input contains non-alpha characters"
        echo 0;
    fi
}

function StringContainsBothNumbersAndCharaters(){
    if [ -z `echo $1 | tr -d "[:alnum:]"` ]; then
        #echo "Input contains alphanumeric characters"
        echo 1;
    else
        #echo "Error: input contains non-alphanumeric characters"
        echo 0;
    fi
}

function RemoveAllOccurrenceOf(){
    local intext=$1
    local toRmv=$2
    outext=${intext//$toRmv/}
    echo $outext
}


function CountNumberOfOccurrencesOf(){
    local intext=$1
    local toCnt=$2
    
    # remove all other chars and count the whats remains, like:
    res="${intext//[^$toCnt]}"
    #echo "$res"
    echo "${#res}"
}


function FileExists(){

    FILE=$1     
    if [ -f $FILE ]; then
        #echo "File $FILE exists."
        echo 1;
    else
        #echo "File $FILE does not exist."
        echo 0;
    fi
}


function EchoWithColor(){
    local message=$1
    local color=$2
    
    #  ANSI escape codes for colors
    
    local noColor='\033[0m' # No Color
    local withColor=$noColor
    
    if [[ $color == 'black' ]]
    then
        withColor='\033[0;30m'
    fi
    
    if [[ $color == 'red' ]]
    then
        withColor='\033[0;31m'
    fi
    
    if [[ $color == 'green' ]]
    then
        withColor='\033[0;32m'
    fi
    
    if [[ $color == 'orange' ]]
    then
        withColor='\033[0;33m'
    fi
    
    if [[ $color == 'blue' ]]
    then
        withColor='\033[0;34m'
    fi
    
    if [[ $color == 'purple' ]]
    then
        withColor='\033[0;35m'
    fi
    
    if [[ $color == 'cyan' ]]
    then
        withColor='\033[0;36m'
    fi
    
    if [[ $color == 'light_gray' ]]
    then
        withColor='\033[0;37m'
    fi
    
    if [[ $color == 'dark_gray' ]]
    then
        withColor='\033[1;30m'
    fi
    
    if [[ $color == 'light_red' ]]
    then
        withColor='\033[1;31m'
    fi
    
    if [[ $color == 'light_green' ]]
    then
        withColor='\033[1;32m'
    fi
    
    if [[ $color == 'yellow' ]]
    then
        withColor='\033[1;33m'
    fi
    
    if [[ $color == 'light_blue' ]]
    then
        withColor='\033[1;34m'
    fi
    
    if [[ $color == 'light_purple' ]]
    then
        withColor='\033[1;35m'
    fi
    
    if [[ $color == 'light_cyan' ]]
    then
        withColor='\033[1;36m'
    fi
    
    if [[ $color == 'white' ]]
    then
        withColor='\033[1;37m'
    fi
    
    echo -e "${withColor}${message}${noColor}"
}
 
