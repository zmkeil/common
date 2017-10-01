#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include "comlog/info_log_context.h"

namespace common {

bool load_protobuf_config(::google::protobuf::Message* config)
{
	std::string full_path = "conf/startup/service.conf";
	int fd = ::open(full_path.c_str(), O_RDONLY);
	if (fd < 0) {
		LOG(ERROR, "Failed to open config file \"conf/startup/service.conf\"");
		return false;
	}
	::google::protobuf::io::FileInputStream stream(fd);
	stream.SetCloseOnDelete(true);
	return ::google::protobuf::TextFormat::Parse(&stream, config);
}

}
