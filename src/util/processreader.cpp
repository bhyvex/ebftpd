#include "util/processreader.hpp"

namespace util
{

ProcessReader::~ProcessReader()
{
  FreeArgv(&argv);
  FreeArgv(&env);
  if (pid != -1)
  {
    try
    {
      if (!Kill(defaultTermTimeout))
        Kill(SIGKILL, defaultKillTimeout);
    }
    catch (const util::SystemError&)
    {
      // ouch! ignore exception in dtor.
      // no logging facilities in util namespace
      // possibly move this class out?
    }
  }
}

void ProcessReader::DoParent()
{
  pipe.CloseWrite();
}

void ProcessReader::DoChild()
{
  pipe.CloseRead();
  if (dup2(pipe.WriteFd(), fileno(stdout)) < 0) return;
  pipe.CloseWrite();

  execvpe(file.c_str(), argv, env);
  exit(1);
}

void ProcessReader::Open()
{
  if (pid != -1) throw std::logic_error("Must call Close before calling Open again");
   
  pid = fork();
  if (pid < 0) throw util::SystemError(errno);
  
  if (pid) DoParent();
  else DoChild();
}

size_t ProcessReader::Read(char* buffer, size_t size, const util::TimePair* timeout)
{
  fd_set set;
  struct timeval tv;
  int max = std::max(pipe.ReadFd(), interruptPipe.ReadFd()) + 1;
  while (true)
  {
    FD_ZERO(&set);
    FD_SET(pipe.ReadFd(), &set);
    FD_SET(interruptPipe.ReadFd(), &set);
    
    if (timeout) memcpy(&tv, &timeout->Timeval(), sizeof(tv));
    
    int n = select(max, &set, nullptr, nullptr, 
          timeout ? &tv : nullptr);
    if (!n) throw util::SystemError(ETIMEDOUT);
    if (n < 0)
    {
      if (errno == EINTR) continue;
      throw util::SystemError(errno);
    }
    
    if (FD_ISSET(interruptPipe.ReadFd(), &set))
    {
      char ch;
      (void) read(interruptPipe.ReadFd(), &ch, 1);
      boost::this_thread::interruption_point();
    }
    
    if (FD_ISSET(pipe.ReadFd(), &set))
    {
      ssize_t len = read(pipe.ReadFd(), buffer, size);
      if (len < 0)
      {
        if (errno == EINTR) continue;
        throw util::SystemError(errno);
      }
      return len;
    }
  }
}

int ProcessReader::GetcharBuffered(const util::TimePair* timeout)
{
  if (!getcharBufferLen)
  {
    getcharBufferLen = Read(getcharBuffer, sizeof(getcharBuffer), timeout);
    if (!getcharBufferLen) return -1;
    getcharBufferPos = getcharBuffer;
  }
  
  --getcharBufferLen;
  return *getcharBufferPos++;
}

bool ProcessReader::Getline(std::string& buffer, const util::TimePair* timeout)
{
  if (eof) return false;
  
  buffer.clear();
  buffer.reserve(defaultBufferSize);
  
  int ch;
  do
  {
    ch = GetcharBuffered(timeout);
    if (ch == -1) 
    {
      eof = true;
      if (buffer.empty()) return false;
      break;
    }
    if (ch != '\r' && ch != '\n')
    {
      buffer += ch;
      if (buffer.length() == buffer.capacity())
        buffer.reserve((buffer.capacity() / defaultBufferSize) + 1);
    }
  }
  while (ch != '\n');
  return true;
}

char** ProcessReader::PrepareArgv(const ArgvType& argv)
{
  char** a = new char*[argv.size() + 1];
  for (size_t i = 0; i < argv.size(); ++i)
  {
    const std::string& s = argv[i];
    a[i] = new char[s.size() + 1];
    s.copy(a[i], s.size());
    a[i][s.size()] = '\0';
  }
  a[argv.size()] = nullptr;
  return a;
}

void ProcessReader::FreeArgv(char*** argv)
{
  if (!*argv) return;
  for (size_t i = 0; *argv[i]; ++i) delete [] *argv[i];
  delete [] *argv;
  *argv = nullptr;
}

ProcessReader::ProcessReader() : 
  argv(nullptr), env(nullptr), pid(-1), exitStatus(-1), eof(false),
  getcharBufferPos(nullptr), getcharBufferLen(0)
{
}

ProcessReader::ProcessReader(const std::string& file, 
    const ArgvType& argv, const ArgvType& env) : 
  file(file), argv(PrepareArgv(argv)), env(PrepareArgv(env)), pid(-1), 
  exitStatus(-1), eof(false), getcharBufferPos(nullptr), 
  getcharBufferLen(0)
{
  Open();
}
  
void ProcessReader::Open(const std::string& file, 
    const ArgvType& argv, const ArgvType& env)
{ 
  if (this->argv || this->env) 
    throw std::logic_error("Must call Close before calling Open again");

  this->file.assign(file);
  this->argv = PrepareArgv(argv);
  this->env = PrepareArgv(env);
  Open();
}

bool ProcessReader::Close()
{
  pipe.Reset();
  eof = false;
  FreeArgv(&argv);
  FreeArgv(&env);
  
  if (pid == -1) return true;
  while (true)
  {
    //sleep(10);
    int result = waitpid(pid, &exitStatus, WNOHANG);
    if (!result) return false;
    if (result < 0)
    {
      if (errno == ECHILD) break;
      if (errno == EINTR) continue;
      pid = -1;
      throw util::SystemError(errno);
    }
    break;
  }
  
  pid = -1;
  return true;
}

bool ProcessReader::Close(const util::TimePair& timeout)
{
  suseconds_t toMicroseconds = (timeout.Seconds() * 1000000) + timeout.Microseconds();
  while (toMicroseconds > 0)
  {
    if (Close()) return true;
    time_t sleepTime = toMicroseconds < 10000 ? toMicroseconds : 10000;
    usleep(sleepTime);
    toMicroseconds -= sleepTime;
  }
  return false;
}

bool ProcessReader::Kill(int signo, const util::TimePair& timeout)
{
  int result = kill(pid, signo);
  if (result < 0)
  {
    if (errno == ESRCH) return Close();
    throw util::SystemError(errno);
  }
  
  suseconds_t toMicroseconds = (timeout.Seconds() * 1000000) + timeout.Microseconds();
  while (toMicroseconds > 0)
  {
    if (Close()) return true;
    time_t sleepTime = toMicroseconds < 10000 ? toMicroseconds : 10000;
    usleep(sleepTime);
    toMicroseconds -= sleepTime;
  }
  
  return false;
}

void ProcessReader::Interrupt()
{
  (void) write(interruptPipe.WriteFd(), "", 1);
}

} /* util namespace */

#ifdef UTIL_PROCESSREADER_TEST

#include <boost/thread/thread.hpp>
#include "util/processreader.hpp"
#include <iostream>
#include <string>

util::ProcessReader pr;

void ThreadMain()
{
  
  util::ProcessReader::ArgvType argv = { "/bin/sleep", "10" };
  util::ProcessReader::ArgvType env;
  
  pr.Open("/bin/sleep", argv, env);
  
  std::string line;
  try
  {
    while (pr.Getline(line, util::TimePair(20))) std::cout << line << std::endl;
    std::cout << "Close: " << pr.Close() << std::endl;
    std::cout << "Kill: SIGTERM " << pr.Kill(SIGKILL, util::TimePair(2, 0)) << std::endl;
  }
  catch (const util::SystemError& e)
  {
    std::cout << e.Message() << std::endl;
  }
  std::cout << "thread exited" << std::endl;
}

int main()
{
  boost::thread t(&ThreadMain);
  //sleep(1);
  //t.interrupt();
  //pr.Interrupt();
  t.join();
  std::cout << pr.ExitStatus() << std::endl;
}

#endif
