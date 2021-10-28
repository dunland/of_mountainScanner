#define IMAGE_WIDTH 1920
#define IMAGE_HEIGHT 1080
#define IMAGE_SCALING 0.5 // float between 0 and 1

// scan controls:
#define MAX_SCAN_ITERATIONS 1 // INFINITY if next image should only always be loaded manually
#define SCANNING_SPEED 2      // defines how many times the scanning is conducted per frame // ATTENTION: THIS MIGHT AFFECT THE SOUND DESIGN!

// send host (aka ip address)
#define HOST "localhost"

// send port
#define SENDING_PORT 9002
#define RECEIVING_PORT 9001

#define PD_NUM_OF_CHARTS 101 // iterate through the number of charts to send data to