#include <ctime>
#include <mongo/client/dbclient.h>
#include <boost/thread/future.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "db/interface.hpp"
#include "db/task.hpp"
#include "db/pool.hpp"
#include "db/types.hpp"
#include "db/bson/user.hpp"
#include "db/bson/group.hpp"
#include "acl/types.hpp"
#include "acl/usercache.hpp"
#include "acl/groupcache.hpp"
#include "acl/ipmaskcache.hpp"
#include "acl/types.hpp"
#include "logs/logs.hpp"
#include "db/error.hpp"
#include "util/verify.hpp"

namespace db
{

void Initialize()
{
  db::Pool::StartThread();

  Pool::Queue(TaskPtr(new db::EnsureIndex("users",
    BSON("uid" << 1 << "name" << 1))));
  Pool::Queue(TaskPtr(new db::EnsureIndex("groups",
    BSON("gid" << 1 << "name" << 1))));
  Pool::Queue(TaskPtr(new db::EnsureIndex("transfers", BSON("uid" << 1 << 
    "direction" << 1 << "section" << 1 << "day" << 1 << "week" << 1 << "month" << 1 << 
    "year" << 1))));
  Pool::Queue(TaskPtr(new db::EnsureIndex("ipmasks",
    BSON("uid" << 1 << "mask" << 1))));
}

void Cleanup()
{
  Pool::StopThread();
}

// end
}
