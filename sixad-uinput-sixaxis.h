/*
 *  sixad driver (based on BlueZ)
 * ------------------------------
 *  Used several code from BlueZ, and some other portions from many projects around the web
 * 
 *  Written by falkTX, 2009
 */

int nr, n_six, just_started = 1;
int enable_led_anim, enable_buttons, enable_sbuttons;
int enable_axis, enable_accel, enable_accon, enable_speed, enable_gyro;
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
//   double x, y, z;       // Position
};

struct state prev;

/* UInput */
static char *uinput_filename[] = {"/dev/uinput", "/dev/input/uinput",
                           "/dev/misc/uinput"};
#define UINPUT_FILENAME_COUNT (sizeof(uinput_filename)/sizeof(char *))

int uinput_open_sixaxis(const char * __restrict__ sixaxis_id)
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
    
    ufd = fd;
    
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

    ioctl (fd, UI_SET_EVBIT, EV_SYN);
    ioctl (fd, UI_SET_EVBIT, EV_ABS);
    ioctl (fd, UI_SET_EVBIT, EV_KEY);
    
    for (i = 0; i < 26; i++) {
      if (i >= 0 && i <= 3) { //left & right stick axis
	dev.absmax[i] = 255;
	dev.absmin[i] = 0;
      }
      else if (i == 4) { //accelerometer X (reversed)
	dev.absmax[i] = 60.5;
	dev.absmin[i] = 37;
      }	    
      else if (i == 5) { //accelerometer Y
	dev.absmax[i] = 61;
	dev.absmin[i] = 39;
      }
      else if (i == 6) { //accelerometer Z
	dev.absmax[i] = 63;
	dev.absmin[i] = 41;
      }
      else if (i == 7) { //gyro (TODO - needs work)
	dev.absmax[i] = 32767;
	dev.absmin[i] = -32767;
      }
      else if (i >= 8 && i <= 19) { //buttons
	dev.absmax[i] = 255;
	dev.absmin[i] = -255;
      }
      else if (i >= 20 && i <= 25) { //acceleration and speed
	dev.absmax[i] = 25;
	dev.absmin[i] = -25;
      }
      else
      {
	dev.absmax[i] = 32767;
	dev.absmin[i] = -32767;
      }
      
    if (ioctl (fd, UI_SET_ABSBIT, i) < 0) {
	    fprintf(stderr, "error on uinput ioctl (UI_SET_ABSBIT)\n");
	    return -1;
	}
    }

    for (i = 0; i < 21; i++) { 
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

static void enable_sixaxis(int csk, int led_n, int enable_led_anim)
{
  	char buf[1024];
	unsigned char enable[] = { 
		0x53, /* HIDP_TRANS_SET_REPORT | HIDP_DATA_RTYPE_FEATURE */
		0xf4, 0x42, 0x03, 0x00, 0x00 };
	unsigned char setleds[] = {
		0x52, /* HIDP_TRANS_SET_REPORT | HIDP_DATA_RTYPE_OUTPUT */
		0x01,
		0x00, 0x00, 0x00, 0x00, 0x00,	// rumble values
		0x00, 0x00, 0x00, 0x00, 0x1E,	// 0x10=LED1 .. 0x02=LED4
		0xff, 0x27, 0x10, 0x00, 0x32,	// LED 4
		0xff, 0x27, 0x10, 0x00, 0x32,	// LED 3
		0xff, 0x27, 0x10, 0x00, 0x32,	// LED 2
		0xff, 0x27, 0x10, 0x00, 0x32,	// LED 1
		0x00, 0x00, 0x00, 0x00, 0x00,
	};
	const unsigned char ledpattern[8] = {  // last one (0x20) is "all-off", none
	  0x02, 0x04, 0x08, 0x10,
	  0x12, 0x14, 0x18, 0x20
	};

	if (led_n == -1)
	  n_six = 7;
	else
	  n_six = led_n - 1;
	
	/* enable reporting */
	send(csk, enable, sizeof(enable), 0);
	recv(csk, buf, sizeof(buf), 0);

	if (enable_led_anim == 1 && led_n != -1)
	{
	  /* Sixaxis LED animation - Way Cool!! */
	  int animation;
	  animation = 0;
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
	}
	
	/* set LEDs */
	setleds[11] = ledpattern[n_six];
	send(csk, setleds, sizeof(setleds), 0);
	recv(csk, buf, sizeof(buf), 0);
}

void fatal(const char *msg) { perror(msg); exit(1); }

static void process_sixaxis(int ufd, int buf_int, int enable_buttons, int enable_sbuttons, int enable_axis,
				      int enable_accel, int enable_accon, int enable_speed, int enable_gyro) {

  while ( (nr=read(buf_int, buf, sizeof(buf))) ) {
    
    if ( nr < 0 ) { return; }
    
    struct state new;
    struct timeval tv;
    if ( gettimeofday(&tv, NULL) ) fatal("gettimeofday");
    new.time = tv.tv_sec + tv.tv_usec*1e-6;
    new.ax = buf[42]<<8 | buf[43];
    new.ay = buf[44]<<8 | buf[45];
    new.az = buf[46]<<8 | buf[47];
    if ( ! prev.time ) {
      prev.time = new.time;
      prev.ax = new.ax;
      prev.ay = new.ay;
      prev.az = new.az;
    }
    double dt = new.time - prev.time;
    double rc_dd = 0.2;  // Time constant for highpass filter on acceleration
    double alpha_dd = rc_dd / (rc_dd+dt);
    new.ddx = alpha_dd*(prev.ddx + (new.ax-prev.ax)*0.01);
    new.ddy = alpha_dd*(prev.ddy + (new.ay-prev.ay)*0.01);
    new.ddz = alpha_dd*(prev.ddz - (new.az-prev.az)*0.01);
    double rc_d = 0.2;  // Time constant for highpass filter on speed
    double alpha_d = rc_d / (rc_d+dt);
    new.dx = alpha_d*(prev.dx + new.ddx*dt);
    new.dy = alpha_d*(prev.dy + new.ddy*dt);
    new.dz = alpha_d*(prev.dz + new.ddz*dt);
    prev = new;
    
    int b1 = buf[3];
    int b2 = buf[4];
    int b3 = buf[5];
    int lx = buf[7];
    int ly = buf[8];
    int rx = buf[9];
    int ry = buf[10];
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
    int posX = abs(((buf[42]<<8 |  buf[43]) / 10) - 100);
    int posY = abs((buf[44]<<8 | buf[45]) / 10);
    int posZ = abs((buf[46]<<8 | buf[47]) / 10);
    int accX = (int)(new.ddx*20);
    int accY = (int)(new.ddy*20);
    int accZ = (int)(new.ddz*20);
    int velX = (int)(new.dx*200);
    int velY = (int)(new.dy*200);
    int velZ = (int)(new.dz*200);
    int gyro = (buf[48]<<8 | buf[49]) * 10000;  //gyro ### TODO - needs something else here...

    //center axis
    if (lx > 120 && lx < 140) { lx = 127.5; }
    if (ly > 120 && ly < 140) { ly = 127.5; }
    if (rx > 120 && rx < 140) { rx = 127.5; }
    if (ry > 120 && ry < 140) { ry = 127.5; }
    
  // start axis at 0 (if not set they start at a random value)
  if (just_started == 1) {
    uinput_send(ufd, EV_ABS, 0, 127.5);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 1, 127.5);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 2, 127.5);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 3, 127.5);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 4, 49);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 5, 50);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 6, 52);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 7, 0);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 8, 0);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 9, 0);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 10, 0);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 11, 0);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 12, 0);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 13, 0);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 14, 0);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 15, 0);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 16, 0);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 17, 0);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 18, 0);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 19, 0);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 20, 0);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 21, 0);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 22, 0);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 23, 0);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 24, 0);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 25, 0);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    just_started = 0;
  }
    
    // buttons
  if (enable_buttons == 1) {
    uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 0, b1 & 1);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0); b1 >>= 1;
    uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 1, b1 & 1);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0); b1 >>= 1;
    uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 2, b1 & 1);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0); b1 >>= 1;
    uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 3, b1 & 1);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0); b1 >>= 1;
    uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 4, b1 & 1);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0); b1 >>= 1;
    uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 5, b1 & 1);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0); b1 >>= 1;
    uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 6, b1 & 1);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0); b1 >>= 1;
    uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 7, b1 & 1);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 8, b2 & 1);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0); b2 >>= 1;
    uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 9, b2 & 1);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0); b2 >>= 1;
    uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 10, b2 & 1);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0); b2 >>= 1;
    uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 11, b2 & 1);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0); b2 >>= 1;
    uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 12, b2 & 1);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0); b2 >>= 1;
    uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 13, b2 & 1);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0); b2 >>= 1;
    uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 14, b2 & 1);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0); b2 >>= 1;
    uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 15, b2 & 1);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 16, b3 & 1);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
  }
    
    //axis
  if (enable_axis == 1) {
    uinput_send(ufd, EV_ABS, 0, lx);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 1, ly);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 2, rx);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 3, ry);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
  }
    
    //accelerometers
  if (enable_accel == 1) {
    uinput_send(ufd, EV_ABS, 4, posX);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 5, posY);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 6, posZ);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
  }
  
    //gyro
  if (enable_gyro == 1) {
    uinput_send(ufd, EV_ABS, 7, gyro);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
  }
    
    //buttons (sensible, as axis)
  if (enable_sbuttons == 1) {
    uinput_send(ufd, EV_ABS, 8, up);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 9, right);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 10, down);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 11, left);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 12, l2);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 13, r2);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 14, l1);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 15, r1);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 16, tri);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 17, cir);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 18, cro);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 19, squ);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
  }
    
    //acceleration
  if (enable_accon == 1) {
    uinput_send(ufd, EV_ABS, 20, accX);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 21, accY);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 22, accZ);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
  }
  
    //speed
  if (enable_speed == 1) {
    uinput_send(ufd, EV_ABS, 23, velX);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 24, velY);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    uinput_send(ufd, EV_ABS, 25, velZ);
    uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
  }

    //accelerometer buttons
  if (enable_buttons == 1 && enable_accel == 1) {
    if (posX >= 55)
    {
	uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 18, 1);
	uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
	uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 20, 0);
	uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    }
    else if (posX > 43 && posX < 55)
    {
	uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 18, 0);
	uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
	uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 20, 0);
	uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    }
    else if (posX <= 42)
    {
	uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 18, 0);
	uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
	uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 20, 1);
	uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    }
    
    if (posY >= 56)
    {
	uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 17, 0);
	uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
	uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 19, 1);
	uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    }
    else if (posY > 45 && posY < 55)
    {
	uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 17, 0);
	uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
	uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 19, 0);
	uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    }
    else if (posY <= 44)
    {
	uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 17, 1);
	uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
	uinput_send(ufd, EV_KEY, BTN_JOYSTICK + 19, 0);
	uinput_send(ufd, EV_SYN, SYN_REPORT, 0);
    }
  }
  }
  close(ufd);
  close(buf_int);
  return;
}
