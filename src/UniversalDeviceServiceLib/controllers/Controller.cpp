#include "Controller.hpp"

#include "IQueryExecutor.hpp"

Controller::Controller(IQueryExecutor* queryExecutor) :
    _queryExecutor(queryExecutor) {}
