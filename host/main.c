/*
 * This file is part of onkyo-ri.
 *
 * Copyright 2017 Ivan Shapovalov <intelfx@intelfx.name>
 *
 * onkyo-ri is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 3
 * as published by the Free Software Foundation.
 *
 * onkyo-ri is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with onkyo-ri.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * This file implements the LIRC-compatible LIRCCODE driver for the UART-based
 * Onkyo RI transceiver device.
 */

#define _GNU_SOURCE
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <inttypes.h>

#include <errno.h>
#include <time.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <lirc/driver.h>
#include <lirc/serial.h>
#include <lirc/lirc_log.h>
#include <lirc/ir_remote.h>

#include "vec.h"
#include "timespec.h"
#include "util.h"

/*
 * Device configuration
 */

static const struct timespec timeout = {
	.tv_sec = 5,
	.tv_nsec = 0
};

/*
 * Forward-declarations
 */

int lirccode_uart_open(const char *path);
int lirccode_uart_close(void);
int lirccode_uart_init(void);
int lirccode_uart_deinit(void);
char *lirccode_uart_rcv(struct ir_remote *);
int lirccode_uart_decode(struct ir_remote *, struct decode_ctx_t *);

char *read_line_until(int fd, const struct timespec *deadline);
char *read_good_line_until(int fd, const struct timespec *deadline);

/*
 * Driver interface
 */

const struct driver lirccode_uart = {
	.name = "lirccode-uart",
	.device = NULL,
	.features = LIRC_CAN_REC_LIRCCODE,
	.send_mode = 0,
	.rec_mode = LIRC_MODE_LIRCCODE,
	.code_length = 0,
	.init_func = &lirccode_uart_init,
	.deinit_func = &lirccode_uart_deinit,
	.open_func = &lirccode_uart_open,
	.close_func = &lirccode_uart_close,
	.send_func = NULL,
	.rec_func = &lirccode_uart_rcv,
	.decode_func = &lirccode_uart_decode,
	.drvctl_func = NULL,
	.readdata = NULL,
	.api_version = 2,
	.driver_version = "0.0.0",
	.info = "Generic LIRCCODE driver receiving values over UART -- for use with custom hardware",
	.fd = -1,
};

const struct driver *hardwares[] = {
	&lirccode_uart,
	NULL
};

static const logchannel_t logchannel = LOG_DRIVER;

/*
 * Implementation
 */

uint16_t rcv_code = 0;

/*
 * ->open_func(): just store the provided device path, do not do anything else.
 */
int lirccode_uart_open(const char *path)
{
	/**
	 *  Function called to do basic driver setup.
	 *  @param device String describing what device driver should
	 *      communicate with. Often (but not always) a /dev/... path.
	 *  @return 0 if everything is fine, else positive error code.
	 */

	if (path == NULL) {
		return 1;
	}

	log_debug("lirccode_uart: open(\"%s\"), path was \"%s\", fd was %d",
	          path,
	          drv.device,
	          drv.fd);

	lirccode_uart_close();
	drv.device = strdup(path);
	return 0;
}

/*
 * ->close_func(): lose all state (for now, this is just the path and fd).
 */
int lirccode_uart_close(void)
{
	/**< Hard closing, returns 0 on OK.*/

	log_debug("lirccode_uart: close(), path was \"%s\", fd was %d",
	          drv.device,
	          drv.fd);

	lirccode_uart_deinit();
	free((char *)drv.device); drv.device = NULL;
	return 0;
}

/*
 * ->init_func(): open the tty (will reset the device) and wait for a response
 *                (device says it's ready after it completes initialization).
 */
int lirccode_uart_init(void)
{
	/**
	 * Function called for initializing the driver and the hardware.
	 * Zero return value indicates failure, all other return values success.
	 */

	int r;
	_cleanup_free_ char *line = NULL;

	log_trace("lirccode_uart: init(), path is \"%s\", fd was %d",
	          drv.device,
	          drv.fd);

	if (drv.fd < 0) {
		drv.fd = open(drv.device, O_RDWR | O_NOCTTY | O_NONBLOCK);
		if (drv.fd < 0) {
			log_perror_err("lirccode_uart: init(): open(\"%s\")",
				       drv.device);
			goto error;
		}
	}

	r = tty_reset(drv.fd);
	if (r == 0) {
		log_perror_err("lirccode_uart: init(): tty_reset()");
		goto error;
	}

	r = tty_setbaud(drv.fd, UART_BAUD);
	if (r == 0) {
		log_perror_err("lirccode_uart: init(): tty_setbaud(%d)",
		               UART_BAUD);
		goto error;
	}

	struct timespec now;
	r = clock_gettime(CLOCK_REALTIME, &now);
	if (r < 0) {
		log_perror_err("lirccode_uart: init(): clock_gettime(CLOCK_REALTIME)");
		goto error;
	}

	struct timespec deadline = timespec_add(&now, &timeout);

	line = read_good_line_until(drv.fd, &deadline);
	if (line == NULL) {
		log_error("lirccode_uart: init(): did not receive device response");
		goto error;
	}
	if (!streq(line, "READY")) {
		log_error("lirccode_uart: init(): wrong device response, expected \"READY\", got \"%s\"",
		          line);
		goto error;
	}

	log_debug("lirccode_uart: init(): opened \"%s\", fd is %d, got response \"%s\"",
	          drv.device,
	          drv.fd,
	          line);
	return 1;

error:
	lirccode_uart_deinit();
	return 0;
}

/*
 * ->deinit_func(): do not do anything for now.
 */
int lirccode_uart_deinit(void)
{
	/**
	 * Function called when transmitting/receiving stops. Zero return value
	 *  indicates failure, all other return values success.
	 */

	log_debug("lirccode_uart: deinit(), path was \"%s\", fd was %d",
	          drv.device,
	          drv.fd);

	close(drv.fd); drv.fd = -1;
	return 1;
}

char *lirccode_uart_rcv(struct ir_remote *remotes)
{
	int r;
	_cleanup_free_ char *line = NULL;
	uint16_t code;

	struct timespec now;
	r = clock_gettime(CLOCK_REALTIME, &now);
	if (r < 0) {
		log_perror_err("lirccode_uart: init(): clock_gettime(CLOCK_REALTIME)");
		goto error;
	}

	struct timespec deadline = timespec_add(&now, &timeout);

	line = read_good_line_until(drv.fd, &deadline);
	if (line == NULL) {
		goto error;
	}
	if (streq(line, "READY")) {
		log_warn("lirccode_uart: rec(): got spurious \"READY\", firmware probably restarted");
		goto error;
	}
	if (sscanf(line, "INPUT: 0x%"SCNx16, &rcv_code) == 1) {
		log_debug("lirccode_uart: rec(): got line \"%s\", parsed as 0x%x",
		          line,
		          rcv_code);
		return decode_all(remotes);
	}

	log_error("lirccode_uart: rec(): got line \"%s\", cannot parse",
	          line);

error:
	return NULL;
}

int lirccode_uart_decode(struct ir_remote *remote, struct decode_ctx_t *ctx)
{
	log_debug("lirccode_uart: decode(): returning 0x%x", rcv_code);

	return map_code(remote, ctx, 0, 0, remote->bits, rcv_code, 0, 0);
}

/*
 * Main UART communication routine. Synchronously read a complete line from the
 * device, subject to a communication timeout.
 * 
 * @fd is expected to be in non-blocking mode (this is important).
 */
char *read_line_until(int fd, const struct timespec *deadline)
{
	int r;

	struct pollfd pfd = {
		.fd = fd,
		.events = POLLIN,
		.revents = 0,
	};

	struct vec vec = vec_new();
	if (vec.data == NULL) {
		log_perror_err("lirccode_uart: read_line_until(): allocate memory");
		return NULL;
	}

	bool complete_line = false;

	for (;;) {
		struct timespec now;
		r = clock_gettime(CLOCK_REALTIME, &now);
		if (r < 0) {
			log_perror_err("lirccode_uart: read_line_until(): clock_gettime(CLOCK_REALTIME)");
			goto error;
		}

		/*
		 * Check if we are past the deadline.
		 */
		if (timespec_ge(&now, deadline)) {
			goto done;
		}

		/*
		 * Wait for input until the deadline.
		 * poll() can be interrupted, so even if we do not have any
		 * input, it does not mean that we've reached the deadline.
		 */
		struct timespec timeout = timespec_subtract(deadline, &now);
		int r = ppoll(&pfd, 1, &timeout, NULL);
		if (r < 0) {
			log_perror_err("lirccode_uart: read_line_until(): ppoll()");
			goto error;
		}
		if (r == 0) {
			/*
			 * Don't check interruption reason, just skip to the
			 * next iteration, it will re-check the deadline for us.
			 */
			continue;
		}

		/*
		 * Once we got any feedback from the device, read while we can
		 * to avoid unnecessary poll()ing.
		 * Note that we don't check poll() result (if there is actually
		 * any data or maybe POLLHUP/POLLERR) because if there is an
		 * error/hangup read() will still say that.
		 */
		for (;;) {
			char c;
			r = read(drv.fd, &c, 1);
			if (r < 0) {
				if (errno == EAGAIN) {
					/*
					 * No more data, break from the inner
					 * loop and continue to the next
					 * iteration.
					 */
					break;
				}
				/*
				 * An error happened. We don't care what is it,
				 * just say that we have no input. Hopefully
				 * the poll() loop inside lirc will notice the
				 * error condition on the fd and issue a close.
				 */
				log_perror_err("lirccode_uart: read_line_until(): read()");
				goto error;
			}

			/*
			 * Ignore \r for now. This is not entirely correct
			 * (we should only special-handle \r\n), but we don't
			 * use \r for anything significant anyway.
			 */
			if (c == '\r') {
				continue;
			}

			/*
			 * Got a newline? That's it.
			 */
			if (c == '\n') {
				complete_line = true;
				goto done;
			}

			/*
			 * Append the new character onto the string.
			 */
			r = vec_add_one(&vec, c);
			if (r < 0) {
				log_perror_err("lirccode_uart: read_line_until(): allocate memory");
				goto error;
			}
		}
	}

done:
	/*
	 * Finally append the zero character.
	 */
	r = vec_add_one(&vec, '\0');
	if (r < 0) {
		log_perror_err("lirccode_uart: read_line_until(): allocate memory");
		goto error;
	}

	/*
	 * Did we quit the loop because we have the complete line? If so,
	 * we're done, return.
	 */
	if (complete_line) {
		return vec.data;
	}

	/*
	 * Otherwise, it's a timeout.
	 */
	log_error("lirccode_uart: read_line_until(): timed out, got an incomplete line \"%s\"",
	          (char *)vec.data);

error:
	vec_delete(&vec);
	return NULL;
}

/*
 * Read a complete line from the device, filtering out debug logging.
 * Device aborts are not acted upon -- we assume there is a watchdog running
 * on the device.
 * 
 * @fd is expected to be in non-blocking mode (this is important).
 */
char *read_good_line_until(int fd, const struct timespec *deadline)
{
	for (;;) {
		char *line = read_line_until(fd, deadline);
		if (line == NULL) {
			return NULL;
		}

		if (strprefix(line, "LOG: ")) {
			log_info("lirccode_uart: device says: %s", line);
		} else if (strprefix(line, "ABORT: ")) {
			log_error("lirccode_uart: device aborted: %s", line);
		} else {
			return line;
		}

		free(line);
	}
}
