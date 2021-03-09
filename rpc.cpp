#include "rpc.h"

const char* RPCClient::server = "";
const char* RPCClient::path = "";

json RPCClient::Request(cstr method, const json& params, Headers headers) {

	json request = {{"method", method}};
	request["id"] = "JRPC++";
	request["jsonrpc"] = "2.0";
	request["params"] = params;

	auto res = Client(server).Post(path, headers, request.dump(), "application/json");
	if(!res || !(res->status <= 200 || res->status >= 300))
		throw runtime_error("status code =! 200!");

	json result = json::parse(res->body);

	// err detection

	return result;
}

void RPCClient::Path(cstr p) { path = p; }
void RPCClient::Server(cstr s) { server = s; }
void RPCClient::Set(cstr s, cstr p) {
	Server(s);
	Path(p);
}