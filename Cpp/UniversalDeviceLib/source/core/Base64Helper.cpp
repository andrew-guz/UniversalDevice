#include "Base64Helper.h"

#include <sstream>
#include <b64/encode.h>
#include <b64/decode.h>

std::string Base64Helper::ToBase64(const std::string& str)
{
    std::istringstream incoming(str);
    std::stringstream outgoing;
    base64::encoder encoder;
    encoder.encode(incoming, outgoing);
    outgoing.flush();
    return outgoing.str();
}

std::string Base64Helper::FromBase64(const std::string& str)
{
    std::istringstream incoming(str);
    std::stringstream outgoing;
    base64::decoder decoder;
    decoder.decode(incoming, outgoing);
    outgoing.flush();
    return outgoing.str();
}
