// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <thread>
#include <map>
#include <mutex>
#include <vector>
#include <assert.h>

#include <WinSock2.h>
#include "mysql.h"

#include <event2/event-config.h>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/buffer.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <signal.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>

#include "ProtocolDef.h"
#include "Common/memory.hpp"
#include "Common/MysqlInterface.h"
#include "Common/AES.h"
#include "Common/ZBase64.h"
#include "UserMgr.h"
#include "WorkServer.h"

// TODO: 在此处引用程序需要的其他头文件
