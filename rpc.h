#pragma once
#include "json.hpp"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

using json = nlohmann::json;
using namespace std;
using namespace httplib;

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