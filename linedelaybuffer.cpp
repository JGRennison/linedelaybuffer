//  linedelaybuffer
//
//  WEBSITE: https://github.com/JGRennison/linedelaybuffer
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version. See: COPYING-GPL.txt
//
//  This program  is distributed in the  hope that it will  be useful, but
//  WITHOUT   ANY  WARRANTY;   without  even   the  implied   warranty  of
//  MERCHANTABILITY  or FITNESS  FOR A  PARTICULAR PURPOSE.   See  the GNU
//  General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program. If not, see <http://www.gnu.org/licenses/>.
//
//  2014 - Jonathan G Rennison <j.g.rennison@gmail.com>
//==========================================================================

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <vector>

#ifndef VERSION_STRING
#define VERSION_STRING __DATE__ " " __TIME__
#endif
const char version_string[] = "linedelaybuffer " VERSION_STRING;
const char authors[] = "Written by Jonathan G. Rennison <j.g.rennison@gmail.com>";

struct buffer_storage {
	char *data;
	size_t length;
	size_t buffer_length;

	buffer_storage()
		: data(NULL), length(0), buffer_length(0) { }
};

long buffer_count = -1;
std::vector<buffer_storage> buffers;
unsigned int next_buffer = 0;

void buffer_write(const char *line, size_t len) {
	ssize_t result = write(STDOUT_FILENO, line, len);
	if(result < (ssize_t) len) {
		fprintf(stderr, "Write failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void usage_msg(FILE *stream) {
	fprintf(stream,
			"Usage: linedelaybuffer [options] LINES\n"
			"\tCopy STDIN to STDOUT linewise, storing LINES lines.\n"
			"\tEach input line is output only after receiving a further LINES lines.\n"
			"\tReads and writes are blocking.\n"
			"Options:\n"
			"-f, --flush\n"
			"\tFlush leftover lines at the end of the input.\n"
			"\tOtherwise the last LINES input lines will be discarded.\n"
			"-h, --help\n"
			"\tShow this help\n"
			"-V, --version\n"
			"\tShow version information\n"
		);
}

void advance_buffer() {
	next_buffer++;
	if(next_buffer == buffer_count) {
		next_buffer = 0;
	}
}

static struct option options[] = {
	{ "help",          no_argument,        NULL, 'h' },
	{ "flush",         no_argument,        NULL, 'f' },
	{ "version",       no_argument,        NULL, 'V' },
	{ NULL, 0, 0, 0 }
};

int main(int argc, char **argv) {
	char *line = NULL;
	size_t len = 0;
	ssize_t read = 0;
	bool flush_at_end = false;

	int n = 0;
	while (n >= 0) {
		n = getopt_long(argc, argv, "hfV", options, NULL);
		if (n < 0) continue;
		switch (n) {
		case 'f':
			flush_at_end = true;
			break;
		case 'V':
			fprintf(stdout, "%s\n\n%s\n", version_string, authors);
			exit(EXIT_SUCCESS);
		case 'h':
			usage_msg(stdout);
			exit(EXIT_SUCCESS);
		case '?':
			usage_msg(stderr);
			exit(EXIT_FAILURE);
		}
	}

	struct sigaction new_action;
	memset(&new_action, 0, sizeof(new_action));
	new_action.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &new_action, 0);

	if(optind == argc - 1) {
		long s = 0;
		char *end = NULL;
		s = strtol(argv[optind], &end, 10);

		if(end && *end == 0) {
			buffer_count = s;
		}
	}

	if(buffer_count < 0) {
		usage_msg(stderr);
		exit(EXIT_FAILURE);
	}
	else if(buffer_count == 0) {
		// Don't buffer anything, just write each line as it appears
		while((read = getline(&line, &len, stdin)) != -1) {
			buffer_write(line, len);
		}
		exit(EXIT_SUCCESS);
	}

	buffers.resize(buffer_count);

	while((read = getline(&line, &len, stdin)) != -1) {
		char *prev = buffers[next_buffer].data;
		size_t prev_length = buffers[next_buffer].length;
		size_t prev_buffer_length = buffers[next_buffer].buffer_length;
		if(prev) {
			buffer_write(prev, prev_length);
		}

		buffers[next_buffer].data = line;
		buffers[next_buffer].length = read;
		buffers[next_buffer].buffer_length = len;

		//re-use spent buffers
		line = prev;
		len = prev_buffer_length;

		advance_buffer();
	}

	if(flush_at_end) {
		for(size_t i = 0; i < buffers.size(); i++) {
			if(buffers[next_buffer].data) {
				buffer_write(buffers[next_buffer].data, buffers[next_buffer].length);
			}
			advance_buffer();
		}
	}

	exit(EXIT_SUCCESS);
}
