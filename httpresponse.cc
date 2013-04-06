#include <stream.h>
#include "httpresponse.h"
#include "SLList_util.h"

httpresponse::httpresponse(void)
{
	status_code = 0;
	bytes_sent = 0;
}

void httpresponse::print(void)
{
	cout << response_line << '\n';
	cout << current_date << '\n';
	cout << server << '\n';
	cout << mime << '\n';
	cout << last_mod_date << '\n';
	cout << content_type << '\n';
	cout << content_length << '\n';
	cout << "Location: " << location << '\n';
	cout << others << '\n';
}

// Create a response line
// http_version comes from httprequest::HTTP_Version
void httpresponse::CreateResponseLine(String& http_version)
{
	response_line = http_version + String(" ");

	response_line += dec(status_code) + String(" ");

	switch(status_code)
	{
		case STATUS_OK:					// 200
				status_phrase = "OK";
				break;
		case STATUS_NO_RESPONSE:		// 204
				status_phrase = "No Response";
				break;
		case STATUS_MOVED:		// 301
				status_phrase = "Moved";
				break;
		case STATUS_FOUND:		// 302
				status_phrase = "Found";
				break;
		case STATUS_METHOD:		// 303
				status_phrase = "Method";
				break;
		case STATUS_NOT_MODIFIED:		// 304
				status_phrase = "Not Modified";
				break;
		case STATUS_BAD_REQUEST:		// 400
				status_phrase = "Bad request";
				break;
		case STATUS_UNAUTHORIZED:		// 401
				status_phrase = "Unauthorized";
				break;
		case STATUS_PAYMENT_REQUIRED:	// 402
				status_phrase = "Payment Required";
				break;
		case STATUS_FORBIDDEN:			// 403
				status_phrase = "Forbidden";
				break;
		case STATUS_NOT_FOUND:			// 404
				status_phrase = "Not Found";
				break;
		case STATUS_INTERNAL_ERROR:		// 500
				status_phrase = "Internal error";
				break;
		case STATUS_NOT_IMPLEMENTED:	// 501
		default: status_phrase = "Not implemented";
	}
	response_line += status_phrase;
}


