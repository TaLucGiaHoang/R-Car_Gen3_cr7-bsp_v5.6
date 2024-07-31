#generate romfs
#Generate image list from previous romfs
cd 
cat fs_data.c | awk '/^#if 0/{stop=1}stop==0 && /^ *{/{print}' | sed 's/ *{\"//;s/\".*$//'| grep -v '^ *$' >image_list

#Use image_list to create romfs

cd 2d-wow-cluster-demo/image/data