#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "rlite.h"
#include "util.h"
#include "crc64.h"
#include "endianconv.h"

// header is 4-bytes size and 8-bytes crc
#define FIFO_HEADER_SIZE 12

int rl_create_fifo(const char *fifo_name) {
	return mkfifo(fifo_name, 0777) == 0 ? RL_OK : RL_UNEXPECTED;
}

int rl_delete_fifo(const char *fifo_name) {
	unlink(fifo_name);
	return RL_OK;
}

int rl_read_fifo(const char *fifo_name, char **_data, size_t *_datalen) {
	char header[FIFO_HEADER_SIZE];
	uint64_t crc;
	size_t read;
	size_t datalen;
	FILE *fp;
	int retval;
	char *data = NULL;

	fp = fopen(fifo_name, "r");
	if (fp == NULL) {
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	read = fread(header, sizeof(char), FIFO_HEADER_SIZE, fp);
	if (read != FIFO_HEADER_SIZE) {
		retval = RL_UNEXPECTED;
		goto cleanup;
	}

	datalen = (size_t)get_4bytes((unsigned char *)header);
	RL_MALLOC(data, sizeof(char) * datalen);

	read = fread(data, sizeof(char), datalen, fp);
	if (read != datalen) {
		retval = RL_UNEXPECTED;
		goto cleanup;
	}
	crc = rl_crc64(0, (unsigned char *)data, datalen);
	memrev64ifbe(&crc);
	memcpy(&header[4], &crc, 8);
	if (memcmp(&crc, &header[4], 8) != 0) {
		retval = RL_UNEXPECTED;
		goto cleanup;
	}
	*_data = data;
	*_datalen = datalen;
	retval = RL_OK;
cleanup:
	if (fp) {
		fclose(fp);
	}
	if (retval != RL_OK) {
		rl_free(data);
	}
	return retval;
}

int rl_write_fifo(const char *fifo_name, const char *data, size_t datalen) {
	char header[FIFO_HEADER_SIZE];
	put_4bytes((unsigned char *)header, datalen);

	uint64_t crc = rl_crc64(0, (unsigned char *)data, datalen);
	memrev64ifbe(&crc);
	memcpy(&header[4], &crc, 8);

	int fd = open(fifo_name, O_WRONLY | O_NONBLOCK);
	if (fd == -1) {
		fprintf(stderr, "Failed to open fifo for writing\n");
		return RL_UNEXPECTED;
	}
	write(fd, header, FIFO_HEADER_SIZE);
	write(fd, data, datalen);
	close(fd);
	return RL_OK;
}
