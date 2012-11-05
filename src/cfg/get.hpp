#ifndef __CFG_GET_HPP
#define __CFG_GET_HPP

#include "cfg/config.hpp"
#include <memory>

namespace cfg
{

void UpdateShared(const std::shared_ptr<Config> newShared);
void UpdateLocal();
const Config& Get();

} /* cfg namespace */

#endif
