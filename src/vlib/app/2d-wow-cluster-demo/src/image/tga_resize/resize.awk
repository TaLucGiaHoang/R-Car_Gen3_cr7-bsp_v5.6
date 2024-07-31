#!/usr/bin/gawk -f
BEGIN{
    system("cp -v ../tga/*.tga .")
}
{
#   print "Hello",$12

    print "Processing",$12
    compress=""
    if ( $5=="RLE" ) {
        compress="-compress RLE"
    } 
    input=$0
    command="convert " $12  " -resize " $8 "%x" $9 "% -flip " compress " -set filename:base \"%[basename]\" \"%[filename:base].tga\""
#    print "command=", command
    system(command)
    size=  $2 "x" $3
    name=$12
    "identify " $12 |& getline
    if (size != $3) {
        print "size mismatch",name,size, $3
        print command
        print input
	print "Error resizing stopped."
	exit 1
    }
}

