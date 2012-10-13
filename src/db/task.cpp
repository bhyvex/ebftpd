#include "db/task.hpp"
#include "db/worker.hpp"
#include "db/exception.hpp"
#include "logger/logger.hpp"
#include <boost/thread/future.hpp>
namespace db
{

void Update::Execute(Worker& worker)
{
  try
  {
    worker.Update(container, obj, query, upsert);
  }
  catch (const DBError& e)
  {
    logger::error << e.Message() << logger::endl;
  }
}

void Delete::Execute(Worker& worker)
{
  try
  {
    worker.Delete(container, query);
  }
  catch (const DBError& e)
  {
    logger::error << e.Message() << logger::endl;
  }
}

void Select::Execute(Worker& worker)
{
  try
  {
    worker.Get(container, query, results, limit);
    promise.set_value(true);
  }
  catch (const DBError& e)
  {
    logger::error << e.Message() << logger::endl;
  }
}

void Insert::Execute(Worker& worker)
{
  try
  {
    worker.Insert(container, obj);
  }
  catch (const DBError& e)
  {
    logger::error << e.Message() << logger::endl;
  }
}

void EnsureIndex::Execute(Worker& worker)
{
  try
  {
    worker.EnsureIndex(container, obj);
  }
  catch(const DBError& e)
  {
    logger::error << e.Message() << logger::endl;
  }
}


// end
}