#include "HttpMessage.h"

#include <vector>
#include <algorithm>

namespace HttpMessage_n
{
    //***************************************
    // Static values
    //***************************************
    const std::map<HttpMethod, std::string>HttpMessage::httpMethodsMap =
    {
        {HttpMethod::GET,    std::string("GET")},
        {HttpMethod::POST,   std::string("POST")},
        {HttpMethod::PUT,    std::string("PUT")},
        {HttpMethod::DELETE, std::string("DELETE")}
    };

    const std::map<HttpVersion, std::string>HttpMessage::httpVersionsMap =
    {
        {HttpVersion::HTTP_1_0, std::string("HTTP/1.0")},
        {HttpVersion::HTTP_1_1, std::string("HTTP/1.1")}
    };

    const std::map<HttpStatus, std::string>HttpMessage::httpStatusesMap =
    {
        // 1xx response codes
        {HttpStatus::_100_CONTINUE,                        std::string("100 Continue")},
        {HttpStatus::_101_SWITCHING_PROTOCOLS,             std::string("101 Switching Protocols")},
        {HttpStatus::_102_PROCESSING,                      std::string("102 Processing")},
        {HttpStatus::_103_EARLY_HINTS,                     std::string("103 Early Hints")},

        // 2xx response codes
        {HttpStatus::_200_OK,                              std::string("200 OK")},
        {HttpStatus::_201_CREATED,                         std::string("201 Created")},
        {HttpStatus::_202_ACCEPTED,                        std::string("202 Accepted")},
        {HttpStatus::_203_NON_AUTHORITATIVE_INFORMATION,   std::string("203 Non-Authoritative Information")},
        {HttpStatus::_204_NO_CONTENT,                      std::string("204 No Content")},
        {HttpStatus::_205_RESET_CONTENT,                   std::string("205 Reset Content")},
        {HttpStatus::_206_PARTIAL_CONTENT,                 std::string("206 Partial Content")},
        {HttpStatus::_207_MULTI_STATUS,                    std::string("207 Multi-Status")},
        {HttpStatus::_208_ALREADY_REPORTED,                std::string("208 Already Reported")},
        {HttpStatus::_226_IM_USED,                         std::string("226 IM Used")},

        // 3xx response codes
        {HttpStatus::_300_MULTIPLE_CHOICES,                std::string("300 Multiple Choices")},
        {HttpStatus::_301_MOVED_PERMANENTLY,               std::string("301 Moved Permanently")},
        {HttpStatus::_302_FOUND,                           std::string("302 Found")},
        {HttpStatus::_303_SEE_OTHER,                       std::string("303 See Other")},
        {HttpStatus::_304_NOT_MODIFIED,                    std::string("304 Not Modified")},
        {HttpStatus::_305_USE_PROXY,                       std::string("305 Use Proxy")},
        {HttpStatus::_306_SWITCH_PROXY,                    std::string("306 Switch Proxy")},
        {HttpStatus::_307_TEMPORARY_REDIRECT,              std::string("307 Temporary Redirect")},
        {HttpStatus::_308_PERMANENT_REDIRECT,              std::string("308 Permanent Redirect")},

        // 4xx response codes
        {HttpStatus::_400_BAD_REQUEST,                     std::string("400 Bad Request")},
        {HttpStatus::_401_UNAUTHORIZED,                    std::string("401 Unauthorized")},
        {HttpStatus::_402_PAYMENT_REQUIRED,                std::string("402 Payment Required")},
        {HttpStatus::_403_FORBIDDEN,                       std::string("403 Forbidden")},
        {HttpStatus::_404_NOT_FOUND,                       std::string("404 Not Found")},
        {HttpStatus::_405_METHOD_NOT_ALLOWED,              std::string("405 Method Not Allowed")},
        {HttpStatus::_406_NOT_ACCEPTABLE,                  std::string("406 Not Acceptable")},
        {HttpStatus::_407_PROXY_AUTHENTICATION_REQUIRED,   std::string("407 Proxy Authentication Required")},
        {HttpStatus::_408_REQUEST_TIMEOUT,                 std::string("408 Request Timeout")},
        {HttpStatus::_409_CONFLICT,                        std::string("409 Conflict")},
        {HttpStatus::_410_GONE,                            std::string("410 Gone")},
        {HttpStatus::_411_LENGTH_REQUIRED,                 std::string("411 Length Required")},
        {HttpStatus::_412_PRECONDITION_FAILED,             std::string("412 Precondition Failed")},
        {HttpStatus::_413_PAYLOAD_TOO_LARGE,               std::string("413 Payload Too Large")},
        {HttpStatus::_414_URI_TOO_LONG,                    std::string("414 URI Too Long")},
        {HttpStatus::_415_UNSUPPORTED_MEDIA_TYPE,          std::string("415 Unsupported Media Type")},
        {HttpStatus::_416_RANGE_NOT_SATISFIABLE,           std::string("416 Range Not Satisfiable")},
        {HttpStatus::_417_EXPECTATION_FAILED,              std::string("417 Expectation Failed")},
        {HttpStatus::_418_IM_A_TEAPOT,                     std::string("418 I'm a teapot")},
        {HttpStatus::_421_MISDIRECTED_REQUEST,             std::string("421 Misdirected Request")},
        {HttpStatus::_422_UNPROCESSABLE_ENTITY,            std::string("422 Unprocessable Entity")},
        {HttpStatus::_423_LOCKED,                          std::string("423 Locked")},
        {HttpStatus::_424_FAILED_DEPENDENCY,               std::string("424 Failed Dependency")},
        {HttpStatus::_426_UPGRADE_REQUIRED,                std::string("426 Upgrade Required")},
        {HttpStatus::_428_PRECONDITION_REQUIRED,           std::string("428 Precondition Required")},
        {HttpStatus::_429_TOO_MANY_REQUESTS,               std::string("429 Too Many Requests")},
        {HttpStatus::_431_REQUEST_HEADER_FIELDS_TOO_LARGE, std::string("431 Request Header Fields Too Large")},
        {HttpStatus::_451_UNAVAILABLE_FOR_LEGAL_REASONS,   std::string("451 Unavailable For Legal Reasons")},

        // 5xx response codes
        {HttpStatus::_500_INTERNAL_SERVER_ERROR,           std::string("500 Internal Server Error")},
        {HttpStatus::_501_NOT_IMPLEMENTED,                 std::string("501 Not Implemented")},
        {HttpStatus::_502_BAD_GATEWAY,                     std::string("502 Bad Gateway")},
        {HttpStatus::_503_SERVICE_UNAVAILABLE,             std::string("503 Service Unavailable")},
        {HttpStatus::_504_GATEWAY_TIMEOUT,                 std::string("504 Gateway Timeout")},
        {HttpStatus::_505_HTTP_VERSION_NOT_SUPPORTED,      std::string("505Http Version Not Supported")},
        {HttpStatus::_506_VARIANT_ALSO_NEGOTIATES,         std::string("506 Variant Also Negotiates")},
        {HttpStatus::_507_INSUFFICIENT_STORAGE,            std::string("507 Insufficient Storage")},
        {HttpStatus::_508_LOOP_DETECTED,                   std::string("508 Loop Detected")},
        {HttpStatus::_510_NOT_EXTENDED,                    std::string("510 Not Extended")},
        {HttpStatus::_511_NETWORK_AUTHENTICATION_REQUIRED, std::string("511 Network Authentication Required")},
    };

    const std::map<HttpHeader, std::string>HttpMessage::httpHeadersMap =
    {
        {HttpHeader::ACCEPT,                              std::string("Accept")},
        {HttpHeader::ACCEPT_CHARSET,                      std::string("Accept-Charset")},
        {HttpHeader::ACCEPT_ENCODING,                     std::string("Accept-Encoding")},
        {HttpHeader::ACCEPT_LANGUAGE,                     std::string("Accept-Language")},
        {HttpHeader::ACCEPT_DATETIME,                     std::string("Accept-Datetime")},
        {HttpHeader::ACCESS_CONTROL_REQUEST_METHOD,       std::string("Access-Control-Request-Method")},
        {HttpHeader::ACCESS_CONTROL_REQUEST_HEADERS,      std::string("Access-Control-Request-Headers")},
        {HttpHeader::AUTHORIZATION,                       std::string("Authorization")},
        {HttpHeader::CACHE_CONTROL,                       std::string("Cache-Control")},
        {HttpHeader::CONNECTION,                          std::string("Connection")},
        {HttpHeader::COOKIE,                              std::string("Cookie")},
        {HttpHeader::CONTENT_LENGTH,                      std::string("Content-Length")},
        {HttpHeader::CONTENT_MD5,                         std::string("Content-MD5")},
        {HttpHeader::CONTENT_TYPE,                        std::string("Content-Type")},
        {HttpHeader::DATE,                                std::string("Date")},
        {HttpHeader::EXPECT,                              std::string("Expect")},
        {HttpHeader::FORWARDED,                           std::string("Forwarded")},
        {HttpHeader::FROM,                                std::string("From")},
        {HttpHeader::HOST,                                std::string("Host")},
        {HttpHeader::IF_MATCH,                            std::string("If-Match")},
        {HttpHeader::IF_MODIFIED_SINCE,                   std::string("If-Modified-Since")},
        {HttpHeader::IF_NONE_MATCH,                       std::string("If-None-Match")},
        {HttpHeader::IF_RANGE,                            std::string("If-Range")},
        {HttpHeader::IF_UNMODIFIED_SINCE,                 std::string("If-Unmodified-Since")},
        {HttpHeader::MAX_FORWARDS,                        std::string("Max-Forwards")},
        {HttpHeader::ORIGIN,                              std::string("Origin")},
        {HttpHeader::PRAGMA,                              std::string("Pragma")},
        {HttpHeader::PROXY_AUTHORIZATION,                 std::string("Proxy-Authorization")},
        {HttpHeader::RANGE,                               std::string("Range")},
        {HttpHeader::REFERER,                             std::string("Referer")},
        {HttpHeader::TE,                                  std::string("TE")},
        {HttpHeader::USER_AGENT,                          std::string("User-Agent")},
        {HttpHeader::UPGRADE,                             std::string("Upgrade")},
        {HttpHeader::VIA,                                 std::string("Via")},
        {HttpHeader::WARNING,                             std::string("Warning")},
        {HttpHeader::ACCEPT_PATCH,                        std::string("Accept-Patch")},
        {HttpHeader::ACCEPT_RANGES,                       std::string("Accept-Ranges")},
        {HttpHeader::ACCESS_CONTROL_ALLOW_ORIGIN,         std::string("Access-Control-Allow-Origin")},
        {HttpHeader::ACCESS_CONTROL_ALLOW_CREDENTIALS,    std::string("Access-Control-Allow-Credentials")},
        {HttpHeader::ACCESS_CONTROL_EXPOSE_HEADERS,       std::string("Access-Control-Expose-Headers")},
        {HttpHeader::ACCESS_CONTROL_MAX_AGE,              std::string("Access-Control-Max-Age")},
        {HttpHeader::ACCESS_CONTROL_ALLOW_METHODS,        std::string("Access-Control-Allow-Methods")},
        {HttpHeader::ACCESS_CONTROL_ALLOW_HEADERS,        std::string("Access-Control-Allow-Headers")},
        {HttpHeader::AGE,                                 std::string("Age")},
        {HttpHeader::ALLOW,                               std::string("Allow")},
        {HttpHeader::ALT_SVC,                             std::string("Alt-Svc")},
        {HttpHeader::CONTENT_DISPOSITION,                 std::string("Content-Disposition")},
        {HttpHeader::CONTENT_ENCODING,                    std::string("Content-Encoding")},
        {HttpHeader::CONTENT_LANGUAGE,                    std::string("Content-Language")},
        {HttpHeader::CONTENT_LOCATION,                    std::string("Content-Location")},
        {HttpHeader::CONTENT_RANGE,                       std::string("Content-Range")},
        {HttpHeader::ETAG,                                std::string("ETag")},
        {HttpHeader::EXPIRES,                             std::string("Expires")},
        {HttpHeader::LAST_MODIFIED,                       std::string("Last-Modified")},
        {HttpHeader::LINK,                                std::string("Link")},
        {HttpHeader::LOCATION,                            std::string("Location")},
        {HttpHeader::P3P,                                 std::string("P3P")},
        {HttpHeader::PROXY_AUTHENTICATE,                  std::string("Proxy-Authenticate")},
        {HttpHeader::PUBLIC_KEY_PINS,                     std::string("Public-Key-Pins")},
        {HttpHeader::RETRY_AFTER,                         std::string("Retry-After")},
        {HttpHeader::SERVER,                              std::string("Server")},
        {HttpHeader::SET_COOKIE,                          std::string("Set-Cookie")},
        {HttpHeader::STRICT_TRANSPORT_SECURITY,           std::string("Strict-Transport-Security")},
        {HttpHeader::TK,                                  std::string("Tk")},
        {HttpHeader::TRAILER,                             std::string("Trailer")},
        {HttpHeader::TRANSFER_ENCODING,                   std::string("Transfer-Encoding")},
        {HttpHeader::VARY,                                std::string("Vary")},
        {HttpHeader::WWW_AUTHENTICATE,                    std::string("WWW-Authenticate")},
        {HttpHeader::X_FRAME_OPTIONS,                     std::string("X-Frame-Options")},
        {HttpHeader::X_PAD,                               std::string("X-Pad")}
    };

    //***************************************
    // Constructors and destructors
    //***************************************

    HttpMessage::HttpMessage(const std::string &text)
    {
        auto rawText = text;

        if (rawText != std::string())
        {
            auto reqResLineEndInd = rawText.find(Defines::ENDL);
            auto reqResLine = rawText.substr(0, reqResLineEndInd);
            auto reqResFields = std::vector<std::string>();
            
            auto currStr = std::string("");
            for(auto chr : reqResLine)
            {
                if (chr != ' ')
                {
                    currStr += chr;
                    continue;
                }
                else
                {
                    reqResFields.push_back(currStr);
                    currStr = std::string("");
                }
            }
            

            if (!reqResFields.empty())
            {
                auto firstReqResField = reqResFields.at(0);

                auto httpMethodFromMap = std::find_if(httpMethodsMap.begin(), httpMethodsMap.end(), [&firstReqResField](const std::pair<HttpMethod, std::string> &pair) -> bool { return pair.second == firstReqResField; });

                if (httpMethodFromMap != httpMethodsMap.end() && (reqResFields.size() > 2))
                {
                    // if first word is a known http method,
                    // this is a request type message
                    _method  = reqResFields.at(0);
                    _request = reqResFields.at(1);
                    _version = reqResFields.at(2);
                }
                else 
                {
                    auto httpVersionFromMap = std::find_if(httpVersionsMap.begin(), httpVersionsMap.end(), [&firstReqResField](const std::pair<HttpVersion, std::string> &pair) -> bool { return pair.second == firstReqResField; });

                    if ( (httpVersionFromMap != httpVersionsMap.end()) && (reqResFields.size() > 2) )
                    {
                        // if first word is a known http version,
                        // this is a response type message
                        _version = firstReqResField;
                        reqResFields.erase(reqResFields.begin());
                        
                        _status = "";
                        for (auto field : reqResFields)
                            _status += field + ' ';
                        _status.pop_back(); // remove trailing ' ' symbol (space)
                    }
                    else 
                        return; // TODO: debug: "invalid HTTP message format";
                }
            }

            auto endlInd = rawText.find(Defines::ENDL);

            if (endlInd >= 0)
                rawText.erase(0, endlInd + 1);

            auto headerEndInd = rawText.find(std::string(Defines::ENDL) + std::string(Defines::ENDL));  // search for the end of the header which is two endlines (\r\n)

            if (headerEndInd >= 0)
            {
                _header = rawText.substr(0, headerEndInd);
                rawText.erase(0, headerEndInd + 3);

                auto bodyEndInd = rawText.find(std::string(Defines::ENDL) + std::string(Defines::ENDL)); // search for the end of the body

                if (bodyEndInd >= 0)
                {
                    _body = rawText.substr(3, bodyEndInd);
                    _isValid = true;
                }
            }

            if (!_isValid)
                return; // TODO: debug: "invalid HTTP message format";
        }
        else
        {
            clear();
        }
    }

    HttpMessage::HttpMessage()
    {
        clear();
    }

    HttpMessage::~HttpMessage() { }

    //***************************************
    // Miscellanous methods
    //***************************************

    const std::string HttpMessage::rawText() const
    {
        if (_isValid)
        {
            return   _method + (_request == "" ? "" : " ")
                + _request   + (_version == "" ? "" : " ")
                + _version   + (_status  == "" ? "" : " ")
                + _status    + Defines::ENDL
                + _header    + Defines::ENDL
                + _body;
        }
        else return std::string("");
    }

    void HttpMessage::clear()
    {
        _method  = "";
        _request = "";
        _version = "";
        _status  = "";
        _header  = "";
        _body    = "";
        _isValid = false;
    }

    //***************************************
    // To string methods
    //***************************************
    const std::string HttpMessage::getHttpMethodStr(const HttpMethod &httpMethod)
    {
        if (httpMethodsMap.find(httpMethod) != httpMethodsMap.end())
            return httpMethodsMap.at(httpMethod);
        else
            return std::string();
    }

    const std::string HttpMessage::getHttpVersionStr(const HttpVersion &httpVersion)
    {
        if (httpVersionsMap.find(httpVersion) != httpVersionsMap.end())
            return httpVersionsMap.at(httpVersion);
        else
            return std::string();
    }

    const std::string HttpMessage::getHttpStatusStr(const HttpStatus &httpStatus)
    {
        if (httpStatusesMap.find(httpStatus) != httpStatusesMap.end())
            return httpStatusesMap.at(httpStatus);
        else
            return std::string();
    }

    const std::string HttpMessage::getHttpHeaderStr(const HttpHeader &httpHeader)
    {
        if (httpHeadersMap.find(httpHeader) != httpHeadersMap.end())
            return httpHeadersMap.at(httpHeader);
        else
            return std::string();
    }

    //***************************************
    // Getters
    //***************************************

    const HttpMethod HttpMessage::method() const
    {
        auto __method = _method;

        auto httpMethodFromMap = std::find_if(httpMethodsMap.begin(), httpMethodsMap.end(), [&__method](const std::pair<HttpMethod, std::string> &pair) -> bool { return pair.second == __method; });

        if (httpMethodFromMap != httpMethodsMap.end())
            return httpMethodFromMap->first;
        else
            return static_cast<HttpMethod>(Defines::INVALID_VALUE);
    }

    const std::string &HttpMessage::request() const
    {
        return _request;
    }

    const HttpVersion HttpMessage::version() const
    {
        auto __version = _version;

        auto httpVersionFromMap = std::find_if(httpVersionsMap.begin(), httpVersionsMap.end(), [&__version](const std::pair<HttpVersion, std::string> &pair) -> bool { return pair.second == __version; });

        if (httpVersionFromMap != httpVersionsMap.end())
            return httpVersionFromMap->first;
        else
            return static_cast<HttpVersion>(Defines::INVALID_VALUE);
    }

    const HttpStatus HttpMessage::status() const
    {
        auto __status = _status;

        auto httpStatusFromMap = std::find_if(httpStatusesMap.begin(), httpStatusesMap.end(), [&__status](const std::pair<HttpStatus, std::string> &pair) -> bool { return pair.second == __status; });

        if (httpStatusFromMap != httpStatusesMap.end())
            return httpStatusFromMap->first;
        else
            return static_cast<HttpStatus>(Defines::INVALID_VALUE);
    }

    const std::string HttpMessage::header(const HttpHeader &hdr) const
    {
        auto headerStr = httpHeadersMap.at(hdr);

        if (headerStr != std::string(""))
        {
            if (_header.find(headerStr) != std::string::npos)
            {
                auto hdrStr = _header;
                auto hdrInd = hdrStr.find(headerStr);
                
                if (hdrInd >= 0)
                {
                    hdrStr.erase(0, hdrInd);
                    hdrInd = hdrStr.find(Defines::ENDL);

                    if (hdrInd >= 0)
                    {
                        auto delim_ind = hdrStr.find(':');
                        auto endl_ind  = hdrStr.find(Defines::ENDL);

                        if ((delim_ind >= 0) && (endl_ind >= 0))
                        {
                            hdrStr = hdrStr.substr(delim_ind + 2, endl_ind - delim_ind - 2);

                            return hdrStr;
                        }
                        else
                            return "";
                    }
                    else
                        return "";
                }
                else return "";
            }
            else return "";
        }
        else return "";
    }

    const std::string &HttpMessage::body() const
    {
        return _body;
    }

    const bool &HttpMessage::isValid() const
    {
        return _isValid;
    }

    //***************************************
    // Setters
    //***************************************
    HttpMessage &HttpMessage::setRequestLine(const HttpMethod &method, const std::string &request, const HttpVersion &version)
    {
        _method  = "";
        _request = "";
        _version = "";
        _status  = "";

        _method  = httpMethodsMap.at(method);
        _request = request;
        _version = httpVersionsMap.at(version);

        auto __method  = _method;
        auto __version = _version;

        auto httpMethodFromMap  = std::find_if(httpMethodsMap.begin(), httpMethodsMap.end(), [&__method](const std::pair<HttpMethod, std::string> &pair) -> bool { return pair.second == __method; });
        auto httpVersionFromMap = std::find_if(httpVersionsMap.begin(), httpVersionsMap.end(), [&__version](const std::pair<HttpVersion, std::string> &pair) -> bool { return pair.second == __version; });

        _isValid = (httpMethodFromMap != httpMethodsMap.end()) && (httpVersionFromMap != httpVersionsMap.end());

        return *this;
    }

    HttpMessage &HttpMessage::setResponseLine(const HttpVersion &version, const HttpStatus &status)
    {
        _method  = "";
        _request = "";
        _version = "";
        _status  = "";

        _version = httpVersionsMap.at(version);
        _status  = httpStatusesMap.at(status);

        auto __version = _version;
        auto __status  = _status;

        auto httpVersionFromMap = std::find_if(httpVersionsMap.begin(), httpVersionsMap.end(), [&__version](const std::pair<HttpVersion, std::string> &pair) -> bool { return pair.second == __version; });
        auto httpStatusFromMap = std::find_if(httpStatusesMap.begin(), httpStatusesMap.end(), [&__status](const std::pair<HttpStatus, std::string> &pair) -> bool { return pair.second == __status; });

        _isValid = (httpStatusFromMap != httpStatusesMap.end()) && (httpVersionFromMap != httpVersionsMap.end());

        return *this;
    }

    HttpMessage &HttpMessage::appendHeader(const HttpHeader &header, const std::string &value)
    {
        if (httpHeadersMap.find(header) != httpHeadersMap.end())
        {
            auto headerLine = httpHeadersMap.at(header) + ':' + " " + value + Defines::ENDL;

            _header += headerLine;
        }

        return *this;
    }

    HttpMessage &HttpMessage::appendBody(const std::string &bodyStr)
    {
        _body += bodyStr;

        return *this;
    }

}
