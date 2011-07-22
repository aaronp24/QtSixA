/*
 * sixad-raw (Well, actually the same thing as sixad-uinput-sixaxis, but for hidraw devices instead of bluetooth)
 * written by falkTX, 2009  (used some code from xsixhidtest.c)
 *
 * Compile with: gcc -Wall sixad-raw.c -o sixad-raw
 * run with: "sixad-raw /dev/hidrawX"
 *
*/

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <linux/input.h>
#include <linux/joystick.h>
#include <linux/uinput.h>

int enable_buttons;
int enable_sbuttons;
int enable_axis;
int enable_accel;
int enable_accon;
int enable_speed;
int enable_pos;
int ufd;

int just_started = 1;

struct device_info {
	unsigned int vendor;
	unsigned int product;
	unsigned int version;
};

struct state {
  double time;
  int ax, ay, az;       // Raw accelerometer data
  double ddx, ddy, ddz; // Acceleration
  double dx, dy, dz;    // Speed
  double x, y, z;       // Position
};

struct state prev;
struct state newH;
struct timeval tv;

/* UInput */
static char *uinput_filename[] = {"/dev/uinput", "/dev/input/uinput",
                           "/dev/misc/uinput"};
#define UINPUT_FILENAME_COUNT (sizeof(uinput_filename)/sizeof(char *))

int uinput_open(int enable_axis, int enable_accel)
{
    unsigned int i;
    int fd = -1;
    
    struct uinput_user_dev dev;
    
    /* Open uinput device */
    for (i=0; i < UINPUT_FILENAME_COUNT; i++) {
	if ((fd = open(uinput_filename[i], O_RDWR)) >= 0) {
	    break;
	}
    }
    
    ufd = fd;
    
    if (fd < 0) {
	fprintf(stderr, "Unable to open uinput\n");
	return -1;
	exit(-1);
    }
    
    memset(&dev, 0, sizeof(dev));
    
    dev.id.vendor = 0x054c;
    dev.id.product = 0x0268;
    dev.id.version = 0x0100;
    dev.id.bustype = BUS_USB;
		
    strncpy (dev.name, "PLAYSTATION(R)3 Controller (hidraw)", UINPUT_MAX_NAME_SIZE);

    ioctl (fd, UI_SET_EVBIT, EV_SYN);
    ioctl (fd, UI_SET_EVBIT, EV_ABS);
    ioctl (fd, UI_SET_EVBIT, EV_KEY);
    
    for (i = 0; i < 29; i++) {
        if (i >= 0 && i <= 3 && enable_axis == 1) { //left & right stick axis
            dev.absmax[i] = 127;
            dev.absmin[i] = -127;
        } else if (i == 4 && enable_accel == 1) { //accelerometer X (reversed)
            dev.absmax[i] = -402;
            dev.absmin[i] = -622;
        } else if (i == 5 && enable_accel == 1) { //accelerometer Y
            dev.absmax[i] = 622;
            dev.absmin[i] = 402;
        } else if (i == 6 && enable_accel == 1) { //accelerometer Z
	  dev.absmax[i] = 622;
	  dev.absmin[i] = 402;
//      } else if (i == 7 && enable_accel == 1) { //Gyro
// 	dev.absmax[i] = ???;
// 	dev.absmin[i] = ???;
	} else if (i >= 8 && i <= 19) { //buttons
            dev.absmax[i] = 255;
            dev.absmin[i] = -255;
        } else if (i >= 20 && i <= 28) { //acceleration/speed/position
	    dev.absmax[i] = 1250;
	    dev.absmin[i] = -1250;
	} else {
            dev.absmax[i] = 32767;
            dev.absmin[i] = -32767;
	}

	if (ioctl (fd, UI_SET_ABSBIT, i) < 0) {
	    fprintf(stderr, "error on uinput ioctl (UI_SET_ABSBIT)\n");
	    return -1;
	}
    }

    for (i = 0; i < 17; i++) { 
	if (ioctl(fd, UI_SET_KEYBIT, BTN_JOYSTICK + i) < 0) {
	    fprintf(stderr, "error on uinput ioctl (UI_SET_KEYBIT)\n");
	    return -1;
	}
    }
	
    
    if (write(fd, &dev, sizeof(dev)) != sizeof(dev)) {
	fprintf(stderr, "Error on uinput device setup\n");
	close(fd);
	return -1;
    }

    if (ioctl(fd, UI_DEV_CREATE) < 0) {
	fprintf(stderr, "Error on uinput dev create");
	close(fd);
	return -1;
    }

    return fd;

}

int uinput_close(int fd)
{
    if (ioctl(fd, UI_DEV_DESTROY) < 0) {
	fprintf(stderr, "Error on uinput ioctl (UI_DEV_DESTROY)\n");
    }

    if (close(fd)) {
	fprintf(stderr, "Error on uinput close");
	return -1;
    }

    return 0;
}

int uinput_send(int fd, __u16 type, __u16 code, __s32 value)
{
    struct input_event event;

    memset(&event, 0, sizeof(event));
    event.type = type;
    event.code = code;
    event.value = value;
    gettimeofday(&(event.time), NULL);

    if (write(fd, &event, sizeof(event)) != sizeof(event)) {
	fprintf(stderr, "Error on send_event\n");
	return -1;
    }

    return 0;
}

int main(int argc, char **argv) {
  
  struct state prev;
  memset(&prev, 0, sizeof(prev));
  
  unsigned char buf[128];
  int fd, nr, msg = 0;
  
  if (argc < 2) {
	printf("Usage: sixad-raw /dev/hidrawX\n");
	exit(-1);
  }
  
  if ((fd = open(argv[1], O_RDONLY)) < 0) { 
      fprintf(stderr, "sixad-raw");
      exit(-1);
  }
    
  if (argc > 2) {
    if (atoi(argv[2]) == 3654) { //hidden codename for testing
	nr=read(fd, buf, sizeof(buf));
	if ( nr < 0 ) { printf("Error: Not a device\n"); exit(1); }
	if ( nr == 50 ) { printf("Found a Sixaxis\n"); exit(1); }
	else { printf("Error: Not a Sixaxis\n"); exit(1); }
	exit(0);
    }
  }
  
  int rfile[7];
  FILE *f = popen(". /etc/default/sixad; echo $Enable_buttons $Enable_sbuttons $Enable_axis $Enable_accel $Enable_accon $Enable_speed $Enable_pos", "r");
  if ( !f || fscanf(f, "%i %i %i %i %i %i %i", &rfile[0], &rfile[1], &rfile[2], &rfile[3], &rfile[4], &rfile[5], &rfile[6]) != 7 ) {
      printf("No valid sixad configuration file found, using default settings\n");
      enable_buttons = 1;
      enable_sbuttons = 1;
      enable_axis = 1;
      enable_accel = 1;
      enable_accon = 0;
      enable_speed = 0;
      enable_pos = 0;
  } else {
      enable_buttons = rfile[0];
      enable_sbuttons = rfile[1];
      enable_axis = rfile[2];
      enable_accel = rfile[3];
      enable_accon = rfile[4];
      enable_speed = rfile[5];
      enable_pos = rfile[6];
  }
  pclose(f);
  
  uinput_open(enable_axis, enable_accel);
  
  while ( (nr=read(fd, buf, sizeof(buf))) ) {
	if ( nr < 0 ) { perror("Error when opening file"); exit(1); }
	if ( nr != 50 ) { printf("Error: Not a Sixaxis!\n"); exit(1); }
	
	if ( msg == 0 ) { printf("Sixaxis sucessfully initiated\n"); msg = 1; }
	
	if ( gettimeofday(&tv, NULL) ) { perror("gettimeofday"); exit(1); }
	
        newH.time = tv.tv_sec + tv.tv_usec*1e-6;
        newH.ax = buf[42]<<8 | buf[43];
        newH.ay = buf[44]<<8 | buf[45];
        newH.az = buf[46]<<8 | buf[47];
        if ( ! prev.time ) {
            prev.time = newH.time;
            prev.ax = newH.ax;
            prev.ay = newH.ay;
            prev.az = newH.az;
        }
        double dt = newH.time - prev.time; //(time constants were recuced by half)
        double rc_dd = 1.0;  // Time constant for highpass filter on acceleration
        double alpha_dd = rc_dd / (rc_dd+dt);
        newH.ddx = alpha_dd*(prev.ddx + (newH.ax-prev.ax)*0.01);
        newH.ddy = alpha_dd*(prev.ddy + (newH.ay-prev.ay)*0.01);
        newH.ddz = alpha_dd*(prev.ddz - (newH.az-prev.az)*0.01);
        double rc_d = 1.0;  // Time constant for highpass filter on speed
        double alpha_d = rc_d / (rc_d+dt);
        newH.dx = alpha_d*(prev.dx + newH.ddx*dt);
        newH.dy = alpha_d*(prev.dy + newH.ddy*dt);
        newH.dz = alpha_d*(prev.dz + newH.ddz*dt);
        double rc = 0.5;  // Time constant for highpass filter on position
        double alpha = rc / (rc+dt);
        newH.x = alpha*(prev.x + newH.dx*dt);
        newH.y = alpha*(prev.y + newH.dy*dt);
        newH.z = alpha*(prev.z + newH.dz*dt);
        prev = newH;

        int b1 = buf[3];
        int b2 = buf[4];
        int b3 = buf[5];
        int lx = buf[7] - 128;
        int ly = buf[8] - 128;
        int rx = buf[9] - 128;
        int ry = buf[10] - 128;
        int acx = buf[42]<<8 | buf[43];
        int acy = buf[44]<<8 | buf[45];
        int acz = buf[46]<<8 | buf[47];
//      int gyro = buf[48]<<8 | buf[49];
        int up = buf[15];
        int right = buf[16];
        int down = buf[17];
        int left = buf[18];
        int l2 = buf[19];
        int r2 = buf[20];
        int l1 = buf[21];
        int r1 = buf[22];
        int tri = buf[23];
        int cir = buf[24];
        int cro = buf[25];
        int squ = buf[26];
        int posX = (int)(newH.x*1000);
        int posY = (int)(newH.y*1000);
        int posZ = (int)(newH.z*1000);
        int accX = (int)(newH.ddx*1000);
        int accY = (int)(newH.ddy*1000);
        int accZ = (int)(newH.ddz*1000);
        int velX = (int)(newH.dx*1000);
        int velY = (int)(newH.dy*1000);
        int velZ = (int)(newH.dz*1000);

	//deadzones
	if (lx > -10 && lx < 10) { lx = 0; }
	if (ly > -10 && ly < 10) { ly = 0; }
	if (rx > -11 && rx < 11) { rx = 0; }
	if (ry > -11 && ry < 11) { ry = 0; }
	if (acx < -508 && acx > -516) { acx = -512; } //acx is reversed
	if (acy > 508 && acy < 516) { acy = 512; }
	if (acz > 508 && acz < 516) { acz = 512; }
 	if (posX > -30 && posX < 30) { posX = 0; }
	if (posY > -30 && posY < 30) { posY = 0; }
	if (posZ > -30 && posZ < 30) { posZ = 0; }
	if (accX > -30 && accX < 30) { accX = 0; }
	if (accY > -30 && accY < 30) { accY = 0; }
	if (accZ > -30 && accZ < 30) { accZ = 0; }
	if (velX > -30 && velX < 30) { velX = 0; }
	if (velY > -30 && velY < 30) { velY = 0; }
	if (velZ > -30 && velZ < 30) { velZ = 0; }

        // buttons
        if (enable_buttons == 1) {
            uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 0, b1 & 1);
            b1 >>= 1;
            uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 1, b1 & 1);
            b1 >>= 1;
            uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 2, b1 & 1);
            b1 >>= 1;
            uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 3, b1 & 1);
            b1 >>= 1;
            uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 4, b1 & 1);
            b1 >>= 1;
            uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 5, b1 & 1);
            b1 >>= 1;
            uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 6, b1 & 1);
            b1 >>= 1;
            uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 7, b1 & 1);
            uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 8, b2 & 1);
            b2 >>= 1;
            uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 9, b2 & 1);
            b2 >>= 1;
            uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 10, b2 & 1);
            b2 >>= 1;
            uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 11, b2 & 1);
            b2 >>= 1;
            uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 12, b2 & 1);
            b2 >>= 1;
            uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 13, b2 & 1);
            b2 >>= 1;
            uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 14, b2 & 1);
            b2 >>= 1;
            uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 15, b2 & 1);
            uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 16, b3 & 1);
        }

        //axis
        if (enable_axis == 1) {
            uinput_send(ufd, EV_ABS, 0, lx);
            uinput_send(ufd, EV_ABS, 1, ly);
            uinput_send(ufd, EV_ABS, 2, rx);
            uinput_send(ufd, EV_ABS, 3, ry);
        }

        //accelerometer RAW
        if (enable_accel == 1) {
            uinput_send(ufd, EV_ABS, 4, acx);
            uinput_send(ufd, EV_ABS, 5, acy);
            uinput_send(ufd, EV_ABS, 6, acz);
// 	    uinput_send(ufd, EV_ABS, 7, gyro);
        }

        //buttons (sensible, as axis)
        if (enable_sbuttons == 1) {
            uinput_send(ufd, EV_ABS, 8, up);
            uinput_send(ufd, EV_ABS, 9, right);
            uinput_send(ufd, EV_ABS, 10, down);
            uinput_send(ufd, EV_ABS, 11, left);
            uinput_send(ufd, EV_ABS, 12, l2);
            uinput_send(ufd, EV_ABS, 13, r2);
            uinput_send(ufd, EV_ABS, 14, l1);
            uinput_send(ufd, EV_ABS, 15, r1);
            uinput_send(ufd, EV_ABS, 16, tri);
            uinput_send(ufd, EV_ABS, 17, cir);
            uinput_send(ufd, EV_ABS, 18, cro);
            uinput_send(ufd, EV_ABS, 19, squ);
        }

        //acceleration
        if (enable_accon == 1) {
            uinput_send(ufd, EV_ABS, 20, accX);
            uinput_send(ufd, EV_ABS, 21, accY);
            uinput_send(ufd, EV_ABS, 22, accZ);
        }

        //speed
        if (enable_speed == 1) {
            uinput_send(ufd, EV_ABS, 23, velX);
            uinput_send(ufd, EV_ABS, 24, velY);
            uinput_send(ufd, EV_ABS, 25, velZ);
        }

        //position
        if (enable_pos == 1) {
            uinput_send(ufd, EV_ABS, 26, posX);
            uinput_send(ufd, EV_ABS, 27, posY);
            uinput_send(ufd, EV_ABS, 28, posZ);
        }
	
	uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
  }
  return 0;
} //The End
