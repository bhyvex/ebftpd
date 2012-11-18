#include "cmd/rfc/stou.hpp"
#include "fs/file.hpp"
#include "cmd/rfc/factory.hpp"
#include "fs/directory.hpp"
#include "logs/logs.hpp"

namespace cmd { namespace rfc
{

void STOUCommand::Execute()
{
  static size_t filenameLength = 10;

  fs::VirtualPath uniquePath;
  if (!fs::UniqueFile(client, fs::WorkDirectory(),
                      filenameLength, uniquePath))
  {
    control.Reply(ftp::ActionNotOkay,
                 "Unable to generate a unique filename.");
    return;
  }
  
  args.clear();
  args.emplace_back("STOR");
  args.emplace_back(uniquePath.ToString());  

  CommandPtr command(cmd::rfc::Factory::Lookup("STOR")->Create(client, uniquePath.ToString(), args));
  assert(command.get());
  return command->Execute();
}

} /* rfc namespace */
} /* cmd namespace */
