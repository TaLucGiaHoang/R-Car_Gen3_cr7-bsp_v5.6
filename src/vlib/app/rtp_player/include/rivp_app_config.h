
#ifndef RIVP_APP_CONFIG_H

#define RIVP_APP_CONFIG_H

/* Large: more smooth. Little: low lentency */
#define CONFIG_OUTPUT_BUFFERRING (3u)

/* 0: non, 1: error, 2: warning, 3: info*/
#define CONFIG_LOG_LEVEL 3

/* play mode
 * 1: loop, 0: once
 */
#define CONFIG_LOOP_PLAY 1

#define CONFIG_STREAM_WIDTH (1920u)
#define CONFIG_STREAM_HEIGHT (1088u)
#define CONFIG_STREAM_LEVEL (42)
#define CONFIG_STREAM_FRAME_RATE (60)
#define CONFIG_STREAM_WITH_B_FRAME (1)

#endif //RIVP_APP_CONFIG_H
