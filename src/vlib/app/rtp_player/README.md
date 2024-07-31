# RTP Player
## Prerequisites:
- FFmpeg
  - I recommend downloading the latest static builds from https://johnvansickle.com/ffmpeg/ to stream newer video formats like AV1
  - or, `sudo apt install ffmpeg`
- A video file
  - Video file `renesas-intro.mkv` is provided but you may use any video file or capture device like a webcam (e.g. /dev/video0)
## How to use RTP Player
1. Build and flash CR7 Loader with `RCAR_KICK_MAIN_CPU=0` to prevent U-Boot/Rich OS from interfering with Ethernet HW
2. Build and flash RTP Player application on CR7 with the following options:
```bash
cmake -DCMAKE_BUILD_TYPE=Release -DETHER_ENABLE=ON -DLWIP_SUPPORTED=ON -DRIVP_SUPPORTED=ON <build-dir>
```
3. By default, RTP Player app uses IP 192.168.0.20 so PC must be connected via Ethernet and use an IP on the same subnet (e.g. 192.168.0.100)
4. Execute command on PC, replace `<video file>` with path to your video file
```bash
ffmpeg -re -i <video file> -an -c:v libx264 -preset ultrafast -bsf:v h264_mp4toannexb -bsf:v h264_metadata=aud=insert -profile:v baseline -level:v 4.2 -pix_fmt yuv420p -vf "scale=1920:1080" -f rtp rtp://192.168.0.20:4000
```
5. That's it! You should see the video output on the HDMI display.
