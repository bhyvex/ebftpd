#include "acl/ipmaskcache.hpp"
#include "db/interface.hpp"
#include "util/string.hpp"

namespace acl
{
IpMaskCache IpMaskCache::instance;

void IpMaskCache::Initalize()
{
  boost::unique_lock<boost::shared_mutex> lock(instance.mtx);
  db::GetIpMasks(instance.userMaskMap);
}

bool IpMaskCache::Check(const std::string& addr)
{
  boost::shared_lock<boost::shared_mutex> lock(instance.mtx);
  for (auto uid: instance.userMaskMap)
    for (auto mask: uid.second)
    {
      if (util::string::WildcardMatch(mask, addr, false))
        return true; 
    }
  return false;
}


util::Error IpMaskCache::Add(const acl::User& user, const std::string& mask,
  std::vector<std::string>& deleted)
{
  deleted.clear();
  boost::upgrade_lock<boost::shared_mutex> lock(instance.mtx);
  UserMaskMap::iterator masks = instance.userMaskMap.find(user.UID());
  if (masks == instance.userMaskMap.end())
  {
    boost::upgrade_to_unique_lock<boost::shared_mutex> writeLock(lock);
    instance.userMaskMap.insert({user.UID(), {mask}});
    return util::Error::Success();
  }

  std::vector<std::string>::iterator it;
  for (it = masks->second.begin();
    it != masks->second.end(); ++it)
  {
    // check if there is a broader mask
    if (util::string::WildcardMatch((*it), mask, false))
      return util::Error::Failure("Broader IP mask exists.");
    // check if mask we are adding is broader
    else if (util::string::WildcardMatch(mask, (*it), false))
    {
      deleted.push_back((*it));
      db::DelIpMask(user, (*it));
      {
        boost::upgrade_to_unique_lock<boost::shared_mutex> writeLock(lock);
        it = masks->second.erase(it);
      }
    }
  }   

  {
    boost::upgrade_to_unique_lock<boost::shared_mutex> writeLock(lock);
    instance.userMaskMap[user.UID()].push_back(mask);
  }
  db::AddIpMask(user, mask);
  return util::Error::Success();
}

util::Error IpMaskCache::Delete(const acl::User& user, const std::string& mask)
{
  boost::upgrade_lock<boost::shared_mutex> lock(instance.mtx);
  UserMaskMap::iterator masks = instance.userMaskMap.find(user.UID());
  if (masks == instance.userMaskMap.end())
    return util::Error::Failure("User has no IP masks.");
  
  std::vector<std::string>::iterator it;
  for (it = masks->second.begin(); it != masks->second.end(); ++it)
  {
    if ((*it) == mask)
    {
      db::DelIpMask(user, (*it));
      boost::upgrade_to_unique_lock<boost::shared_mutex> writeLock(lock);
      masks->second.erase(it);
      return util::Error::Success();                                                                      }
  }
  return util::Error::Failure("Failed to find specified IP mask.");
}

util::Error IpMaskCache::List(const acl::User& user,
  std::vector<std::string>& masks)
{
  masks.clear();
  boost::upgrade_lock<boost::shared_mutex> lock(instance.mtx);
  UserMaskMap::iterator it = instance.userMaskMap.find(user.UID());
  if (it == instance.userMaskMap.end())
    return util::Error::Failure("User has no IP masks.");            
  masks = it->second;
  return util::Error::Success();
}

// end
}

#ifdef ACL_IPMASKQUEUE_TEST

#include "db/interface.hpp"
#include "db/pool.hpp"
#include "acl/usercache.hpp"
#include "logger/logger.hpp"

int main()
{
  db::Initalize();
  acl::IpMaskCache::Initalize();

  logger::ftpd << acl::IpMaskCache::Check("blah@192.168.1.1") << logger::endl;
  logger::ftpd << acl::IpMaskCache::Check("*@hello.com") << logger::endl;

  acl::User user = acl::UserCache::User("iotest");

  std::vector<std::string> deleted;
  acl::IpMaskCache::Add(user, "*@*", deleted);
  util::Error err = acl::IpMaskCache::Add(user, "*@192.168.1.*", deleted);
  if (!err) logger::error << err.Message() << logger::endl;

  for (auto i: deleted)
    logger::ftpd << "Deleted: " << i << logger::endl;
  logger::ftpd << "Added: *@*" << logger::endl;

  acl::IpMaskCache::Delete(user, "*@*");

     
  acl::IpMaskCache::Add(user, "*@192.168.1.*", deleted);
  acl::IpMaskCache::Add(user, "*@5.168.1.*", deleted);
 
  acl::IpMaskCache::List(user, deleted);
  for (auto i: deleted)
    logger::ftpd << i << logger::endl;

   

  db::Pool::StopThread();
  return 0;
}

#endif
    