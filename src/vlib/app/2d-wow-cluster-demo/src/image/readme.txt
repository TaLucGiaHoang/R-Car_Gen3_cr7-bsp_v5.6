#This readme explains how to generate program resources from the tga images.

#set app dir
APP_DIR="/home/sven/Linux/y3.21/vlib/app/2d-wow-cluster-demo"

#resize and RLE compress the images according to the rescaling_info file
cd $APP_DIR/src/image/tga_resize
cat  ../rescaling_info|  ./resize.awk;:



# recreate bin file
#If not done yet
ln -s ../ src
mkdir ../data
ln -s ../data data

cd $APP_DIR/src/image/tga_resize
./generate_resources.awk <../image_info

# Rebuild romfs
cd $APP_DIR/src/misc

# only if image_list is not there
#Generate image list from previous romfs
cat fs_data.c | awk '/^#if 0/{stop=1}stop==0 && /^ *{/{print}' | sed 's/ *{\"//;s/\".*$//'| grep -v '^ *$' >image_list

# This generates the new romfs
cd $APP_DIR/src/image/data
cat $APP_DIR/src/misc/image_list  | $APP_DIR/src/misc/romfs/generate_flash.sh 0xf0000000

# This generates the new array
cd $APP_DIR/src/misc/
./generate_array.sh

cd $APP_DIR/src/image
#Set IMG_ATTRIBUTE_USE_FROM_PERSISTENT_MEMORY for images you like
#currently not automatically set
# this has to be set in the *.c files of the images


########################################################################################################################
#                                                                                                                      #
#                                                   create intermediate files appendix                                 #
#                                                         usually not needed                                           #
########################################################################################################################

#This procedurece explains how to create all_files file. As it is there it is not necessary
#compare old / new files (is not essential)
for i in *.c ; do cat $i  | tr -d '\r' | tr -d '"'| egrep ', *$' | sed 's/^.*RLE.*$/RLE,/'| awk -F, '/,$/{if(FNR<7) printf"%s ",$1}END{printf"\n"}';done  >files_new.txt
cd  olddir
for i in *.c ; do cat $i  | tr -d '\r' | tr -d '"'| egrep ', *$' | sed 's/^.*RLE.*$/RLE,/'| awk -F, '/,$/{if(FNR<7) printf"%s ",$1}END{printf"\n"}';done  >files_old.txt

paste files_new.txt ~/Linux/drw2d_port/2d-wow-cluster-demo.img/src/image/files_old.txt | awk '{printf"%-30s %4d %4d %15s %4s %4d %4d %f %f %s\n",$1,$2,$3,$4,$6,$8,$9,$2/$8*100,$3/$9*100,$7}' >/tmp/all_files

# remove double entry by hand 
#cluster_gear_s_off.bin 
#cluster_gear_s_on.bin 

#Generate tga names
for i  in *.c ; do grep $i  tga/create.sh | awk -v name=$i  '{print name,$3}' ;done  >/tmp/tga_names
# add RGB565 to IconCluster manually

#open "all_files" with an editor
#add line
#cluster_needle.bin              217   26    IMG_ARGB8888  (0)  119   15 182.352941 173.333333 cluster_needle.bin	cluster_needle.c MetalNeedle.tga
# to add the metal needle
# if metal needle is not needed then the line must be deleted from tga_names as it is preset there and 
# will generate problems in that case
# merger all_files with tga_names to create rescaling_info:
paste /tmp/all_files /tmp/tga_names  >rescaling_info
#delete last line

#create list of image from create.sh script is not already done. Not necessary.
cat create.sh| grep -v sed| grep -v '^ *$' | awk '{printf"%-25s %-16s %s %s %s\n", $3, $5,$7, $9,$11 }' >../image_info
