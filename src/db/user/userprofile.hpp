#ifndef __DB_USER_USERPROFILE_HPP
#define __DB_USER_USERPROFILE_HPP

#include <vector>
#include <unordered_map>
#include <boost/ptr_container/ptr_vector.hpp>
#include <mongo/client/dbclient.h>
#include "acl/userprofile.hpp"
#include "acl/user.hpp"
#include "acl/types.hpp"
#include "util/error.hpp"
#include "acl/user.hpp"

namespace db { namespace userprofile
{

void Save(const acl::UserProfile& profile);
void Delete(acl::UserID uid);
std::vector<acl::UserProfile> GetAll();
acl::UserProfile Get(acl::UserID uid);

std::unordered_map<acl::UserID, acl::UserProfile>
GetSelection(const std::vector<acl::User>& users);

bool DecrCredits(acl::UserID uid, long long kBytes, 
        const std::string& section, bool negativeOkay);
void IncrCredits(acl::UserID uid, long long kBytes,
        const std::string& section);

// setters
void Set(acl::UserID uid, mongo::BSONObj obj);

util::Error SetWeeklyAllotment(acl::UserID uid, const std::string& value);
util::Error SetHomeDir(acl::UserID uid, const std::string& value);
util::Error SetStartupDir(acl::UserID uid, const std::string& value);
util::Error SetIdleTime(acl::UserID uid, const std::string& value);
util::Error SetExpires(acl::UserID uid, std::string& value);
util::Error SetNumLogins(acl::UserID uid, const std::string& value);
util::Error SetComment(acl::UserID uid, const std::string& value);
util::Error SetMaxDlSpeed(acl::UserID uid, const std::string& value);
util::Error SetMaxUlSpeed(acl::UserID uid, const std::string& value);
util::Error SetMaxSimDl(acl::UserID uid, const std::string& value);
util::Error SetMaxSimUl(acl::UserID uid, const std::string& value);
void SetRatio(acl::UserID uid, const std::string& section, int ratio);
void SetTagline(acl::UserID uid, const std::string& tagline);

void Login(acl::UserID uid);
// end
}
}
#endif
