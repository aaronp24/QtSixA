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

#include "sixad-uinput-sixaxis.h"

int led_n_recv, ufd = -1;

static inline int ppoll(struct pollfd *fds, nfds_t nfds, const struct timespec *timeout, const sigset_t *sigmask)
{
	if (timeout == NULL)
		return poll(fds, nfds, -1);
	else if (timeout->tv_sec == 0)
		return poll(fds, nfds, 500);
	else
		return poll(fds, nfds, timeout->tv_sec * 1000);
}

int main(int argc, char *argv[])
{
	struct pollfd p[3];
	sigset_t sigs;
	short events;
	struct timespec timeout;
	
	// led_n led_anim buttons sbuttons axis accel accon speed gyro bda
	if (argc < 11)
	{
	 printf("Running %s requires 'sixad'. Please run sixad instead\n",  argv[0]);
	 exit(-1);
	}
	
	led_n_recv = atoi(argv[1]);

	enable_led_anim = atoi(argv[2]);
	enable_buttons = atoi(argv[3]);
	enable_sbuttons = atoi(argv[4]);
	enable_axis = atoi(argv[5]);
	enable_accel = atoi(argv[6]);
	enable_accon = atoi(argv[7]);
	enable_speed = atoi(argv[8]);
	enable_gyro = atoi(argv[9]);

	ufd = uinput_open_sixaxis(argv[10], enable_accel); //argv[10] is char ADDRESS
	if (ufd < 0) { return -1; }

	enable_sixaxis(0, led_n_recv, enable_led_anim);
// 	enable_sixaxis(0, ufd, enable_led_anim);

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
			process_sixaxis(ufd, 1, enable_buttons, enable_sbuttons, enable_axis, enable_accel, enable_accon, enable_speed, enable_gyro);

		events = p[0].revents | p[1].revents | p[2].revents;
		
		if (events & (POLLERR | POLLHUP)) { break; }
	}

	return 0;
}
