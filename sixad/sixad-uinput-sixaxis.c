/*
 *  sixad driver (based on BlueZ)
 * ------------------------------
 *  Used several code from BlueZ, and some other portions from many projects around the web
 *
 *  Written by falkTX, 2009
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <malloc.h>
#include <syslog.h>
#include <signal.h>
#include <getopt.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <linux/input.h>
#include <linux/uinput.h>

int n_six, led_n, led_js_n, ufd = -1;
int enable_leds, enable_led_anim, enable_buttons, enable_sbuttons, enable_axis;
int enable_accel, enable_accon, enable_speed, enable_pos, enable_rumble, debug;
unsigned char buf[128];

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

static inline int ppoll(struct pollfd *fds, nfds_t nfds, const struct timespec *timeout, const sigset_t *sigmask)
{
    if (timeout == NULL)
        return poll(fds, nfds, -1);
    else if (timeout->tv_sec == 0)
        return poll(fds, nfds, 500);
    else
        return poll(fds, nfds, timeout->tv_sec * 1000);
}


/* UInput */
static char *uinput_filename[] = { "/dev/uinput", "/dev/input/uinput", "/dev/misc/uinput" };
#define UINPUT_FILENAME_COUNT (sizeof(uinput_filename)/sizeof(char *))

int uinput_open_sixaxis(char *sixaxis_id, int enable_axis, int enable_accel, int enable_accon, int enable_speed, int enable_pos, int enable_rumble)
{
    unsigned int i;
    int fd = -1, ufd;
    char fullname[512];

    struct uinput_user_dev dev;

    /* Open uinput device */
    for (i=0; i < UINPUT_FILENAME_COUNT; i++) {
        if ((fd = open(uinput_filename[i], O_RDWR)) >= 0) {
            break;
        }
    }

    ufd = fd; //fd is closed later, ufd saves the value to return

    if (fd < 0) {
        fprintf(stderr, "Unable to open uinput\n");
        return -1;
    }

    memset(&dev, 0, sizeof(dev));

    dev.id.vendor = 0x054c;
    dev.id.product = 0x0268;
    dev.id.version = 0x0100;
    dev.id.bustype = BUS_BLUETOOTH;

    strcpy (fullname, "PLAYSTATION(R)3 Controller (");
    strcat (fullname, sixaxis_id);
    strcat (fullname, ")");

    strncpy (dev.name, fullname, UINPUT_MAX_NAME_SIZE);

    if (enable_rumble) {
        if (ioctl(fd, UI_SET_EVBIT, EV_FF) < 0 ||
                ioctl(fd, UI_SET_FFBIT, FF_RUMBLE) < 0) {
            fprintf(stderr, "error on uinput ioctl (FF_RUMBLE)\n");
            close(fd);
            return -1;
        }
        dev.ff_effects_max = 1;
    }

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

    return ufd;

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

static void rumble_exec(int rumble_weak_r, int rumble_strong_r)
{
//     char buf[128];
    unsigned char setrumble[] = {
        0x52, /* HIDP_TRANS_SET_REPORT | HIDP_DATA_RTYPE_OUTPUT */
        0x01,
        0x00, 0x00, 0x00, 0x00, 0x00	// rumble values
    };

    if (rumble_weak_r || rumble_strong_r) {
        setrumble[3] = setrumble[5] = 254; // stay on for a long while
        setrumble[4] = rumble_weak_r ? 255 : 0;
        int strong = 0x40 + (rumble_strong_r * 0xc8 / 65535);
        if (strong>0xff) strong = 0xff;
        setrumble[6] = strong;
    } else {
        setrumble[3] = setrumble[5] = 0; // off
        setrumble[4] = setrumble[6] = 0; // minimum rumble
    }

    send(0, setrumble, sizeof(setrumble), 0);
    recv(0, buf, sizeof(buf), 0);

}

static void enable_sixaxis(int csk, int led_n, int enable_led_anim)
{
//     char buf[1024];
    unsigned char enable[] = {
        0x53, /* HIDP_TRANS_SET_REPORT | HIDP_DATA_RTYPE_FEATURE */
        0xf4, 0x42, 0x03, 0x00, 0x00
    };
    unsigned char setleds[] = {
        0x52, /* HIDP_TRANS_SET_REPORT | HIDP_DATA_RTYPE_OUTPUT */
        0x01,
        0x00, 0x00, 0x00, 0x00, 0x00,	// rumble values
        0x00, 0x00, 0x00, 0x00, 0x1E,	// 0x10=LED1 .. 0x02=LED4
        0xff, 0x27, 0x10, 0x00, 0x32,	// LED 4
        0xff, 0x27, 0x10, 0x00, 0x32,	// LED 3
        0xff, 0x27, 0x10, 0x00, 0x32,	// LED 2
        0xff, 0x27, 0x10, 0x00, 0x32,	// LED 1
        0x00, 0x00, 0x00, 0x00, 0x00
    };
    const unsigned char ledpattern[8] = {  // last one (0x20) is "all-off", none
        0x02, 0x04, 0x08, 0x10,
        0x12, 0x14, 0x18, 0x20
    };

    if (enable_leds) {
	if (led_n < 1) {
	    n_six = 0;
	} else if (n_six > 7) {
	    n_six = 6;
	} else {
	    n_six = led_n - 1;
	}
    } else
      n_six = 7;

    /* enable reporting */
    send(csk, enable, sizeof(enable), 0);
    recv(csk, buf, sizeof(buf), 0);

    if (enable_led_anim && enable_leds)
    {
        /* Sixaxis LED animation - Way Cool!! */
        int animation = 0;
        if (enable_rumble) { rumble_exec(999, 0); } //small rumble start
        while ( animation < 4 ) {  // repeat it 4 times
            setleds[11] = ledpattern[0];
            send(csk, setleds, sizeof(setleds), 0);
            recv(csk, buf, sizeof(buf), 0);
            usleep(10000);
            setleds[11] = ledpattern[1];
            send(csk, setleds, sizeof(setleds), 0);
            recv(csk, buf, sizeof(buf), 0);
            usleep(5000);
            setleds[11] = ledpattern[2];
            send(csk, setleds, sizeof(setleds), 0);
            recv(csk, buf, sizeof(buf), 0);
            usleep(5000);
            setleds[11] = ledpattern[3];
            send(csk, setleds, sizeof(setleds), 0);
            recv(csk, buf, sizeof(buf), 0);
            usleep(10000);
            setleds[11] = ledpattern[2];
            send(csk, setleds, sizeof(setleds), 0);
            recv(csk, buf, sizeof(buf), 0);
            usleep(5000);
            setleds[11] = ledpattern[1];
            send(csk, setleds, sizeof(setleds), 0);
            recv(csk, buf, sizeof(buf), 0);
            usleep(5000);
            animation = animation + 1;
        }
        /* 2nd part of animation (animate until LED reaches selected number) */
        if (n_six == 1)
        {
            setleds[11] = ledpattern[0];
            send(csk, setleds, sizeof(setleds), 0);
            recv(csk, buf, sizeof(buf), 0);
        }
        else if (n_six == 2)
        {
            setleds[11] = ledpattern[0];
            send(csk, setleds, sizeof(setleds), 0);
            recv(csk, buf, sizeof(buf), 0);
            usleep(10000);
            setleds[11] = ledpattern[1];
            send(csk, setleds, sizeof(setleds), 0);
            recv(csk, buf, sizeof(buf), 0);
        }
        else if (n_six == 3)
        {
            setleds[11] = ledpattern[0];
            send(csk, setleds, sizeof(setleds), 0);
            recv(csk, buf, sizeof(buf), 0);
            usleep(100000);
            setleds[11] = ledpattern[1];
            send(csk, setleds, sizeof(setleds), 0);
            recv(csk, buf, sizeof(buf), 0);
            usleep(50000);
            setleds[11] = ledpattern[2];
            send(csk, setleds, sizeof(setleds), 0);
            recv(csk, buf, sizeof(buf), 0);
        }
        else if (n_six == 5)
        {
            setleds[11] = ledpattern[0];
            send(csk, setleds, sizeof(setleds), 0);
            recv(csk, buf, sizeof(buf), 0);
        }
        else if (n_six == 6)
        {
            setleds[11] = ledpattern[0];
            send(csk, setleds, sizeof(setleds), 0);
            recv(csk, buf, sizeof(buf), 0);
            usleep(100000);
            setleds[11] = ledpattern[1];
            send(csk, setleds, sizeof(setleds), 0);
            recv(csk, buf, sizeof(buf), 0);
        }
	if (enable_rumble) { rumble_exec(0, 0); } //stop rumble "animation"
    }

    /* set LEDs */
    setleds[11] = ledpattern[n_six];
    send(csk, setleds, sizeof(setleds), 0);
    recv(csk, buf, sizeof(buf), 0);
}

static void process_sixaxis(int ufd, int enable_buttons, int enable_sbuttons, int enable_axis, int enable_accel,
                            int enable_accon, int enable_speed, int enable_pos, int enable_rumble) {
    
    int br;
    while ( (br=read(1, buf, sizeof(buf))) ) {

        if ( br < 0) {
	    if (debug) syslog(LOG_ERR, "Read loop was broken on the Sixaxis process");
            return;
        }

     if (br==50 && buf[0]==0xa1 && buf[1]==0x01 && buf[2]==0x00) { //only continue if we've got a Sixaxis

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
        double dt = newH.time - prev.time; // (time constants were reduced by half)
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
        int acx = - (buf[42]<<8 | buf[43]); //reversed
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
        if (enable_buttons) {
	    //part1
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
	    //part2
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
	    //part3
            uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 16, b3 & 1);
        }

        //axis
        if (enable_axis) {
            uinput_send(ufd, EV_ABS, 0, lx);
            uinput_send(ufd, EV_ABS, 1, ly);
            uinput_send(ufd, EV_ABS, 2, rx);
            uinput_send(ufd, EV_ABS, 3, ry);
        }

        //accelerometer RAW
        if (enable_accel) {
            uinput_send(ufd, EV_ABS, 4, acx);
            uinput_send(ufd, EV_ABS, 5, acy);
            uinput_send(ufd, EV_ABS, 6, acz);
// 	    uinput_send(ufd, EV_ABS, 7, gyro);
        }

        //buttons (sensible, as axis)
        if (enable_sbuttons) {
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
        if (enable_accon) {
            uinput_send(ufd, EV_ABS, 20, accX);
            uinput_send(ufd, EV_ABS, 21, accY);
            uinput_send(ufd, EV_ABS, 22, accZ);
        }

        //speed
        if (enable_speed) {
            uinput_send(ufd, EV_ABS, 23, velX);
            uinput_send(ufd, EV_ABS, 24, velY);
            uinput_send(ufd, EV_ABS, 25, velZ);
        }

        //position
        if (enable_pos) {
            uinput_send(ufd, EV_ABS, 26, posX);
            uinput_send(ufd, EV_ABS, 27, posY);
            uinput_send(ufd, EV_ABS, 28, posZ);
        }
	
	uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
	
     } else
       if (debug) syslog(LOG_INFO, "non-Sixaxis packet received and ignored");

    }

    close(ufd);
    if (debug) syslog(LOG_ERR, "Read loop has ended");
    return;

}

int main(int argc, char *argv[])
{
    struct pollfd p[3];
    struct timespec timeout;
    sigset_t sigs;
    short events;
    int led;
    int log_option = LOG_NDELAY | LOG_PID | LOG_PERROR;
    
    if (argc < 2) {
        printf("Running %s requires 'sixad'. Please run sixad instead\n",  argv[0]);
        exit(-1);
    }
    
    //led_n bda
    led_n = atoi(argv[1]);
    if (led_n < 1) {
        led_n = 1;
    } else if (led_n > 7) {
        led_n = 7;
    }
    
    char *mac = argv[2];
    
    int sfile[13];
    FILE *s = popen(". /etc/default/sixad; echo $Enable_leds $LED_js_n $LED_n $LED_anim $Enable_buttons $Enable_sbuttons $Enable_axis $Enable_accel $Enable_accon $Enable_speed $Enable_pos $Enable_rumble $Debug", "r");
    if ( !s || fscanf(s, "%i %i %i %i %i %i %i %i %i %i %i %i %i", &sfile[0], &sfile[1], &sfile[2], &sfile[3], &sfile[4], &sfile[5], &sfile[6], &sfile[7], &sfile[8], &sfile[9], &sfile[10], &sfile[11], &sfile[12]) != 13 ) {
	if (debug) syslog(LOG_INFO, "No valid sixad configuration file found, using default settings");
	enable_leds = 1;
	led_js_n = 1;
	enable_led_anim = 1;
	enable_buttons = 1;
	enable_sbuttons = 1;
	enable_axis = 1;
	enable_accel = 1;
	enable_accon = 0;
	enable_speed = 0;
	enable_pos = 0;
	enable_rumble = 0;
	debug = 0;
    } else {
	enable_leds = sfile[0];
	led_js_n = sfile[1];
	enable_led_anim = sfile[3];
	enable_buttons = sfile[4];
	enable_sbuttons = sfile[5];
	enable_axis = sfile[6];
	enable_accel = sfile[7];
	enable_accon = sfile[8];
	enable_speed = sfile[9];
	enable_pos = sfile[10];
	enable_rumble = sfile[11];
	debug = sfile[12];
    }
    pclose(s);
    
    if (debug) openlog("sixad-debug", log_option, LOG_DAEMON);
    if (debug) syslog(LOG_INFO, "debug mode started");

    ufd = uinput_open_sixaxis(mac, enable_axis, enable_accel, enable_accon, enable_speed, enable_pos, enable_rumble);
    if (ufd < 0) {
        return -1;
    }

    if (led_js_n) { //check for js# devices
	int rfile[1];
	FILE *f = popen("ls /dev/input/ | grep js | awk \'sub(\"js\",\"\")\' | tail -n 1", "r");
	if ( !f || fscanf(f, "%i", &rfile[0]) != 1 ) {
	    led = 1;
	    if (debug) syslog(LOG_INFO, "No previous js# found, setting LED # to 1");
	} else {
	    led = rfile[0] + 2;
	    if (debug) syslog(LOG_INFO, "Previous js# were found, setting LED # to %i", led);
	}
	
	enable_sixaxis(0, led, enable_led_anim);
        pclose(f);
    } else
      enable_sixaxis(0, led_n, enable_led_anim);

    sigfillset(&sigs);
    sigdelset(&sigs, SIGCHLD);
    sigdelset(&sigs, SIGPIPE);
    sigdelset(&sigs, SIGTERM);
    sigdelset(&sigs, SIGINT);
    sigdelset(&sigs, SIGHUP);

    p[0].fd = 0;
    p[0].events = POLLIN | POLLERR | POLLHUP;

    p[1].fd = 1;
    p[1].events = POLLIN | POLLERR | POLLHUP;

    p[2].fd = ufd;
    p[2].events = POLLIN | POLLERR | POLLHUP;

    for (;;) {
        int idx = 3;
        int i;
        for (i = 0; i < idx; i++)
            p[i].revents = 0;

        timeout.tv_sec = 1;
        timeout.tv_nsec = 0;

        if (ppoll(p, idx, &timeout, &sigs) < 1)
            continue;

        if (p[1].revents & POLLIN)
            process_sixaxis(ufd, enable_buttons, enable_sbuttons, enable_axis, enable_accel, enable_accon, enable_speed, enable_pos, enable_rumble);

        events = p[0].revents | p[1].revents | p[2].revents;

        if (events & (POLLERR | POLLHUP)) {
            break;
        }
    }

    return 0;
}
