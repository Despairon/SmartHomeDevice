#pragma once

#include <string>
#include <map>
#include <stdint.h>

namespace HttpMessage_n
{
    enum class HttpMethod : uint8_t
    {
        GET = 1,
        POST,
        PUT,
        DELETE
    };

    enum class HttpVersion : uint8_t
    {
        HTTP_1_0 = 1,
        HTTP_1_1
    };

    enum class HttpStatus : uint8_t
    {
        // 1xx response codes
        _100_CONTINUE = 1,
        _101_SWITCHING_PROTOCOLS,
        _102_PROCESSING,
        _103_EARLY_HINTS,

        // 2xx response codes
        _200_OK,
        _201_CREATED,
        _202_ACCEPTED,
        _203_NON_AUTHORITATIVE_INFORMATION,
        _204_NO_CONTENT,
        _205_RESET_CONTENT,
        _206_PARTIAL_CONTENT,
        _207_MULTI_STATUS,
        _208_ALREADY_REPORTED,
        _226_IM_USED,

        // 3xx response codes
        _300_MULTIPLE_CHOICES,
        _301_MOVED_PERMANENTLY,
        _302_FOUND,
        _303_SEE_OTHER,
        _304_NOT_MODIFIED,
        _305_USE_PROXY,
        _306_SWITCH_PROXY,
        _307_TEMPORARY_REDIRECT,
        _308_PERMANENT_REDIRECT,

        // 4xx response codes
        _400_BAD_REQUEST,
        _401_UNAUTHORIZED,
        _402_PAYMENT_REQUIRED,
        _403_FORBIDDEN,
        _404_NOT_FOUND,
        _405_METHOD_NOT_ALLOWED,
        _406_NOT_ACCEPTABLE,
        _407_PROXY_AUTHENTICATION_REQUIRED,
        _408_REQUEST_TIMEOUT,
        _409_CONFLICT,
        _410_GONE,
        _411_LENGTH_REQUIRED,
        _412_PRECONDITION_FAILED,
        _413_PAYLOAD_TOO_LARGE,
        _414_URI_TOO_LONG,
        _415_UNSUPPORTED_MEDIA_TYPE,
        _416_RANGE_NOT_SATISFIABLE,
        _417_EXPECTATION_FAILED,
        _418_IM_A_TEAPOT,
        _421_MISDIRECTED_REQUEST,
        _422_UNPROCESSABLE_ENTITY,
        _423_LOCKED,
        _424_FAILED_DEPENDENCY,
        _426_UPGRADE_REQUIRED,
        _428_PRECONDITION_REQUIRED,
        _429_TOO_MANY_REQUESTS,
        _431_REQUEST_HEADER_FIELDS_TOO_LARGE,
        _451_UNAVAILABLE_FOR_LEGAL_REASONS,

        // 5xx response codes
        _500_INTERNAL_SERVER_ERROR,
        _501_NOT_IMPLEMENTED,
        _502_BAD_GATEWAY,
        _503_SERVICE_UNAVAILABLE,
        _504_GATEWAY_TIMEOUT,
        _505_HTTP_VERSION_NOT_SUPPORTED,
        _506_VARIANT_ALSO_NEGOTIATES,
        _507_INSUFFICIENT_STORAGE,
        _508_LOOP_DETECTED,
        _510_NOT_EXTENDED,
        _511_NETWORK_AUTHENTICATION_REQUIRED
    };

    enum class HttpHeader : uint8_t
    {
        // Request fields
        ACCEPT = 1,
        ACCEPT_CHARSET,
        ACCEPT_ENCODING,
        ACCEPT_LANGUAGE,
        ACCEPT_DATETIME,
        ACCESS_CONTROL_REQUEST_METHOD,
        ACCESS_CONTROL_REQUEST_HEADERS,
        AUTHORIZATION,
        CACHE_CONTROL,
        CONNECTION,
        COOKIE,
        CONTENT_LENGTH,
        CONTENT_MD5,
        CONTENT_TYPE,
        DATE,
        EXPECT,
        FORWARDED,
        FROM,
        HOST,
        IF_MATCH,
        IF_MODIFIED_SINCE,
        IF_NONE_MATCH,
        IF_RANGE,
        IF_UNMODIFIED_SINCE,
        MAX_FORWARDS,
        ORIGIN,
        PRAGMA,
        PROXY_AUTHORIZATION,
        RANGE,
        REFERER,
        TE,
        USER_AGENT,
        UPGRADE,
        VIA,
        WARNING,

        // response fields
        ACCEPT_PATCH,
        ACCEPT_RANGES,
        ACCESS_CONTROL_ALLOW_ORIGIN,
        ACCESS_CONTROL_ALLOW_CREDENTIALS,
        ACCESS_CONTROL_EXPOSE_HEADERS,
        ACCESS_CONTROL_MAX_AGE,
        ACCESS_CONTROL_ALLOW_METHODS,
        ACCESS_CONTROL_ALLOW_HEADERS,
        AGE,
        ALLOW,
        ALT_SVC,
        CONTENT_DISPOSITION,
        CONTENT_ENCODING,
        CONTENT_LANGUAGE,
        CONTENT_LOCATION,
        CONTENT_RANGE,
        ETAG,
        EXPIRES,
        LAST_MODIFIED,
        LINK,
        LOCATION,
        P3P,
        PROXY_AUTHENTICATE,
        PUBLIC_KEY_PINS,
        RETRY_AFTER,
        SERVER,
        SET_COOKIE,
        STRICT_TRANSPORT_SECURITY,
        TK,
        TRAILER,
        TRANSFER_ENCODING,
        VARY,
        WWW_AUTHENTICATE,
        X_FRAME_OPTIONS,
        X_PAD
    };

    class HttpMessage
    {
    private:
        class Defines
        {
        public:
            static constexpr auto ENDL = "\r\n";
            static constexpr auto INVALID_VALUE = -1;
        };

        std::string _method;
        std::string _request;
        std::string _version;
        std::string _status;
        std::string _header;
        std::string _body;
        bool        _isValid;

    private:
        static const std::map<HttpMethod,  std::string> httpMethodsMap;
        static const std::map<HttpVersion, std::string> httpVersionsMap;
        static const std::map<HttpStatus,  std::string> httpStatusesMap;
        static const std::map<HttpHeader,  std::string> httpHeadersMap;

    public:
        // constructor and destructor
        HttpMessage(const std::string&);
        HttpMessage();
        ~HttpMessage();

        // miscellanous
        const std::string rawText() const;
        void              clear();

        // to string methods
        static const std::string getHttpMethodStr(const HttpMethod&);
        static const std::string getHttpVersionStr(const HttpVersion&);
        static const std::string getHttpStatusStr(const HttpStatus&);
        static const std::string getHttpHeaderStr(const HttpHeader&);

        //getters
        const HttpMethod    method() const;
        const std::string   &request() const;
        const HttpVersion   version() const;
        const HttpStatus    status() const;
        const std::string   header(const HttpHeader&) const;
        const std::string   &body() const;
        const bool          &isValid() const;

        //setters
        HttpMessage& setRequestLine(const HttpMethod&, const std::string&, const HttpVersion&);
        HttpMessage& setResponseLine(const HttpVersion&, const HttpStatus&);
        HttpMessage& appendHeader(const HttpHeader&, const std::string&);
        HttpMessage& appendBody(const std::string&);
    };

}