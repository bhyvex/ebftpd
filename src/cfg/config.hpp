#ifndef __CFG_CONFIG_CPP
#define __CFG_CONFIG_CPP

#include <algorithm>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <unordered_set>
#include <map>
#include <boost/optional.hpp>
#include "cfg/setting.hpp"
#include "cfg/section.hpp"
#include "util/enumstrings.hpp"

namespace cfg
{

enum class WeekStart { Sunday, Monday };
enum class EPSVFxp { Allow, Deny, Force };
enum class LogAddresses { Never, Errors, Always };

class Config;

typedef std::shared_ptr<cfg::Config> ConfigPtr;

class Config
{ 
  int version;
  bool tool;
 
  void ParseGlobal(const std::string& opt, std::vector<std::string>& toks);
  void ParseSection(const std::string& opt, std::vector<std::string>& toks);
  void Parse(std::string line);
  
  bool CheckSetting(const std::string& name);
  void SanityCheck();
  void SetDefaults(const std::string& opt); 

  std::unordered_map<std::string, int> settingsCache;
  Section* currentSection;

  // containers
  std::string sitepath;
  std::string pidfile;
  std::string tlsCertificate;
  std::string tlsCiphers;
  int port;
  // glftpd
  setting::AsciiDownloads asciiDownloads;
  setting::AsciiUploads asciiUploads;
  int freeSpace;
  std::string sitenameLong;
  std::string sitenameShort;
  std::string loginPrompt;
  std::vector<std::string> master;
  std::vector<setting::SecureIp> secureIp;
  std::vector<setting::SecurePass> securePass;
  std::string datapath;
  std::vector<std::string> bouncerIp;
  bool bouncerOnly;
  std::vector<setting::SpeedLimit> maximumSpeed;
  std::vector<setting::SpeedLimit> minimumSpeed;
  setting::SimXfers simXfers;
  std::vector<std::string> calcCrc;
  std::vector<std::string> xdupe;
  std::vector<std::string> validIp;
  std::vector<std::string> activeAddr;
  std::vector<std::string> pasvAddr;
  setting::Ports activePorts;
  setting::Ports pasvPorts;
  std::vector<setting::AllowFxp> allowFxp;
  std::vector<setting::Right> welcomeMsg;
  std::vector<setting::Right> goodbyeMsg;
  std::string banner;
  std::vector<setting::Alias> alias;
  std::vector<std::string> cdpath;
  
  setting::Log securityLog;
  setting::Log databaseLog;
  setting::Log eventLog;
  setting::Log errorLog;
  setting::Log debugLog;
  setting::Log siteopLog;
  
  // ind rights
  std::vector<setting::Right> delete_; // delete is reserved
  std::vector<setting::Right> deleteown;
  std::vector<setting::Right> overwrite;
  std::vector<setting::Right> resume;
  std::vector<setting::Right> rename;
  std::vector<setting::Right> renameown;
  std::vector<setting::Right> filemove;
  std::vector<setting::Right> makedir;
  std::vector<setting::Right> upload;
  std::vector<setting::Right> download;
  std::vector<setting::Right> nuke;
  std::vector<setting::Right> hideinwho;
  std::vector<setting::Right> freefile;
  std::vector<setting::Right> nostats;
  std::vector<setting::Right> hideowner;

  std::vector<std::string> eventpath;
  std::vector<std::string> dupepath;
  std::vector<std::string> indexpath;

  // end rights
  std::vector<setting::PathFilter> pathFilter;
  setting::MaxUsers maxUsers;
  std::vector<setting::ACLInt> maxUstats;
  std::vector<setting::ACLInt> maxGstats;
  std::vector<std::string> bannedUsers;
  std::vector<setting::Right> showDiz;
  bool dlIncomplete;
  std::vector<setting::Cscript> cscript;
  std::vector<std::string> idleCommands;
  int totalUsers;
  setting::Lslong lslong;
  std::vector<setting::HiddenFiles> hiddenFiles;
  std::vector<std::string> noretrieve;
  int multiplierMax;
  long emptyNuke;
  std::vector<setting::Creditcheck> creditcheck;
  std::vector<setting::Creditloss> creditloss;
  setting::NukedirStyle nukedirStyle;
  std::vector<setting::Msgpath> msgpath;
  std::vector<setting::Privpath> privpath;
  std::vector<setting::SiteCmd> siteCmd;
  int maxSitecmdLines;
  setting::IdleTimeout idleTimeout;
  setting::Database database;
  ::cfg::WeekStart weekStart;
  std::vector<setting::CheckScript> preCheck;
  std::vector<setting::CheckScript> preDirCheck;
  std::vector<setting::CheckScript> postCheck;
  std::unordered_map<std::string, acl::ACL> commandACLs;  
  std::map<std::string, Section> sections;
  ::cfg::EPSVFxp epsvFxp;
  int maximumRatio;
  int dirSizeDepth;
  bool asyncCRC;
  bool identLookup;
  bool dnsLookup;
  ::cfg::LogAddresses logAddresses;
  
  acl::ACL tlsControl;
  acl::ACL tlsListing;
  acl::ACL tlsData;
  acl::ACL tlsFxp;
  
  static std::unordered_set<std::string> aclKeywords;
  static int latestVersion;
  static const std::vector<std::string> requiredSettings;
  static const std::string configFile;
  static const std::vector<std::string> configSearch;
  static std::string lastConfigPath;
  
  Config(const Config&) = default;
  Config& operator=(const Config&) = default;
  
  static void ParameterCheck(const std::string& opt,
                             const std::vector<std::string>& toks, 
                             int minimum, int maximum);
  static void ParameterCheck(const std::string& opt,
                             const std::vector<std::string>& toks, 
                             int minimum)
  { ParameterCheck(opt, toks, minimum, minimum); }


public:
  Config(const std::string& configPath, bool tool = false);

  int Version() const { return version; }

  const setting::Database& Database() const { return database; }
  const std::string& Sitepath() const { return sitepath; }
  const std::string& Pidfile() const { return pidfile; }
  const std::string& TlsCertificate() const { return tlsCertificate; }
  const std::string& TlsCiphers() const { return tlsCiphers; }
  int Port() const { return port; }
  const setting::AsciiDownloads& AsciiDownloads() const { return asciiDownloads; } 
  const setting::AsciiUploads& AsciiUploads() const { return asciiUploads; } 
  int FreeSpace() const { return freeSpace; }
  const std::string& SitenameLong() const { return sitenameLong; }
  const std::string& SitenameShort() const { return sitenameShort; }
  const std::string& LoginPrompt() const { return loginPrompt; }
  const std::vector<std::string>& Master() const { return master; }  
  bool IsMaster(const std::string& login) const
  {
    return std::find(master.begin(), master.end(), login) != master.end();
  }
  const std::vector<setting::SecureIp>& SecureIp() const { return secureIp; }
  const std::vector<setting::SecurePass>& SecurePass() const { return securePass; }
  const std::string& Datapath() const { return datapath; }
  //const std::vector<std::string>& BouncerIp() const { return bouncerIp; }
  bool IsBouncer(const std::string& ip) const;
  bool BouncerOnly() const { return bouncerOnly; }
  const std::vector<setting::SpeedLimit>& MaximumSpeed() const { return maximumSpeed; }
  const std::vector<setting::SpeedLimit>& MinimumSpeed() const { return minimumSpeed; }
  const setting::SimXfers& SimXfers() const { return simXfers; }
  const std::vector<std::string>& CalcCrc() const { return calcCrc; }
  const std::vector<std::string>& Xdupe() const { return xdupe; }
  const std::vector<std::string>& ValidIp() const { return validIp; }
  const std::vector<std::string>& ActiveAddr() const { return activeAddr; }
  const std::vector<std::string>& PasvAddr() const { return pasvAddr; }
  const setting::Ports& ActivePorts() const { return activePorts; }
  const setting::Ports& PasvPorts() const { return pasvPorts; }
  const std::vector<setting::AllowFxp>& AllowFxp() const { return allowFxp; }
  const std::vector<setting::Right>& WelcomeMsg() const { return welcomeMsg; }
  const std::vector<setting::Right>& GoodbyeMsg() const { return goodbyeMsg; }
  const std::string& Banner() const { return banner; }
  const std::vector<setting::Alias>& Alias() const { return alias; }
  const std::vector<std::string>& Cdpath() const { return cdpath; }
  
  const setting::Log SecurityLog() const { return securityLog; }
  const setting::Log DatabaseLog() const { return databaseLog; }
  const setting::Log EventLog() const { return eventLog; }
  const setting::Log ErrorLog() const { return errorLog; }
  const setting::Log DebugLog() const { return debugLog; }
  const setting::Log SiteopLog() const { return siteopLog; }

  // rights section
  const std::vector<setting::Right>& Delete() const { return delete_; } 
  const std::vector<setting::Right>& Deleteown() const { return deleteown; } 
  const std::vector<setting::Right>& Overwrite() const { return overwrite; } 
  const std::vector<setting::Right>& Resume() const { return resume; } 
  const std::vector<setting::Right>& Rename() const { return rename; } 
  const std::vector<setting::Right>& Renameown() const { return renameown; } 
  const std::vector<setting::Right>& Filemove() const { return filemove; } 
  const std::vector<setting::Right>& Makedir() const { return makedir; } 
  const std::vector<setting::Right>& Upload() const { return upload; } 
  const std::vector<setting::Right>& Download() const { return download; } 
  const std::vector<setting::Right>& Nuke() const { return nuke; } 
  const std::vector<setting::Right>& Hideinwho() const { return hideinwho; } 
  const std::vector<setting::Right>& Freefile() const { return freefile; } 
  const std::vector<setting::Right>& Nostats() const { return nostats; } 
  const std::vector<setting::Right>& Hideowner() const { return hideowner; } 

  bool IsEventLogged(const std::string& path) const;
  bool IsDupeLogged(const std::string& path) const;
  bool IsIndexed(const std::string& path) const;
  const std::vector<std::string>& Indexed() const { return indexpath; }

  const std::vector<setting::PathFilter>& PathFilter() const { return pathFilter; }
  const setting::MaxUsers& MaxUsers() const { return maxUsers; }
  const std::vector<setting::ACLInt>& MaxUstats() const { return maxUstats; }
  const std::vector<setting::ACLInt>& MaxGstats() const { return maxGstats; }
  const std::vector<std::string>& BannedUsers() const { return bannedUsers; }
  const std::vector<setting::Right>& ShowDiz() const { return showDiz; }
  bool DlIncomplete() const { return dlIncomplete; }
  const std::vector<setting::Cscript>& Cscript() const { return cscript; }
  const std::vector<std::string>& IdleCommands() const { return idleCommands; }
  int TotalUsers() const { return totalUsers; }
  const setting::Lslong& Lslong() const { return lslong; }
  const std::vector<setting::HiddenFiles>& HiddenFiles() const { return hiddenFiles; }
  const std::vector<std::string>& Noretrieve() const { return noretrieve; }
  int MultiplierMax() const { return multiplierMax; }
  long EmptyNuke() const { return emptyNuke; }
  const std::vector<setting::Creditcheck>& Creditcheck() const { return creditcheck; }
  const std::vector<setting::Creditloss>& Creditloss() const { return creditloss; }
  const setting::NukedirStyle& Nukedirtyle() const { return nukedirStyle; }
  const std::vector<setting::Msgpath>& Msgpath() const { return msgpath; }
  const std::vector<setting::Privpath>& Privpath() const { return privpath; }
  const std::vector<setting::SiteCmd>& SiteCmd() const { return siteCmd; }
  int MaxSitecmdLines() const { return maxSitecmdLines; }
  const setting::IdleTimeout& IdleTimeout() const { return idleTimeout; }
  ::cfg::WeekStart WeekStart() const { return weekStart; }
  const std::vector<setting::CheckScript>& PreCheck() const { return preCheck; }
  const std::vector<setting::CheckScript>& PreDirCheck() const { return preDirCheck; }
  const std::vector<setting::CheckScript>& PostCheck() const { return postCheck; }  
  const std::map<std::string, Section>& Sections() const { return sections; }
  boost::optional<const Section&> SectionMatch(const std::string& path) const;
  ::cfg::EPSVFxp EPSVFxp() const { return epsvFxp; }
  int MaximumRatio() const { return maximumRatio; }
  const acl::ACL& TLSControl() const { return tlsControl; }
  const acl::ACL& TLSListing() const { return tlsListing; }
  const acl::ACL& TLSData() const { return tlsData; }
  const acl::ACL& TLSFxp() const { return tlsFxp; }
  int DirSizeDepth() const { return dirSizeDepth; }
  bool AsyncCRC() const { return asyncCRC; }
  bool IdentLookup() const { return identLookup; }
  bool DNSLookup() const { return dnsLookup; }
  ::cfg::LogAddresses LogAddresses() const { return logAddresses; }

  const acl::ACL& CommandACL(const std::string& keyword) const
  { return commandACLs.at(keyword); }
  
  static void PopulateACLKeywords(const std::unordered_set<std::string>& keywords)
  { aclKeywords.insert(keywords.begin(), keywords.end()); }
  
  static ConfigPtr Load(std::string configPath = lastConfigPath, bool tool = false);
  
  friend void UpdateLocal();
};

}

#endif 
