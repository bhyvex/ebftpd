//    Copyright (C) 2012, 2013 ebftpd team
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __CMD_SITE_NUKING_HPP
#define __CMD_SITE_NUKING_HPP

#include "cmd/command.hpp"

namespace text
{
class Template;
}

namespace fs
{
class VirtualPath;
}

namespace db { namespace nuking
{
class Nuke;
}
}

namespace cmd { namespace site
{

class NUKECommand : public Command
{
public:
  NUKECommand(ftp::Client& client, const std::string& argStr, const Args& args) :
    Command(client, client.Control(), client.Data(), argStr, args) { }

  void Execute();
};

class UNNUKECommand : public Command
{
public:
  UNNUKECommand(ftp::Client& client, const std::string& argStr, const Args& args) :
    Command(client, client.Control(), client.Data(), argStr, args) { }

  void Execute();
};

class NUKESCommand : public Command
{
  bool isUnnukes;
  
public:
  NUKESCommand(ftp::Client& client, const std::string& argStr, const Args& args) :
    Command(client, client.Control(), client.Data(), argStr, args),
    isUnnukes(false)
  { }

  NUKESCommand(ftp::Client& client, const std::string& argStr, const Args& args, bool isUnnukes) :
    Command(client, client.Control(), client.Data(), argStr, args),
    isUnnukes(isUnnukes)
  { }

  void Execute();
};

class UNNUKESCommand : public NUKESCommand
{
public:
  UNNUKESCommand(ftp::Client& client, const std::string& argStr, const Args& args) :
    NUKESCommand(client, argStr, args, true)
  { }
};

db::nuking::Nuke Nuke(const fs::VirtualPath& path, int multiplier, bool isPercent, 
                      const std::string& reason, acl::UserID nukerUID);
db::nuking::Nuke Unnuke(const fs::VirtualPath& path, const std::string& reason, 
                        acl::UserID nukerUID);
                        
std::string NukeTemplateCompile(const db::nuking::Nuke& nuke, text::Template& tmpl, const fs::VirtualPath& path);

} /* site namespace */
} /* cmd namespace */

#endif
