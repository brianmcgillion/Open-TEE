/*****************************************************************************
** Copyright (C) 2013 Intel Corporation.                                    **
**                                                                          **
** Licensed under the Apache License, Version 2.0 (the "License");          **
** you may not use this file except in compliance with the License.         **
** You may obtain a copy of the License at                                  **
**                                                                          **
**      http://www.apache.org/licenses/LICENSE-2.0                          **
**                                                                          **
** Unless required by applicable law or agreed to in writing, software      **
** distributed under the License is distributed on an "AS IS" BASIS,        **
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. **
** See the License for the specific language governing permissions and      **
** limitations under the License.                                           **
*****************************************************************************/

#include "context_child.h"
#include <syslog.h>
#include <unistd.h>
#include <dlfcn.h>
#include <errno.h>
#include <string.h>

#include "tee_internal_api.h"


void context_handler_loop(int childfd)
{
	ssize_t n;
	char *buff;
	void *applet;
	TEE_Result ret;

	buff = TEE_Malloc(1024, 0);
	if (!buff) {
		syslog(LOG_DEBUG, "Malloc failed");
		return;
	}

	applet = dlopen("/home/brian/code/ccpp/project-build/qtc_Desktop-debug/libtest_applet.so", RTLD_LAZY);
	if (!applet)
		syslog(LOG_ERR, "Failed to load the library : %s", strerror(errno));

	TA_CreateEntryPoint_t ep = dlsym(applet, "TA_CreateEntryPoint");
	if (!ep)
		syslog(LOG_DEBUG, "Failed to load enty symbol : %s", strerror(errno));

	ret = ep();
	syslog(LOG_DEBUG, "Return value is %d", ret);

	while ((n = (read(childfd, buff, 1024 - 1))) > 0) {
		buff[n] = '\0';
		syslog(LOG_DEBUG, "%s", buff);
	}

	TEE_Free(buff);
	return;
}
