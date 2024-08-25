#include "Base64Helper.hpp"

#include <b64/decode.h>
#include <b64/encode.h>
#include <sstream>

std::string Base64Helper::ToBase64(const std::string_view str) {
    std::istringstream incoming(str.data());
    std::stringstream outgoing;
    base64::encoder encoder;
    encoder.encode(incoming, outgoing);
    outgoing.flush();
    std::string result = outgoing.str();
    result.erase(result.size() - 1);
    return result;
}

std::string Base64Helper::FromBase64(const std::string_view str) {
    std::istringstream incoming(str.data());
    std::stringstream outgoing;
    base64::decoder decoder;
    decoder.decode(incoming, outgoing);
    outgoing.flush();
    return outgoing.str();
}
