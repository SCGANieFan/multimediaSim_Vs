#pragma once
#include "smf_debug.h"
#include "stdbool.h"
#include "minimp4.h"

#define FOUR_CHAR_INT(a, b, c, d) (((uint32_t)(a) << 24) | ((b) << 16) | ((c) << 8) | (d))
enum
{
    BOX_co64    = FOUR_CHAR_INT( 'c', 'o', '6', '4' ),//ChunkLargeOffsetAtomType
    BOX_stco    = FOUR_CHAR_INT( 's', 't', 'c', 'o' ),//ChunkOffsetAtomType
    BOX_crhd    = FOUR_CHAR_INT( 'c', 'r', 'h', 'd' ),//ClockReferenceMediaHeaderAtomType
    BOX_ctts    = FOUR_CHAR_INT( 'c', 't', 't', 's' ),//CompositionOffsetAtomType
    BOX_cprt    = FOUR_CHAR_INT( 'c', 'p', 'r', 't' ),//CopyrightAtomType
    BOX_url_    = FOUR_CHAR_INT( 'u', 'r', 'l', ' ' ),//DataEntryURLAtomType
    BOX_urn_    = FOUR_CHAR_INT( 'u', 'r', 'n', ' ' ),//DataEntryURNAtomType
    BOX_dinf    = FOUR_CHAR_INT( 'd', 'i', 'n', 'f' ),//DataInformationAtomType
    BOX_dref    = FOUR_CHAR_INT( 'd', 'r', 'e', 'f' ),//DataReferenceAtomType
    BOX_stdp    = FOUR_CHAR_INT( 's', 't', 'd', 'p' ),//DegradationPriorityAtomType
    BOX_edts    = FOUR_CHAR_INT( 'e', 'd', 't', 's' ),//EditAtomType
    BOX_elst    = FOUR_CHAR_INT( 'e', 'l', 's', 't' ),//EditListAtomType
    BOX_uuid    = FOUR_CHAR_INT( 'u', 'u', 'i', 'd' ),//ExtendedAtomType
    BOX_free    = FOUR_CHAR_INT( 'f', 'r', 'e', 'e' ),//FreeSpaceAtomType
    BOX_hdlr    = FOUR_CHAR_INT( 'h', 'd', 'l', 'r' ),//HandlerAtomType
    BOX_hmhd    = FOUR_CHAR_INT( 'h', 'm', 'h', 'd' ),//HintMediaHeaderAtomType
    BOX_hint    = FOUR_CHAR_INT( 'h', 'i', 'n', 't' ),//HintTrackReferenceAtomType
    BOX_mdia    = FOUR_CHAR_INT( 'm', 'd', 'i', 'a' ),//MediaAtomType
    BOX_mdat    = FOUR_CHAR_INT( 'm', 'd', 'a', 't' ),//MediaDataAtomType
    BOX_mdhd    = FOUR_CHAR_INT( 'm', 'd', 'h', 'd' ),//MediaHeaderAtomType
    BOX_minf    = FOUR_CHAR_INT( 'm', 'i', 'n', 'f' ),//MediaInformationAtomType
    BOX_moov    = FOUR_CHAR_INT( 'm', 'o', 'o', 'v' ),//MovieAtomType
    BOX_mvhd    = FOUR_CHAR_INT( 'm', 'v', 'h', 'd' ),//MovieHeaderAtomType
    BOX_stsd    = FOUR_CHAR_INT( 's', 't', 's', 'd' ),//SampleDescriptionAtomType
    BOX_stsz    = FOUR_CHAR_INT( 's', 't', 's', 'z' ),//SampleSizeAtomType
    BOX_stz2    = FOUR_CHAR_INT( 's', 't', 'z', '2' ),//CompactSampleSizeAtomType
    BOX_stbl    = FOUR_CHAR_INT( 's', 't', 'b', 'l' ),//SampleTableAtomType
    BOX_stsc    = FOUR_CHAR_INT( 's', 't', 's', 'c' ),//SampleToChunkAtomType
    BOX_stsh    = FOUR_CHAR_INT( 's', 't', 's', 'h' ),//ShadowSyncAtomType
    BOX_skip    = FOUR_CHAR_INT( 's', 'k', 'i', 'p' ),//SkipAtomType
    BOX_smhd    = FOUR_CHAR_INT( 's', 'm', 'h', 'd' ),//SoundMediaHeaderAtomType
    BOX_stss    = FOUR_CHAR_INT( 's', 't', 's', 's' ),//SyncSampleAtomType
    BOX_stts    = FOUR_CHAR_INT( 's', 't', 't', 's' ),//TimeToSampleAtomType
    BOX_trak    = FOUR_CHAR_INT( 't', 'r', 'a', 'k' ),//TrackAtomType
    BOX_tkhd    = FOUR_CHAR_INT( 't', 'k', 'h', 'd' ),//TrackHeaderAtomType
    BOX_tref    = FOUR_CHAR_INT( 't', 'r', 'e', 'f' ),//TrackReferenceAtomType
    BOX_udta    = FOUR_CHAR_INT( 'u', 'd', 't', 'a' ),//UserDataAtomType
    BOX_vmhd    = FOUR_CHAR_INT( 'v', 'm', 'h', 'd' ),//VideoMediaHeaderAtomType
    BOX_url     = FOUR_CHAR_INT( 'u', 'r', 'l', ' ' ),
    BOX_urn     = FOUR_CHAR_INT( 'u', 'r', 'n', ' ' ),

    BOX_gnrv    = FOUR_CHAR_INT( 'g', 'n', 'r', 'v' ),//GenericVisualSampleEntryAtomType
    BOX_gnra    = FOUR_CHAR_INT( 'g', 'n', 'r', 'a' ),//GenericAudioSampleEntryAtomType

    //V2 atoms
    BOX_ftyp    = FOUR_CHAR_INT( 'f', 't', 'y', 'p' ),//FileTypeAtomType
    BOX_padb    = FOUR_CHAR_INT( 'p', 'a', 'd', 'b' ),//PaddingBitsAtomType

    //MP4 Atoms
    BOX_sdhd    = FOUR_CHAR_INT( 's', 'd', 'h', 'd' ),//SceneDescriptionMediaHeaderAtomType
    BOX_dpnd    = FOUR_CHAR_INT( 'd', 'p', 'n', 'd' ),//StreamDependenceAtomType
    BOX_iods    = FOUR_CHAR_INT( 'i', 'o', 'd', 's' ),//ObjectDescriptorAtomType
    BOX_odhd    = FOUR_CHAR_INT( 'o', 'd', 'h', 'd' ),//ObjectDescriptorMediaHeaderAtomType
    BOX_mpod    = FOUR_CHAR_INT( 'm', 'p', 'o', 'd' ),//ODTrackReferenceAtomType
    BOX_nmhd    = FOUR_CHAR_INT( 'n', 'm', 'h', 'd' ),//MPEGMediaHeaderAtomType
    BOX_esds    = FOUR_CHAR_INT( 'e', 's', 'd', 's' ),//ESDAtomType
    BOX_sync    = FOUR_CHAR_INT( 's', 'y', 'n', 'c' ),//OCRReferenceAtomType
    BOX_ipir    = FOUR_CHAR_INT( 'i', 'p', 'i', 'r' ),//IPIReferenceAtomType
    BOX_mp4s    = FOUR_CHAR_INT( 'm', 'p', '4', 's' ),//MPEGSampleEntryAtomType
    BOX_mp4a    = FOUR_CHAR_INT( 'm', 'p', '4', 'a' ),//MPEGAudioSampleEntryAtomType
    BOX_mp4v    = FOUR_CHAR_INT( 'm', 'p', '4', 'v' ),//MPEGVisualSampleEntryAtomType

    // http://www.itscj.ipsj.or.jp/sc29/open/29view/29n7644t.doc
    BOX_avc1    = FOUR_CHAR_INT( 'a', 'v', 'c', '1' ),
    BOX_avc2    = FOUR_CHAR_INT( 'a', 'v', 'c', '2' ),
    BOX_svc1    = FOUR_CHAR_INT( 's', 'v', 'c', '1' ),
    BOX_avcC    = FOUR_CHAR_INT( 'a', 'v', 'c', 'C' ),
    BOX_svcC    = FOUR_CHAR_INT( 's', 'v', 'c', 'C' ),
    BOX_btrt    = FOUR_CHAR_INT( 'b', 't', 'r', 't' ),
    BOX_m4ds    = FOUR_CHAR_INT( 'm', '4', 'd', 's' ),
    BOX_seib    = FOUR_CHAR_INT( 's', 'e', 'i', 'b' ),

    // H264/HEVC
    BOX_hev1    = FOUR_CHAR_INT( 'h', 'e', 'v', '1' ),
    BOX_hvc1    = FOUR_CHAR_INT( 'h', 'v', 'c', '1' ),
    BOX_hvcC    = FOUR_CHAR_INT( 'h', 'v', 'c', 'C' ),

    //3GPP atoms
    BOX_samr    = FOUR_CHAR_INT( 's', 'a', 'm', 'r' ),//AMRSampleEntryAtomType
    BOX_sawb    = FOUR_CHAR_INT( 's', 'a', 'w', 'b' ),//WB_AMRSampleEntryAtomType
    BOX_damr    = FOUR_CHAR_INT( 'd', 'a', 'm', 'r' ),//AMRConfigAtomType
    BOX_s263    = FOUR_CHAR_INT( 's', '2', '6', '3' ),//H263SampleEntryAtomType
    BOX_d263    = FOUR_CHAR_INT( 'd', '2', '6', '3' ),//H263ConfigAtomType

    //V2 atoms - Movie Fragments
    BOX_mvex    = FOUR_CHAR_INT( 'm', 'v', 'e', 'x' ),//MovieExtendsAtomType
    BOX_trex    = FOUR_CHAR_INT( 't', 'r', 'e', 'x' ),//TrackExtendsAtomType
    BOX_moof    = FOUR_CHAR_INT( 'm', 'o', 'o', 'f' ),//MovieFragmentAtomType
    BOX_mfhd    = FOUR_CHAR_INT( 'm', 'f', 'h', 'd' ),//MovieFragmentHeaderAtomType
    BOX_traf    = FOUR_CHAR_INT( 't', 'r', 'a', 'f' ),//TrackFragmentAtomType
    BOX_tfhd    = FOUR_CHAR_INT( 't', 'f', 'h', 'd' ),//TrackFragmentHeaderAtomType
    BOX_tfdt    = FOUR_CHAR_INT( 't', 'f', 'd', 't' ),//TrackFragmentBaseMediaDecodeTimeBox
    BOX_trun    = FOUR_CHAR_INT( 't', 'r', 'u', 'n' ),//TrackFragmentRunAtomType
    BOX_mehd    = FOUR_CHAR_INT( 'm', 'e', 'h', 'd' ),//MovieExtendsHeaderBox

    BOX_mfra = FOUR_CHAR_INT('m', 'f', 'r', 'a'),//
    BOX_mfro = FOUR_CHAR_INT('m', 'f', 'r', 'o'),//
    BOX_tfra = FOUR_CHAR_INT('t', 'f', 'r', 'a'),//

    // Object Descriptors (OD) data coding
    // These takes only 1 byte; this implementation translate <od_tag> to
    // <od_tag> + OD_BASE to keep API uniform and safe for string functions
    OD_BASE    = FOUR_CHAR_INT( '$', '$', '$', '0' ),//
    OD_ESD     = FOUR_CHAR_INT( '$', '$', '$', '3' ),//SDescriptor_Tag
    OD_DCD     = FOUR_CHAR_INT( '$', '$', '$', '4' ),//DecoderConfigDescriptor_Tag
    OD_DSI     = FOUR_CHAR_INT( '$', '$', '$', '5' ),//DecoderSpecificInfo_Tag
    OD_SLC     = FOUR_CHAR_INT( '$', '$', '$', '6' ),//SLConfigDescriptor_Tag

    BOX_meta   = FOUR_CHAR_INT( 'm', 'e', 't', 'a' ),
    BOX_ilst   = FOUR_CHAR_INT( 'i', 'l', 's', 't' ),

    // Metagata tags, see http://atomicparsley.sourceforge.net/mpeg-4files.html
    BOX_calb    = FOUR_CHAR_INT( '\xa9', 'a', 'l', 'b'),    // album
    BOX_cart    = FOUR_CHAR_INT( '\xa9', 'a', 'r', 't'),    // artist
    BOX_aART    = FOUR_CHAR_INT( 'a', 'A', 'R', 'T' ),      // album artist
    BOX_ccmt    = FOUR_CHAR_INT( '\xa9', 'c', 'm', 't'),    // comment
    BOX_cday    = FOUR_CHAR_INT( '\xa9', 'd', 'a', 'y'),    // year (as string)
    BOX_cnam    = FOUR_CHAR_INT( '\xa9', 'n', 'a', 'm'),    // title
    BOX_cgen    = FOUR_CHAR_INT( '\xa9', 'g', 'e', 'n'),    // custom genre (as string or as byte!)
    BOX_trkn    = FOUR_CHAR_INT( 't', 'r', 'k', 'n'),       // track number (byte)
    BOX_disk    = FOUR_CHAR_INT( 'd', 'i', 's', 'k'),       // disk number (byte)
    BOX_cwrt    = FOUR_CHAR_INT( '\xa9', 'w', 'r', 't'),    // composer
    BOX_ctoo    = FOUR_CHAR_INT( '\xa9', 't', 'o', 'o'),    // encoder
    BOX_tmpo    = FOUR_CHAR_INT( 't', 'm', 'p', 'o'),       // bpm (byte)
    BOX_cpil    = FOUR_CHAR_INT( 'c', 'p', 'i', 'l'),       // compilation (byte)
    BOX_covr    = FOUR_CHAR_INT( 'c', 'o', 'v', 'r'),       // cover art (JPEG/PNG)
    BOX_rtng    = FOUR_CHAR_INT( 'r', 't', 'n', 'g'),       // rating/advisory (byte)
    BOX_cgrp    = FOUR_CHAR_INT( '\xa9', 'g', 'r', 'p'),    // grouping
    BOX_stik    = FOUR_CHAR_INT( 's', 't', 'i', 'k'),       // stik (byte)  0 = Movie   1 = Normal  2 = Audiobook  5 = Whacked Bookmark  6 = Music Video  9 = Short Film  10 = TV Show  11 = Booklet  14 = Ringtone
    BOX_pcst    = FOUR_CHAR_INT( 'p', 'c', 's', 't'),       // podcast (byte)
    BOX_catg    = FOUR_CHAR_INT( 'c', 'a', 't', 'g'),       // category
    BOX_keyw    = FOUR_CHAR_INT( 'k', 'e', 'y', 'w'),       // keyword
    BOX_purl    = FOUR_CHAR_INT( 'p', 'u', 'r', 'l'),       // podcast URL (byte)
    BOX_egid    = FOUR_CHAR_INT( 'e', 'g', 'i', 'd'),       // episode global unique ID (byte)
    BOX_desc    = FOUR_CHAR_INT( 'd', 'e', 's', 'c'),       // description
    BOX_clyr    = FOUR_CHAR_INT( '\xa9', 'l', 'y', 'r'),    // lyrics (may be > 255 bytes)
    BOX_tven    = FOUR_CHAR_INT( 't', 'v', 'e', 'n'),       // tv episode number
    BOX_tves    = FOUR_CHAR_INT( 't', 'v', 'e', 's'),       // tv episode (byte)
    BOX_tvnn    = FOUR_CHAR_INT( 't', 'v', 'n', 'n'),       // tv network name
    BOX_tvsh    = FOUR_CHAR_INT( 't', 'v', 's', 'h'),       // tv show name
    BOX_tvsn    = FOUR_CHAR_INT( 't', 'v', 's', 'n'),       // tv season (byte)
    BOX_purd    = FOUR_CHAR_INT( 'p', 'u', 'r', 'd'),       // purchase date
    BOX_pgap    = FOUR_CHAR_INT( 'p', 'g', 'a', 'p'),       // Gapless Playback (byte)

    //BOX_aart   = FOUR_CHAR_INT( 'a', 'a', 'r', 't' ),     // Album artist
    BOX_cART    = FOUR_CHAR_INT( '\xa9', 'A', 'R', 'T'),    // artist
    BOX_gnre    = FOUR_CHAR_INT( 'g', 'n', 'r', 'e'),

    // 3GPP metatags  (http://cpansearch.perl.org/src/JHAR/MP4-Info-1.12/Info.pm)
    BOX_auth    = FOUR_CHAR_INT( 'a', 'u', 't', 'h'),       // author
    BOX_titl    = FOUR_CHAR_INT( 't', 'i', 't', 'l'),       // title
    BOX_dscp    = FOUR_CHAR_INT( 'd', 's', 'c', 'p'),       // description
    BOX_perf    = FOUR_CHAR_INT( 'p', 'e', 'r', 'f'),       // performer
    BOX_mean    = FOUR_CHAR_INT( 'm', 'e', 'a', 'n'),       //
    BOX_name    = FOUR_CHAR_INT( 'n', 'a', 'm', 'e'),       //
    BOX_data    = FOUR_CHAR_INT( 'd', 'a', 't', 'a'),       //

    // these from http://lists.mplayerhq.hu/pipermail/ffmpeg-devel/2008-September/053151.html
    BOX_albm    = FOUR_CHAR_INT( 'a', 'l', 'b', 'm'),      // album
    BOX_yrrc = FOUR_CHAR_INT('y', 'r', 'r', 'c'),       // album

    // alac
    BOX_alac = FOUR_CHAR_INT('a', 'l', 'a', 'c'),       // alac
    BOX_extr = FOUR_CHAR_INT('e', 'x', 't', 'r'),       // extra
};

// Video track : 'vide'
#define MP4E_HANDLER_TYPE_VIDE                                  0x76696465
// Audio track : 'soun'
#define MP4E_HANDLER_TYPE_SOUN                                  0x736F756E
// General MPEG-4 systems streams (without specific handler).
// Used for private stream, as suggested in http://www.mp4ra.org/handler.html
#define MP4E_HANDLER_TYPE_GESM                                  0x6765736D

typedef struct
{
    boxsize_t size;
    boxsize_t offset;
    unsigned duration:31;
    unsigned flag_random_access:1;
} sample_t;

typedef struct
{
    boxsize_t size:28;
    bool flag_random_access : 1;
    uint32_t trunk_id : 3;
} sample32_t;

typedef struct frag_t {
    uint32_t timestamp;
    uint32_t offset;
}frag_t;

typedef struct
{
    boxsize_t size;
} fragframe_t;

typedef struct
{
    uint32_t pos;
    uint32_t size;
    uint64_t data;
} update_item_t;

typedef struct {
    unsigned char* data;
    int bytes;
    int capacity;
} minimp4_vector_t;

typedef struct
{
    uint32_t trunk_id;
    MP4E_track_t info;
    minimp4_vector_t smpl;  // sample descriptor
    minimp4_vector_t pending_sample;

    minimp4_vector_t vsps;  // or dsi for audio
    minimp4_vector_t vpps;  // not used for audio
    minimp4_vector_t vvps;  // used for HEVC
    minimp4_vector_t extra;  // extra data

    minimp4_vector_t fragments;  // fragment descriptor
    uint32_t frame_count;
    uint32_t fragments_count;
    uint32_t duration;
    uint32_t first_timestamp;
    uint32_t first_timestamp_pos;
} track_t;

typedef struct MP4E_mux_tag
{
    minimp4_vector_t tracks;

    int64_t write_pos;
    int (*write_callback)(int64_t offset, const void* buffer, size_t size, void* token);
    void* token;
    char* text_comment;
    int text_comment_len;

    int sequential_mode_flag;
    bool enable_fragmentation; // flag, indicating streaming-friendly 'fragmentation' mode
    int fragments_count;      // # of fragments in 'fragmentation' mode
    int duration_pos;
} MP4E_mux_t;


/**
*   Endian-independent byte-write macros
*/
#define WR(x, n) *p = (unsigned char)((x) >> 8*n); p++
#define WRITE_1(x) WR(x, 0);
#define WRITE_2(x) WR(x, 1); WR(x, 0);
#define WRITE_3(x) WR(x, 2); WR(x, 1); WR(x, 0);
#define WRITE_4(x) WR(x, 3); WR(x, 2); WR(x, 1); WR(x, 0);
#define WR4(p, x) (p)[0] = (char)((x) >> 8*3); (p)[1] = (char)((x) >> 8*2); (p)[2] = (char)((x) >> 8*1); (p)[3] = (char)((x));
static inline uint32_t Swap32(uint32_t v) { return (v >> 24) | (v << 24) | (v >> 8 << 24 >> 8) | (v << 8 >> 24 << 8); }
static inline uint16_t Swap16(uint16_t v) { return (v >> 8) | (v << 8); }

// Finish atom: update atom size field
#define END_ATOM --stack; WR4((unsigned char*)*stack, p - *stack);

// Initiate atom: save position of size field on stack
#define ATOM(x)  *stack++ = p; p += 4; WRITE_4(x);

// Atom with 'FullAtomVersionFlags' field
#define ATOM_FULL(x, flag)  ATOM(x); WRITE_4(flag);

//#define ERR(func) { int err = func; if (err) return err; }
#define ERR(func) {int err=func; returnIfErrCS(err,err,"%d",err);}

#define CHUNK_OFFSET_COUNT 1000
#define MOOV_TIMESCALE 1000

EXTERNC void* Mp4Malloc(uint32_t size);
EXTERNC void* Mp4Realloc(void* ptr, uint32_t size);
EXTERNC void Mp4Free(void* ptr);

#define malloc Mp4Malloc
#define realloc Mp4Realloc
#define free Mp4Free

#define NELEM(x)  (sizeof(x) / sizeof((x)[0]))

#if MP4D_TRACE_SUPPORTED
#define TRACE(s,...) dbgErrPXL(s,##__VA_ARGS__)
#else
#define TRACE(fmt, ...) //printf("%s/%d#%s()" fmt "\n", strrchr(__FILE__,'\\') + 1, __LINE__, __func__,  ##__VA_ARGS__)
#endif
