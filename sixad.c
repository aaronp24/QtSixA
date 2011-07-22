/*
 *  sixad driver (based on BlueZ)
 * ------------------------------
 *  Used several code from BlueZ, and some other portions from many projects around the web
 * 
 *  Written by falkTX, 2009
 */

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <malloc.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/wait.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/hidp.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

#include <linux/input.h>
#include <linux/joystick.h>
#include <linux/uinput.h>

#include "textfile.h"
#include "sdp.h"

#define _GNU_SOURCE
#define L2CAP_PSM_HIDP_CTRL 0x11
#define L2CAP_PSM_HIDP_INTR 0x13

int led_n, ledplus, n_six, enable_ps3fix;
char *enable_led_anim, *enable_buttons, *enable_sbuttons;
char *enable_axis, *enable_accel, *enable_accon, *enable_speed, *enable_gyro;

static volatile sig_atomic_t __io_canceled = 0;

static void sig_hup(int sig)
{
}

static void sig_term(int sig)
{
	__io_canceled = 1;
}

static inline int ppoll(struct pollfd *fds, nfds_t nfds,
		const struct timespec *timeout, const sigset_t *sigmask)
{
	if (timeout == NULL)
		return poll(fds, nfds, -1);
	else if (timeout->tv_sec == 0)
		return poll(fds, nfds, 500);
	else
		return poll(fds, nfds, timeout->tv_sec * 1000);
}


//PS3 Compatibility Fix LEDs
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

static int get_type(int snsk)
{
	struct hidp_connadd_req req;
	struct sockaddr_l2 addr;
	socklen_t addrlen;
	bdaddr_t src, dst;

	memset(&addr, 0, sizeof(addr));
	addrlen = sizeof(addr);

	if (getsockname(snsk, (struct sockaddr *) &addr, &addrlen) < 0)
		return -1;

	bacpy(&src, &addr.l2_bdaddr);

	memset(&addr, 0, sizeof(addr));
	addrlen = sizeof(addr);

	if (getpeername(snsk, (struct sockaddr *) &addr, &addrlen) < 0)
		return -1;

	bacpy(&dst, &addr.l2_bdaddr);

	get_sdp_device_info(&src, &dst, &req);

	if (req.vendor == 0x054c && req.product == 0x0268) { return 1; }
	else { return 0; }
}

static int l2cap_listen(const bdaddr_t *bdaddr, unsigned short psm, int lm, int backlog)
{
	struct sockaddr_l2 addr;
	struct l2cap_options opts;
	int sk;

	if ((sk = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP)) < 0)
		return -1;

	memset(&addr, 0, sizeof(addr));
	addr.l2_family = AF_BLUETOOTH;
	bacpy(&addr.l2_bdaddr, bdaddr);
	addr.l2_psm = htobs(psm);

	if (bind(sk, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		close(sk);
		return -1;
	}

	setsockopt(sk, SOL_L2CAP, L2CAP_LM, &lm, sizeof(lm));

	memset(&opts, 0, sizeof(opts));
	opts.imtu = 64;
	opts.omtu = HIDP_DEFAULT_MTU; //64?
	opts.flush_to = 0xffff;

	setsockopt(sk, SOL_L2CAP, L2CAP_OPTIONS, &opts, sizeof(opts));

	if (listen(sk, backlog) < 0) { //backlog = 5 instead of 10?
		close(sk);
		return -1;
	}

	return sk;
}

static int create_device(int ctl, int csk, int isk)
{
	struct hidp_connadd_req req;
	struct sockaddr_l2 addr;
	socklen_t addrlen;
	bdaddr_t src, dst;
	char bda[18];
	int err;

	memset(&addr, 0, sizeof(addr));
	addrlen = sizeof(addr);

	if (getsockname(csk, (struct sockaddr *) &addr, &addrlen) < 0)
		return -1;

	bacpy(&src, &addr.l2_bdaddr);

	memset(&addr, 0, sizeof(addr));
	addrlen = sizeof(addr);

	if (getpeername(csk, (struct sockaddr *) &addr, &addrlen) < 0)
		return -1;

	bacpy(&dst, &addr.l2_bdaddr);

	memset(&req, 0, sizeof(req));
	req.ctrl_sock = csk;
	req.intr_sock = isk;
	req.flags     = 0;
	req.idle_to   = 1800;

	err = get_stored_device_info(&src, &dst, &req);
	if (!err)
		goto create;

	err = get_sdp_device_info(&src, &dst, &req);
	if (err < 0)
		goto error;

create:
	ba2str(&dst, bda);
	syslog(LOG_INFO, "Connected %s (%s)", req.name, bda);

	if (req.vendor == 0x054c && req.product == 0x0268 && enable_ps3fix != 1)
	{
		syslog(LOG_ERR, "Cannot start Sixaxis now; It should had been initialized before");
		return -1;
	}
	else
	{
		if (req.vendor == 0x054c && req.product == 0x0268) { enable_sixaxis(csk, led_n, atoi(enable_led_anim)); }
		err = ioctl(ctl, HIDPCONNADD, &req);
	}

error:
	if (req.rd_data)
		free(req.rd_data);

	return err;
}

void l2cap_accept(int ctl, int csk, int isk)
{
	bdaddr_t bdaddr;
	int ctrl_socket, intr_socket, err; // nsk = ctrl_socket or intr_socket
	struct sockaddr_l2 addr;
	socklen_t addrlen;

	memset(&addr, 0, sizeof(addr));
	addrlen = sizeof(addr);

	if ((ctrl_socket =
	     accept(csk, (struct sockaddr *)&addr, &addrlen)) < 0) {
		syslog(LOG_INFO, "unable to accept control stream");
		return;
	}
	bacpy(&bdaddr, &addr.l2_bdaddr);
	
	memset(&addr, 0, sizeof(addr));
	addrlen = sizeof(addr);

	if ((intr_socket =
	     accept(isk, (struct sockaddr *)&addr, &addrlen)) < 0) {
		syslog(LOG_INFO, "unable to accept info stream");
		close(ctrl_socket);
		return;
	}

	if (bacmp(&bdaddr, &addr.l2_bdaddr)) {
		syslog(LOG_INFO,
		       "intr and ctrl streams from different hosts - rejecting both");
		close(ctrl_socket);
		close(intr_socket);
		return;
	}
	
	int type = get_type(ctrl_socket);
	
	if (type == 1 && enable_ps3fix != 1) {
  		printf("Will initiate Sixaxis now\n");
		
		char bda[18];
		char *led_n_char;
		ba2str(&addr.l2_bdaddr, bda);
		char *uinput_sixaxis_cmd = "/usr/sbin/sixad-uinput-sixaxis";
		
		int pid = fork();
		if (pid == 0) {
		  
			close(ctl);
			close(csk);
			close(isk);

			dup2(ctrl_socket, 0);
			close(ctrl_socket);
			dup2(intr_socket, 1);
			close(intr_socket);

			if (led_n == -1) { led_n_char = "-1"; }
			else if (led_n == 1) { led_n_char = "1"; }
			else if (led_n == 2) { led_n_char = "2"; }
			else if (led_n == 3) { led_n_char = "3"; }
			else if (led_n == 4) { led_n_char = "4"; }
			else if (led_n == 5) { led_n_char = "5"; }
			else if (led_n == 6) { led_n_char = "6"; }
			else if (led_n == 7) { led_n_char = "7"; }
			else { led_n_char = "1"; };
		
			char *argv[] = { uinput_sixaxis_cmd, led_n_char,
			  enable_led_anim, enable_buttons, enable_sbuttons, enable_axis,
			  enable_accel, enable_accon, enable_speed, enable_gyro, bda, NULL };
			  
			char *envp[] = { NULL };
		
			syslog(LOG_INFO, "Connected PLAYSTATION(R)3 Controller (%s)", bda);
			
			if (execve(argv[0], argv, envp) < 0) {
				syslog(LOG_INFO, "cannot exec %s", uinput_sixaxis_cmd);
				close(0);
				close(1);
			}
			else
				syslog(LOG_INFO, "Awesome! The device %s won't be disconnected when killing sixad", bda);
		}
	}
	else { 
		err = create_device(ctl, ctrl_socket, intr_socket);
		printf("Connected new device using the default driver\n");
		if (err < 0)
			syslog(LOG_ERR, "HID create error %d (%s)", errno, strerror(errno));
		close(intr_socket);
		close(ctrl_socket);
	}
	return;
}

static void run_server(int ctl, int csk, int isk)
{
	struct pollfd p[2];
	sigset_t sigs;
	short events;
	struct timespec timeout;

	sigfillset(&sigs);
	sigdelset(&sigs, SIGCHLD);
	sigdelset(&sigs, SIGPIPE);
	sigdelset(&sigs, SIGTERM);
	sigdelset(&sigs, SIGINT);
	sigdelset(&sigs, SIGHUP);
	
	printf("Server mode now active, will start search now\n");

	p[0].fd = csk; //ctrl_listen
	p[0].events = POLLIN | POLLERR | POLLHUP;

	p[1].fd = isk; //intr_listen
	p[1].events = POLLIN | POLLERR | POLLHUP;

	while (!__io_canceled) {
		int idx = 2;
		int i;

		for (i = 0; i < idx; i++)
			p[i].revents = 0;

		timeout.tv_sec = 1;
		timeout.tv_nsec = 0;
		
		if (ppoll(p, idx, &timeout, &sigs) < 1)
			continue;

		events = p[0].revents | p[1].revents;

		if (events & POLLIN) {
			
			printf("One event received\n");
			l2cap_accept(ctl, csk, isk);
			printf("One event proccessed\n");
			
			if (ledplus != 0 && led_n != -1)
			{
			  /* for the next connection, don't allow the sixaxis to have the same LED; if no other choice, set to 1 */
			  if (led_n == 1) { led_n = 2; }
			  else if (led_n == 2)  { led_n = 3; }
			  else if (led_n == 3)  { led_n = 4; }
			  else if (led_n == 4)  { led_n = 5; }
			  else if (led_n == 5)  { led_n = 6; }
			  else if (led_n == 6)  { led_n = 7; }
			  else if (led_n == 7)  { led_n = 1; } //should it stay on LED #7 or go back to LED #1?
			  else  { led_n = 1; }
			  printf("Changing LED # to %i\n", led_n);
			} else { printf("No changing LED, still on # %i\n", led_n); }
			
		}
		
		if (events & (POLLERR | POLLHUP)) {
			break;
			printf("Main loop was broken...\n");
		}
		
	}
}

int main(int argc, char *argv[])
{
	struct sigaction sa;
	bdaddr_t bdaddr;
	char addr[18];
	int log_option = LOG_NDELAY | LOG_PID;
	int ctl, csk, isk, lm = 0;
	
	// sixad: led_n ledplus led_anim buttons sbuttons axis accel accon speed gyro ps3fix
	if (argc < 12)
	{
	 printf("Running %s requires 'sixad'. Please run sixad instead\n",  argv[0]);
	 exit(-1);
	}
	
	led_n = atoi(argv[1]);
    
	if (led_n < 1)
	{
	  if (led_n == -1)
	    led_n = -1;
	  else
	    led_n = 1;
	}
	else if (led_n > 7)
	  led_n = 7;
	
	if (led_n != -1 ) { printf("Will use LED # %i\n", led_n); }
	else { printf("Will NOT use LED\n"); }
	
	ledplus = atoi(argv[2]);
	enable_led_anim = argv[3];
	enable_buttons = argv[4];
	enable_sbuttons = argv[5];
	enable_axis = argv[6];
	enable_accel = argv[7];
	enable_accon = argv[8];
	enable_speed = argv[9];
	enable_gyro = argv[10];
	enable_ps3fix = atoi(argv[11]);
	
	if (enable_ps3fix == 1)	{ printf("PS3 Compatibility Fix Enabled\n"); }
	
	bacpy(&bdaddr, BDADDR_ANY);
	
	lm |= L2CAP_LM_MASTER;
	
	ba2str(&bdaddr, addr);
	
	ctl = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HIDP);
	if (ctl < 0) {
		perror("Can't open HIDP control socket");
		exit(1);
	}
	
	csk = l2cap_listen(&bdaddr, L2CAP_PSM_HIDP_CTRL, lm, 10); //ctrl_listen
	if (csk < 0) {
		perror("Can't listen on HID control channel");
		close(ctl);
		exit(1);
	}

	isk = l2cap_listen(&bdaddr, L2CAP_PSM_HIDP_INTR, lm, 10); //intr_listen
	if (isk < 0) {
		perror("Can't listen on HID interrupt channel");
		close(ctl);
		close(csk);
		exit(1);
	}
	
	log_option |= LOG_PERROR;
	
	openlog("sixad", log_option, LOG_DAEMON);
	
	if (bacmp(&bdaddr, BDADDR_ANY))
		syslog(LOG_INFO, "sixad started (adress %s), press the PS button now", addr);
	else
		syslog(LOG_INFO, "sixad started, press the PS button now");
	
	memset(&sa, 0, sizeof(sa));
	sa.sa_flags = SA_NOCLDSTOP;

	sa.sa_handler = sig_term;
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = sig_hup;
	sigaction(SIGHUP, &sa, NULL);

	sa.sa_handler = SIG_IGN;
	sigaction(SIGCHLD, &sa, NULL);
	sigaction(SIGPIPE, &sa, NULL);

	run_server(ctl, csk, isk); //csk = ctrl_listen; isk = intr_listen

	syslog(LOG_INFO, "Exit");

	close(csk);
	close(isk);
	close(ctl);

	return 0;
}

