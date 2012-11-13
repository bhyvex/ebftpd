#include <cctype>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "text/tag.hpp"
#include "text/error.hpp"
#include "logs/logs.hpp"

namespace text
{

void Tag::Register(const std::string& filter)
{
  if (filter == "left")
    alignment = Alignment::Left;

  else if (filter == "right")
    alignment = Alignment::Right;

  else if (filter == "center")
    alignment = Alignment::Center;

  else if (filter == "pretty")
    pretty = true;

  else if (filter == "kb")
  {
    SetType(TagType::Decimal);
    measurement = Measurement::Kbyte;
  }
  else if (filter == "mb")
  {
    SetType(TagType::Decimal);
    measurement = Measurement::Mbyte;
  }
  else if (filter == "gb")
  {
    SetType(TagType::Decimal);
    measurement = Measurement::Gbyte;
  }
  else if (filter == "auto")
    measurement = Measurement::Auto;

  else
  {
    std::vector<std::string> args;
    boost::split(args, filter, boost::is_any_of(".")); 
    if (args.size() > 2) throw TemplateFilterMalform("Error parsing filter, should be '^\\d+(\\.\\d+)?$': " + filter);
  
    for (auto& c: args.front())
      if (!std::isdigit(c)) throw TemplateFilterMalform("Error parsing filter, should be an integer: " + args.front());
    width = args.front();
  
    if (args.size() == 2)
    {
      for (auto& c: args.back())
        if (!std::isdigit(c)) 
          throw TemplateFilterMalform("Error parsing filter, should be an integer: " + args.back());
      precision = args.back();
      SetType(TagType::Float);
    }
  }

}

void Tag::Compile()
{ 
  std::ostringstream os;
  os << "%";

  if (alignment == Alignment::Left)
    os << "-";
  else if (alignment == Alignment::Center)
    os << "=";

  if (!width.empty()) os << width;
  if (!precision.empty()) os << "." << precision;

  if (type == TagType::String) os << "s";
  else if (type == TagType::Float) os << "f";
  else if (type == TagType::Decimal) os << "d";
  else assert(false); // should never get here


  format = os.str();
}

void Tag::Parse(const std::string& value)
{
  std::ostringstream os;
  os << boost::format(format) % value;
  this->value = os.str();
}

void Tag::ParseSize(long long bytes)
{

  if (!precision.empty()) 
  {
    SetType(TagType::Float);
    Compile();
  }

  double value;

  if (measurement == Measurement::Kbyte)
    value = bytes / 1024.0;
  else if (measurement == Measurement::Mbyte)
    value = bytes / 1024.0 / 1024.0;
  else if (measurement == Measurement::Gbyte)
    value = bytes / 1024.0 / 1024.0 / 1024.0;

  std::ostringstream os;
  os << boost::format(format) % value;

  if (pretty)
  {
    std::string number = os.str();
    os.str(std::string());
    os.imbue(std::locale("")); // portability issues?
    try
    {
      os << boost::lexical_cast<long double>(number);
    }
    catch (const boost::bad_lexical_cast& e)
    {
      // doesn't handle spaces/width very well.. also seems to get rid of .00
      this->value = number;
      return;
    }
  }
  this->value = os.str();
}

void Tag::ParseSpeed(long long bytes, long long xfertime)
{
}

// end
}
