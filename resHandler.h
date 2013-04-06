#ifndef _RES_HANDLER_H_
#define _RES_HANDLER_H_ 1

#include <String.h>
#include "Socket.h"

extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
}

class resHandler {
protected:
	Socket *Sock;

public:
	resHandler(void);
	resHandler(Socket *sock);
	int GetFileInfo(String& path, struct stat& file_info, String& ext);

};

#endif
