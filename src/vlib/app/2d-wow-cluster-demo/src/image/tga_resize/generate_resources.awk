#!/usr/bin/gawk -f
!/^#/{
    print "Generating resources for ", $2
    system("tgadataconvert -i " $1 " -s " $2 " -c " $3 " -o " $4 " -b " $5 " -r")
}


