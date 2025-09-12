// Rotations
#define CLOCKWISE 1
#define COUNTERCLOCKWISE 0

// Data status
#define READY 0b11111110 // On developement
#define NOTREADY 0b11111100 // On developement

// Requests
#define REQANGLE 0b00000001 // DONE
#define REQSTATUS 0b00000010 // On developement
#define REQCHECK 0b00000011 // On developement

// Settings
#define ANGLE 0b00010000 // DONE
//#define SPEED 0b00100000
#define STEPS 0b00110000 // On developement
#define DIR 0b01000000 // On developement

// Commands
#define ESTOP 0b11111111 // On developement
#define FULLSTEP 0b00001000 // On developement
#define HALFSTEP 0b00001001 // On developement
#define QUARTEDSTEP 0b00001011 // On developement
#define EIGHTHSTEP 0b00001100 // On developement
#define SIXTEENTHSTEP 0b00001101 // On developement