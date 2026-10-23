#include "StreamServer.h"
#include "StreamServer.audio.h"
#include "SmfFCC.h"
using namespace smf;
using namespace smf::msg;
using namespace smf::ssvc;

EXTERNC void  smf_audio_player_register() {
	Object::Register("mclient-aplayer-server", &AudioServer::Create<fcc64("apserver"), (uint16_t)eStream::audioPlayer, fcc64("mix")>);
}
EXTERNC void  smf_audio_record_register() {
	Object::Register("mclient-arecord-server", &AudioServer::Create<fcc64("arserver"), (uint16_t)eStream::audioRecord, fcc64("link")>);
}
EXTERNC void  smf_stream_server_register() {
	Object::Register("mclient-stream-server", &StreamServer::Create<fcc64("sserver"), (uint16_t)eStream::streamServer>);
}
EXTERNC void  smf_stream_server_all_register() {
	Object::Register("mclient-stream-server-all", &StreamServer::Create<fcc64("sserver"), (uint16_t)eStream::null>);
}
