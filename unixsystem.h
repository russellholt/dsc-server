#ifndef _UNIX_SYSTEM_H_
#define _UNIX_SYSTEM_H_

#include <unistd.h>

extern "C" {
#include <sys/types.h>
#include <sys/socket.h>

	int socket(int domain, int type, int protocol);
	int listen(int s, int backlog);
	int bind(int s, struct sockaddr *name, int namelen);
	int accept(int s, struct sockaddr *name, int *addrlen);
/*
	int close(int fd);
	int read(int fd, char *buf, int nbyte);
	int write(int fd, char *buf, int nbyte);

	void bcopy(char *b1, char *b2, int length);
	int bcmp(char *b1, char *b2, int length);
	void bzero(char *b, int length);
	int ffs(int i);

	int getpid(void);
	int unlink(char *path);
	int execve(char *path, char **argv, char **envp);
*/
}

#endif
