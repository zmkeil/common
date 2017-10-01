#ifndef COMMON_PROTOBUF_UTIL_H
#define COMMON_PROTOBUF_UTIL_H

#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>

namespace common {

bool load_protobuf_config(google::protobuf::Message* config);

}

#endif

