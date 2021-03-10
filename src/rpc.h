#pragma once
#include "json.hpp"
#include "httplib.h"

using namespace httplib;
using json = nlohmann::json;
using namespace std;

typedef const char* cstr;

class RPCClient {
  private:
	static cstr server;
	static cstr path;

  public:
	static json Request(cstr method, const json& params, Headers header = {});
	static void Path(cstr path);
	static void Server(cstr server);
	static void Set(cstr server, cstr path);
};