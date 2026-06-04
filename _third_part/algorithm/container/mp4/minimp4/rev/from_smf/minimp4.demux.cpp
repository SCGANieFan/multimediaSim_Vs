#define MINIMP4_IMPLEMENTATION
#include "smf_debug.h"
// #include "minimp4.h"
#include "minimp4.def.h"
#include "alac.h"

extern "C" {
static int minimp4_fgets(MP4D_demux_t* mp4)
{
    uint8_t c;
    if (mp4->read_callback(mp4->read_pos, &c, 1, mp4->token)!=1)
        return -1;
    mp4->read_pos++;
    return c;
}

/**
*   Read given number of bytes from input stream
*   Used to read box headers
*/
static unsigned minimp4_read(MP4D_demux_t* mp4, int nb, int* eof_flag)
{
    uint32_t v = 0; int last_byte;
    switch (nb)
    {
    case 4: v = (v << 8) | minimp4_fgets(mp4);
    case 3: v = (v << 8) | minimp4_fgets(mp4);
    case 2: v = (v << 8) | minimp4_fgets(mp4);
    default:
    case 1: v = (v << 8) | (last_byte = minimp4_fgets(mp4));
    }
    if (last_byte < 0)
    {
        *eof_flag = 1;
    }
    return v;
}

/**
*   Read given number of bytes, but no more than *payload_bytes specifies...
*   Used to read box payload
*/
static uint32_t read_payload(MP4D_demux_t* mp4, unsigned nb, boxsize_t* payload_bytes, int* eof_flag)
{
    if (*payload_bytes < nb)
    {
        *eof_flag = 1;
        nb = (int)*payload_bytes;
    }
    *payload_bytes -= nb;

    return minimp4_read(mp4, nb, eof_flag);
}

/**
*   Skips given number of bytes.
*   Avoid math operations with fpos_t
*/
static void my_fseek(MP4D_demux_t* mp4, boxsize_t pos, int* eof_flag)
{
    mp4->read_pos += pos;
    if (mp4->read_pos >= mp4->read_size)
        *eof_flag = 1;
}

#define READ(n) read_payload(mp4, n, &payload_bytes, &eof_flag)
#define SKIP(n) { boxsize_t t = MINIMP4_MIN(payload_bytes, n); my_fseek(mp4, t, &eof_flag); payload_bytes -= t; }
#define MALLOC(t, p, size) p = (t)malloc(size); if (!(p)) { ERROR("out of memory"); }

/*
*   On error: release resources.
*/
#define RETURN_ERROR(mess) {        \
    TRACE("\nMP4 ERROR: " mess);  \
    MP4D_close(mp4);                \
    return 0;                       \
}

/*
*   Any errors, occurred on top-level hierarchy is passed to exit check: 'if (!mp4->track_count) ... '
*/
#define ERROR(mess)  \
    if (!depth)      \
        break;       \
    else             \
        RETURN_ERROR(mess);

typedef enum { BOX_ATOM, BOX_OD } boxtype_t;

//jg ++
static int MP4D_get_entry_size(MP4D_demux_t* mp4, MP4D_track_t* tr, unsigned ns) {
    int num = ns >> 10;
    int idx = ns & ((1 << 10) - 1);
    if (num != tr->entry_idx) {
        //read stsz
        //unsigned box_name = tr->entry_box_name;
        unsigned sample_size = tr->entry_sample_size;
        //
        int max = tr->entry_max;
        int rpos = tr->entry_pos + num * max * 4;
        int rsize = max * sample_size / 8;
        unsigned* buff = tr->entry_size;
        returnIfErrC(0, mp4->read_callback(rpos, tr->entry_size, rsize, mp4->token)<=0);
        //
        switch (sample_size){
        case 32:
            for (int i = max - 1; i >= 0; i--) {
                buff[i] = Swap32(buff[i]);
            }
            break;
        case 16:
            for (int i = max - 1; i >= 0; i--) {
                buff[i] = Swap16(((int16_t*)buff)[i]);
            }
            break;
        case  8:
            for (int i = max - 1; i >= 0; i--) {
                buff[i] = ((int8_t*)buff)[i];
            }
            break;
        case  4:
            for (int i = max - 1; i >= 0; i-=2) {
                int8_t v = ((int8_t*)buff)[i];
                buff[i-1] = v >> 4;
                buff[i] = v & 0xf;
            }           
            break;
        }   
        tr->entry_idx = num;
    }
    return tr->entry_size[idx];
}
static MP4D_file_offset_t MP4D_get_chunk_offset(MP4D_demux_t* mp4, MP4D_track_t* tr, int nchunk) {
    if (tr->chunk_offset_div == -1) {
        tr->chunk_offset_div = nchunk / CHUNK_OFFSET_COUNT;
    }
    uint32_t data = 0;
    int div = nchunk / CHUNK_OFFSET_COUNT;
    int count = tr->chunk_count <= CHUNK_OFFSET_COUNT * (div + 1) ? tr->chunk_count : CHUNK_OFFSET_COUNT * (div + 1);
    if (div == tr->chunk_offset_div) {
        if (tr->chunk_offset[nchunk - div * CHUNK_OFFSET_COUNT] ) {
            return tr->chunk_offset[nchunk - div * CHUNK_OFFSET_COUNT];
        }
        else {
            for (int i = div * CHUNK_OFFSET_COUNT; i < count; i++)
            {
                returnIfErrC(0, mp4->read_callback(tr->chunk_offset_begin + i * sizeof(MP4D_file_offset_t), &data, sizeof(MP4D_file_offset_t), mp4->token) <= 0);
                tr->chunk_offset[i - div * CHUNK_OFFSET_COUNT] = Swap32(data);
            }
            if (tr->chunk_offset[nchunk - div * CHUNK_OFFSET_COUNT]) {
                return tr->chunk_offset[nchunk - div * CHUNK_OFFSET_COUNT];
            }
            else {
                TRACE("\nERROR: chunk offset!");
                return 0;
            }
        }
    }
    else {
        for (int i = div * CHUNK_OFFSET_COUNT; i < count; i++)
        {
            returnIfErrC(0, mp4->read_callback(tr->chunk_offset_begin + i * sizeof(MP4D_file_offset_t), &data, sizeof(MP4D_file_offset_t), mp4->token) <= 0);
            tr->chunk_offset[i - div * CHUNK_OFFSET_COUNT] = Swap32(data);
        }
        tr->chunk_offset_div = div;
        if (tr->chunk_offset[nchunk - div * CHUNK_OFFSET_COUNT]) {
            return tr->chunk_offset[nchunk - div * CHUNK_OFFSET_COUNT];
        }
        else {
            TRACE("\nERROR: chunk offset!");
            return 0;
        }
    }
}
int MP4D_open(MP4D_demux_t* mp4, int (*read_callback)(int64_t offset, void* buffer, size_t size, void* token), void* token, int64_t file_size)
{
    // box stack size
    int depth = 0;

    struct
    {
        // remaining bytes for box in the stack
        boxsize_t bytes;

        // kind of box children's: OD chunks handled in the same manner as name chunks
        boxtype_t format;

    } stack[MAX_CHUNKS_DEPTH];

#if MP4D_TRACE_SUPPORTED
    // path of current element: List0/List1/... etc
    uint32_t box_path[MAX_CHUNKS_DEPTH];
#endif

    int eof_flag = 0;
    unsigned i;
    MP4D_track_t* tr = NULL;
    if (!mp4 || !read_callback)
    {
        TRACE("\nERROR: invlaid arguments!");
        return 0;
    }

    memset(mp4, 0, sizeof(MP4D_demux_t));
    mp4->read_callback = read_callback;
    mp4->token = token;
    mp4->read_size = file_size;

    stack[0].format = BOX_ATOM;   // start with atom box
    stack[0].bytes = 0;           // never accessed

    do
    {
        // List of boxes, derived from 'FullBox'
        //                ~~~~~~~~~~~~~~~~~~~~~
        // need read version field and check version for these boxes
        static const struct
        {
            uint32_t name;
            unsigned max_version;
            unsigned use_track_flag;
        } g_fullbox[] =
        {
#if MP4D_INFO_SUPPORTED
            {BOX_mdhd, 1, 1},
            {BOX_mvhd, 1, 0},
            {BOX_hdlr, 0, 0},
            {BOX_meta, 0, 0},   // Android can produce meta box without 'FullBox' field, comment this line to simulate the bug
#endif
#if MP4D_TRACE_TIMESTAMPS
            {BOX_stts, 0, 0},
            {BOX_ctts, 0, 0},
#endif
            {BOX_stz2, 0, 1},
            {BOX_stsz, 0, 1},
            {BOX_stsc, 0, 1},
            {BOX_stco, 0, 1},
            {BOX_co64, 0, 1},
            {BOX_stsd, 0, 0},
            {BOX_esds, 0, 1},    // esds does not use track, but switches to OD mode. Check here, to avoid OD check

            {BOX_mehd, 0, 0},
            {BOX_trex, 0, 0},
        };

        // List of boxes, which contains other boxes ('envelopes')
        // Parser will descend down for boxes in this list, otherwise parsing will proceed to
        // the next sibling box
        // OD boxes handled in the same way as atom boxes...
        static const struct
        {
            uint32_t name;
            boxtype_t type;
        } g_envelope_box[] =
        {
            {BOX_esds, BOX_OD},     // TODO: BOX_esds can be used for both audio and video, but this code supports audio only!
            {OD_ESD,   BOX_OD},
            {OD_DCD,   BOX_OD},
            {OD_DSI,   BOX_OD},
            {BOX_trak, BOX_ATOM},
            {BOX_moov, BOX_ATOM},
            //{BOX_moof, BOX_ATOM},
            {BOX_mdia, BOX_ATOM},
            {BOX_tref, BOX_ATOM},
            {BOX_minf, BOX_ATOM},
            {BOX_dinf, BOX_ATOM},
            {BOX_stbl, BOX_ATOM},
            {BOX_stsd, BOX_ATOM},
            {BOX_mp4a, BOX_ATOM},
            {BOX_mp4s, BOX_ATOM},
#if MP4D_AVC_SUPPORTED
            {BOX_mp4v, BOX_ATOM},
            {BOX_avc1, BOX_ATOM},
            //{BOX_avc2, BOX_ATOM},
            //{BOX_svc1, BOX_ATOM},
#endif
#if MP4D_HEVC_SUPPORTED
            {BOX_hvc1, BOX_ATOM},
            {BOX_hev1, BOX_ATOM},
#endif
            {BOX_udta, BOX_ATOM},
            {BOX_meta, BOX_ATOM},
            {BOX_ilst, BOX_ATOM},
            {BOX_extr, BOX_ATOM},

            {BOX_mvex, BOX_ATOM},
            {BOX_moof, BOX_ATOM},
        };

        uint32_t FullAtomVersionAndFlags = 0;
        boxsize_t payload_bytes;
        boxsize_t box_bytes;
        uint32_t box_name;
#if MP4D_INFO_SUPPORTED
        unsigned char** ptag = NULL;
        unsigned int* ptagSize = NULL;
#endif
        int read_bytes = 0;

        // Read header box type and it's length
        if (stack[depth].format == BOX_ATOM)
        {
            box_bytes = minimp4_read(mp4, 4, &eof_flag);
#if FIX_BAD_ANDROID_META_BOX
            broken_android_meta_hack :
#endif
            if (eof_flag)
                break;  // normal exit

            if (box_bytes >= 2 && box_bytes < 8)
            {
                ERROR("invalid box size (broken file?)");
            }

            box_name = minimp4_read(mp4, 4, &eof_flag);
            read_bytes = 8;

            // Decode box size
            if (box_bytes == 0 ||                         // standard indication of 'till eof' size
                box_bytes == (boxsize_t)0xFFFFFFFFU       // some files uses non-standard 'till eof' signaling
                )
            {
                box_bytes = ~(boxsize_t)0;
            }

            payload_bytes = box_bytes - 8;

            if (box_bytes == 1)           // 64-bit sizes
            {
                TRACE("\n64-bit chunk encountered");

                box_bytes = minimp4_read(mp4, 4, &eof_flag);
#if MP4D_64BIT_SUPPORTED
                box_bytes <<= 32;
                box_bytes |= minimp4_read(mp4, 4, &eof_flag);
#else
                if (box_bytes)
                {
                    ERROR("UNSUPPORTED FEATURE: MP4BoxHeader(): 64-bit boxes not supported!");
                }
                box_bytes = minimp4_read(mp4, 4, &eof_flag);
#endif
                if (box_bytes < 16)
                {
                    ERROR("invalid box size (broken file?)");
                }
                payload_bytes = box_bytes - 16;
            }

            // Read and check box version for some boxes
            for (i = 0; i < NELEM(g_fullbox); i++)
            {
                if (box_name == g_fullbox[i].name)
                {
                    FullAtomVersionAndFlags = READ(4);
                    read_bytes += 4;

#if FIX_BAD_ANDROID_META_BOX
                    // Fix invalid BOX_meta, found in some Android-produced MP4
                    // This branch is optional: bad box would be skipped
                    if (box_name == BOX_meta)
                    {
                        if (FullAtomVersionAndFlags >= 8 && FullAtomVersionAndFlags < payload_bytes)
                        {
                            if (box_bytes > stack[depth].bytes)
                            {
                                ERROR("broken file structure!");
                            }
                            stack[depth].bytes -= box_bytes;;
                            depth++;
                            stack[depth].bytes = payload_bytes + 4; // +4 need for missing header
                            stack[depth].format = BOX_ATOM;
                            box_bytes = FullAtomVersionAndFlags;
                            TRACE("Bad metadata box detected (Android bug?)!\n");
                            goto broken_android_meta_hack;
                        }
                    }
#endif // FIX_BAD_ANDROID_META_BOX

                    if ((FullAtomVersionAndFlags >> 24) > g_fullbox[i].max_version)
                    {
                        ERROR("unsupported box version!");
                    }
                    if (g_fullbox[i].use_track_flag && !tr)
                    {
                        ERROR("broken file structure!");
                    }
                }
            }
        }
        else // stack[depth].format == BOX_OD
        {
            int val;
            box_name = OD_BASE + minimp4_read(mp4, 1, &eof_flag);     // 1-byte box type
            read_bytes += 1;
            if (eof_flag)
                break;

            payload_bytes = 0;
            box_bytes = 1;
            do
            {
                val = minimp4_read(mp4, 1, &eof_flag);
                read_bytes += 1;
                if (eof_flag)
                {
                    ERROR("premature EOF!");
                }
                payload_bytes = (payload_bytes << 7) | (val & 0x7F);
                box_bytes++;
            } while (val & 0x80);
            box_bytes += payload_bytes;
        }

#if MP4D_TRACE_SUPPORTED
        box_path[depth] = (box_name >> 24) | (box_name << 24) | ((box_name >> 8) & 0x0000FF00) | ((box_name << 8) & 0x00FF0000);
        TRACE("%2d %8d %2d %.*s  (%d bytes remains for sibilings) \n", depth, (int)box_bytes, depth * 4, (char*)box_path, (int)stack[depth].bytes);
#endif

        // Check that box size <= parent size
        if (depth)
        {
            // Skip box with bad size
            assert(box_bytes > 0);
            if (box_bytes > stack[depth].bytes)
            {
                TRACE("Wrong %c%c%c%c box size: broken file?\n", (box_name >> 24) & 255, (box_name >> 16) & 255, (box_name >> 8) & 255, box_name & 255);
                box_bytes = stack[depth].bytes;
                box_name = 0;
                payload_bytes = box_bytes - read_bytes;
            }
            stack[depth].bytes -= box_bytes;
        }

        //dbgTestPXL("[%c%c%c%c]  %d\n", box_name >> 24, box_name >> 16, box_name >> 8, box_name, (int)payload_bytes);

        // Read box header
        switch (box_name)
        {
        case BOX_stz2:  //ISO/IEC 14496-1 Page 38. Section 8.17.2 - Sample Size Box.
        case BOX_stsz:
        {
            //int size = 0;
            uint32_t sample_size = READ(4);
            tr->sample_count = READ(4);
            //jg++
            tr->entry_max = 1024;
            if (tr->entry_max > tr->sample_count)
                tr->entry_max = tr->sample_count;
            tr->entry_box_name = box_name;            
            if (box_name == BOX_stsz) {
                tr->entry_sample_size = 32;
            }
            else {
                tr->entry_sample_size = sample_size & 0xff;
            }
            tr->entry_pos = mp4->read_pos;
            tr->entry_cnt = tr->sample_count;
            //tr->entry_idx0 = 0;
            tr->entry_idx = -1;
            //
            tr->entry_last_chunk = -1;
            tr->entry_last_ns = -1;
            tr->entry_last_offset = -1;
            MALLOC(unsigned int*, tr->entry_size, tr->entry_max * 4);
            MP4D_get_entry_size(mp4, tr, 0);
            //jg--
#if 0
            MALLOC(unsigned int*, tr->entry_size, tr->sample_count * 4);
            for (i = 0; i < tr->sample_count; i++)
            {
                if (box_name == BOX_stsz)
                {
                    tr->entry_size[i] = (sample_size ? sample_size : READ(4));
                }
                else
                {
                    switch (sample_size & 0xFF)
                    {
                    case 16:
                        tr->entry_size[i] = READ(2);
                        break;
                    case  8:
                        tr->entry_size[i] = READ(1);
                        break;
                    case  4:
                        if (i & 1)
                        {
                            tr->entry_size[i] = size & 15;
                        }
                        else
                        {
                            size = READ(1);
                            tr->entry_size[i] = (size >> 4);
                        }
                        break;
                    }
                }
            }
#endif
        }
        break;
        case BOX_stss:
        {
            SKIP(4)
            tr->sample_sync_count = READ(4);
            MALLOC(unsigned*, tr->sample_sync, tr->sample_sync_count * sizeof(tr->sample_sync[0]));
            for (i = 0; i < tr->sample_sync_count; i++) 
            {
                tr->sample_sync[i] = READ(4);
            }
        }
        break;
        case BOX_stsc:  //ISO/IEC 14496-12 Page 38. Section 8.18 - Sample To Chunk Box.
            tr->sample_to_chunk_count = READ(4);
            MALLOC(MP4D_sample_to_chunk_t*, tr->sample_to_chunk, tr->sample_to_chunk_count * sizeof(tr->sample_to_chunk[0]));
            for (i = 0; i < tr->sample_to_chunk_count; i++)
            {
                tr->sample_to_chunk[i].first_chunk = READ(4);
                tr->sample_to_chunk[i].samples_per_chunk = READ(4);
                SKIP(4);    // sample_description_index
            }
            break;
#if MP4D_TRACE_TIMESTAMPS || MP4D_TIMESTAMPS_SUPPORTED
        case BOX_stts:
        {
            SKIP(4);
            unsigned count = READ(4);
            unsigned j, k = 0, ts = 0, ts_count = 0;//count;
#if MP4D_TIMESTAMPS_SUPPORTED
            //MALLOC(unsigned int*, tr->timestamp, ts_count * 4);
            //MALLOC(unsigned int*, tr->duration, ts_count * 4);
#endif
            for (i = 0; i < count; i++)
            {
                unsigned sc = READ(4);
                int d = READ(4);
                TRACE("sample %8d count %8d duration %8d\n", i, sc, d);
#if MP4D_TIMESTAMPS_SUPPORTED
                if (k + sc > ts_count)
                {
                    ts_count = k + (sc > 1023 ? sc : 1023);
                    tr->timestamp = (unsigned int*)realloc(tr->timestamp, (ts_count + 1) * sizeof(unsigned));
                    //tr->duration = (unsigned int*)realloc(tr->duration, ts_count * sizeof(unsigned));
                }
                if (tr->timestamp) {
                    for (j = 0; j < sc; j++)
                    {
                        //tr->duration[k] = d;
                        //tr->timestamp[k++] = ts;
                        tr->timestamp[k++] = ts;
                        ts += d;
                    }
                }
#endif
            }
#if MP4D_TIMESTAMPS_SUPPORTED
            if (tr->timestamp) {
                tr->timestamp[k++] = ts;
            }
#endif
        }
        break;
        case BOX_ctts:
        {
            unsigned count = READ(4);
            for (i = 0; i < count; i++)
            {
                int sc = READ(4);
                int d = READ(4);
                (void)sc;
                (void)d;
                TRACE("sample %8d count %8d decoding to composition offset %8d\n", i, sc, d);
            }
        }
        break;
#endif
        case BOX_stco:  //ISO/IEC 14496-12 Page 39. Section 8.19 - Chunk Offset Box.
        case BOX_co64:
            tr->chunk_count = READ(4);
            tr->chunk_offset_begin = mp4->read_pos;

            if (tr->chunk_count > CHUNK_OFFSET_COUNT) {
                //mp4->read_pos += (int64_t)tr->chunk_count * 4;
                auto skipbyte = (int64_t)tr->chunk_count * 4;
                SKIP(skipbyte);
                MALLOC(MP4D_file_offset_t*, tr->chunk_offset, CHUNK_OFFSET_COUNT * sizeof(MP4D_file_offset_t));
                memset(tr->chunk_offset, 0, CHUNK_OFFSET_COUNT * sizeof(MP4D_file_offset_t));
            }
            else {
                MALLOC(MP4D_file_offset_t*, tr->chunk_offset, tr->chunk_count * sizeof(MP4D_file_offset_t));
                for (i = 0; i < tr->chunk_count; i++)
                {
                    tr->chunk_offset[i] = READ(4);
                    if (box_name == BOX_co64)
                    {
#if !MP4D_64BIT_SUPPORTED
                        if (tr->chunk_offset[i])
                        {
                            ERROR("UNSUPPORTED FEATURE: 64-bit chunk_offset not supported!");
                        }
#endif
                        // tr->chunk_offset[i] <<= 32;
                        tr->chunk_offset[i] |= READ(4);
                    }
                }
            }
            break;

#if MP4D_INFO_SUPPORTED
        case BOX_mvhd:
            SKIP(((FullAtomVersionAndFlags >> 24) == 1) ? 8 + 8 : 4 + 4);
            mp4->timescale = READ(4);
            mp4->duration_hi = ((FullAtomVersionAndFlags >> 24) == 1) ? READ(4) : 0;
            mp4->duration_lo = READ(4);
            SKIP(4 + 2 + 2 + 4 * 2 + 4 * 9 + 4 * 6 + 4);
            break;

        case BOX_mdhd:
            SKIP(((FullAtomVersionAndFlags >> 24) == 1) ? 8 + 8 : 4 + 4);
            tr->timescale = READ(4);
            tr->duration_hi = ((FullAtomVersionAndFlags >> 24) == 1) ? READ(4) : 0;
            tr->duration_lo = READ(4);

            {
                int ISO_639_2_T = READ(2);
                tr->language[2] = (ISO_639_2_T & 31) + 0x60; ISO_639_2_T >>= 5;
                tr->language[1] = (ISO_639_2_T & 31) + 0x60; ISO_639_2_T >>= 5;
                tr->language[0] = (ISO_639_2_T & 31) + 0x60;
            }
            // the rest of this box is skipped by default ...
            break;

        case BOX_hdlr:
            if (tr) // When this box is within 'meta' box, the track may not be avaialable
            {
                SKIP(4); // pre_defined
                tr->handler_type = READ(4);
            }
            // typically hdlr box does not contain any useful info.
            // the rest of this box is skipped by default ...
            break;

        case BOX_btrt:
            if (!tr)
            {
                ERROR("broken file structure!");
            }

            SKIP(4 + 4);
            tr->avg_bitrate_bps = READ(4);
            break;

            // Set pointer to tag to be read...
        case BOX_calb: ptag = &mp4->tag.album; ptagSize = &mp4->tag.album_size;  break;
        case BOX_cART: ptag = &mp4->tag.artist; ptagSize = &mp4->tag.artist_size; break;
        case BOX_cnam: ptag = &mp4->tag.title; ptagSize = &mp4->tag.title_size; break;
        case BOX_cday: ptag = &mp4->tag.year;    break;
        case BOX_ccmt: ptag = &mp4->tag.comment; break;
        case BOX_cgen: ptag = &mp4->tag.genre;   break;

#endif

        case BOX_stsd:
            SKIP(4); // entry_count, BOX_mp4a & BOX_mp4v boxes follows immediately
            break;
        case BOX_alac:
            SKIP(28);
            tr->dsi_bytes = payload_bytes;
            MALLOC(unsigned char*, tr->dsi, tr->dsi_bytes);
            if (mp4->read_callback(mp4->read_pos, tr->dsi, tr->dsi_bytes, mp4->token)!= tr->dsi_bytes) {
                ERROR("broken alac file structure!");
            }
            mp4->read_pos+= tr->dsi_bytes;
            payload_bytes = 0;
            {
                alac_extradata_t* alac = (alac_extradata_t*)tr->dsi;
                tr->SampleDescription.audio.samplerate_hz = Swap32(alac->samplerate);
                tr->SampleDescription.audio.channelcount = alac->channels;
                tr->avg_bitrate_bps = alac->bitrate;
                tr->stream_type = 5;//case 0x05: return "AudioStream";
                tr->object_type_indication = Swap32(BOX_alac);
            }
            break;
        case BOX_mp4s:  // private stream
            if (!tr)
            {
                ERROR("broken file structure!");
            }
            SKIP(6 * 1 + 2/*Base SampleEntry*/);
            tr->stream_type = 0xC0;
            tr->object_type_indication = MP4_OBJECT_TYPE_USER_PRIVATE;
            break;

        case BOX_mp4a:
            if (!tr)
            {
                ERROR("broken file structure!");
            }
#if MP4D_INFO_SUPPORTED
            SKIP(6 * 1 + 2/*Base SampleEntry*/ + 4 * 2);
            tr->SampleDescription.audio.channelcount = READ(2);
            SKIP(2/*samplesize*/ + 2 + 2);
            tr->SampleDescription.audio.samplerate_hz = READ(4) >> 16;
#else
            SKIP(28);
#endif
            break;

#if MP4D_AVC_SUPPORTED
        case BOX_avc1:  // AVCSampleEntry extends VisualSampleEntry
        case BOX_hev1:
        case BOX_hvc1:
//         case BOX_avc2:   - no test
//         case BOX_svc1:   - no test
        case BOX_mp4v:
            if (!tr)
            {
                ERROR("broken file structure!");
            }
#if MP4D_INFO_SUPPORTED
            SKIP(6 * 1 + 2/*Base SampleEntry*/ + 2 + 2 + 4 * 3);
            tr->SampleDescription.video.width = READ(2);
            tr->SampleDescription.video.height = READ(2);
            // frame_count is always 1
            // compressorname is rarely set..
            SKIP(4 + 4 + 4 + 2/*frame_count*/ + 32/*compressorname*/ + 2 + 2);
#else
            SKIP(78);
#endif
            // ^^^ end of VisualSampleEntry
            // now follows for BOX_avc1:
            //      BOX_avcC
            //      BOX_btrt (optional)
            //      BOX_m4ds (optional)
            // for BOX_mp4v:
            //      BOX_esds
            break;

        case BOX_avcC:  // AVCDecoderConfigurationRecord()
            // hack: AAC-specific DSI field reused (for it have same purpoose as sps/pps)
            // TODO: check this hack if BOX_esds co-exist with BOX_avcC
            tr->stream_type = 0x04;
            tr->object_type_indication = MP4_OBJECT_TYPE_AVC;
            tr->dsi = (unsigned char*)malloc((size_t)box_bytes);
            tr->dsi_bytes = (unsigned)box_bytes;
            {
                int spspps;
                unsigned char* p = tr->dsi;
                unsigned int configurationVersion = READ(1);
                unsigned int AVCProfileIndication = READ(1);
                unsigned int profile_compatibility = READ(1);
                unsigned int AVCLevelIndication = READ(1);
                //bit(6) reserved =
                unsigned int lengthSizeMinusOne = READ(1) & 3;

                (void)configurationVersion;
                (void)AVCProfileIndication;
                (void)profile_compatibility;
                (void)AVCLevelIndication;
                (void)lengthSizeMinusOne;

                for (spspps = 0; spspps < 2; spspps++)
                {
                    unsigned int numOfSequenceParameterSets = READ(1);
                    if (!spspps)
                    {
                        numOfSequenceParameterSets &= 31;  // clears 3 msb for SPS
                    }
                    //*p++ = numOfSequenceParameterSets;
                    *p++ = 0;
                    *p++ = 0;
                    for (i = 0; i < numOfSequenceParameterSets; i++)
                    {
                        unsigned k, sequenceParameterSetLength = READ(2);
                        *p++ = sequenceParameterSetLength >> 8;
                        *p++ = sequenceParameterSetLength;
                        for (k = 0; k < sequenceParameterSetLength; k++)
                        {
                            *p++ = READ(1);
                        }
                    }
                }
                tr->dsi_bytes = p - (unsigned char*)tr->dsi;
            }
            break;
            case BOX_hvcC:
            tr->stream_type = 0x04;
            tr->object_type_indication = MP4_OBJECT_TYPE_HEVC;
            SKIP(23);
            tr->dsi = (unsigned char*)malloc((size_t)box_bytes);
            tr->dsi_bytes = (unsigned)box_bytes;
            {
                int vpsspspps;
                unsigned char* p = tr->dsi;

                for (vpsspspps = 0; vpsspspps < 3; vpsspspps++)
                {
                    unsigned int arrayCompleteness = READ(1);
                    unsigned int nalUnitType = READ(2);

                    (void)arrayCompleteness;
                    (void)nalUnitType;

                    unsigned int numOfSequenceParameterSets = READ(2);
                    *p++ = 0;
                    *p++ = 0;
                    *p++ = numOfSequenceParameterSets >> 8;
                    *p++ = numOfSequenceParameterSets;
                    for (i = 0; i < numOfSequenceParameterSets; i++)
                    {
                        *p++ = READ(1);
                    }
                }
                tr->dsi_bytes = p - (unsigned char*)tr->dsi;
            }
            break;
#endif  // MP4D_AVC_SUPPORTED
        case BOX_extr: 
            if (tr->dsi) {
                SKIP(payload_bytes);
                break;
            }
            tr->dsi = (unsigned char*)malloc((size_t)box_bytes);
            returnIfErrC(false, !tr->dsi);
            tr->dsi_bytes = (unsigned)box_bytes - 8;
            for (int k = 0; k < box_bytes - 8; k++)
            {
                tr->dsi[k] = READ(1);
            }
            break;
        case OD_ESD:
        {
            unsigned flags = READ(3);   // ES_ID(2) + flags(1)

            if (flags & 0x80)       // steamdependflag
            {
                SKIP(2);            // dependsOnESID
            }
            if (flags & 0x40)       // urlflag
            {
                unsigned bytecount = READ(1);
                SKIP(bytecount);    // skip URL
            }
            if (flags & 0x20)       // ocrflag (was reserved in MPEG-4 v.1)
            {
                SKIP(2);            // OCRESID
            }
            break;
        }

        case OD_DCD:        //ISO/IEC 14496-1 Page 28. Section 8.6.5 - DecoderConfigDescriptor.
            assert(tr);     // ensured by g_fullbox[] check
            tr->object_type_indication = READ(1);
#if MP4D_INFO_SUPPORTED
            tr->stream_type = READ(1) >> 2;
            SKIP(3/*bufferSizeDB*/ + 4/*maxBitrate*/);
            tr->avg_bitrate_bps = READ(4);
#else
            SKIP(1 + 3 + 4 + 4);
#endif
            break;

        case OD_DSI:        //ISO/IEC 14496-1 Page 28. Section 8.6.5 - DecoderConfigDescriptor.
            assert(tr);     // ensured by g_fullbox[] check
            if (!tr->dsi && payload_bytes)
            {
                MALLOC(unsigned char*, tr->dsi, (int)payload_bytes);
                for (i = 0; i < payload_bytes; i++)
                {
                    tr->dsi[i] = minimp4_read(mp4, 1, &eof_flag);    // These bytes available due to check above
                }
                tr->dsi_bytes = i;
                payload_bytes -= i;
                break;
            }

        //fragment mp4
        case BOX_mvex:
            break;
        case BOX_mehd:
            mp4->duration_hi = 0;
            mp4->duration_lo = READ(4);//duration            
            break;
        case BOX_trex: 
            READ(4);        // track_ID
            READ(4);        // default_sample_description_index
            READ(4);        // default_sample_duration
            READ(4);        // default_sample_size
            READ(4);        // default_sample_flags
            break;
        case BOX_moof:
            mp4->moof_start_pos = mp4->read_pos - 8;
            eof_flag = 1;
            break;
        default:
            TRACE("[%c%c%c%c]  %d\n", box_name >> 24, box_name >> 16, box_name >> 8, box_name, (int)payload_bytes);
        }

#if MP4D_INFO_SUPPORTED
        // Read tag is tag pointer is set
        if (ptag && !*ptag && payload_bytes > 16)
        {
#if 0
            uint32_t size = READ(4);
            uint32_t data = READ(4);
            uint32_t clas = READ(4);
            uint32_t x1 = READ(4);
            // TRACE("%2d  %2d %2d ", size, clas, x1);
#else
            SKIP(4 + 4 + 4 + 4);
#endif
            MALLOC(unsigned char*, *ptag, (unsigned)payload_bytes + 1);
            for (i = 0; payload_bytes != 0; i++)
            {
                (*ptag)[i] = READ(1);
            }
            (*ptag)[i++] = 0; // zero-terminated string
            if(ptagSize) {*ptagSize = i;}
        }
#endif

        if (box_name == BOX_trak)
        {
            // New track found: allocate memory using realloc()
            // Typically there are 1 audio track for AAC audio file,
            // 4 tracks for movie file,
            // 3-5 tracks for scalable audio (CELP+AAC)
            // and up to 50 tracks for BSAC scalable audio
            void* mem = realloc(mp4->track, (mp4->track_count + 1) * sizeof(MP4D_track_t));
            if (!mem)
            {
                // if realloc fails, it does not deallocate old pointer!
                ERROR("out of memory");
            }
            mp4->track = (MP4D_track_t*)mem;
            tr = mp4->track + mp4->track_count++;
            memset(tr, 0, sizeof(MP4D_track_t));
            tr->chunk_offset_div = -1;
        }
        else if (box_name == BOX_meta)
        {
            tr = NULL;  // Avoid update of 'hdlr' box, which may contains in the 'meta' box
        }

        // If this box is envelope, save it's size in box stack
        for (i = 0; i < NELEM(g_envelope_box); i++)
        {
            if (box_name == g_envelope_box[i].name)
            {
                if (++depth >= MAX_CHUNKS_DEPTH)
                {
                    ERROR("too deep atoms nesting!");
                }
                stack[depth].bytes = payload_bytes;
                stack[depth].format = g_envelope_box[i].type;
                break;
            }
        }

        // if box is not envelope, just skip it
        if (i == NELEM(g_envelope_box))
        {
            if (payload_bytes > file_size)
            {
                eof_flag = 1;
            }
            else
            {
                SKIP(payload_bytes);
            }
        }

        // remove empty boxes from stack
        // don't touch box with index 0 (which indicates whole file)
        while (depth > 0 && !stack[depth].bytes)
        {
            depth--;
        }

    } while (!eof_flag);

    if (!mp4->track_count)
    {
        RETURN_ERROR("no tracks found");
    }
    return 1;
}

/**
*   Find chunk, containing given sample.
*   Returns chunk number, and first sample in this chunk.
*/
static int sample_to_chunk(MP4D_track_t* tr, unsigned nsample, unsigned* nfirst_sample_in_chunk)
{
    unsigned chunk_group = 0, nc;
    unsigned sum = 0;
    *nfirst_sample_in_chunk = 0;
    if (tr->chunk_count <= 1)
    {
        return 0;
    }
    for (nc = 0; nc < tr->chunk_count; nc++)
    {
        if (chunk_group + 1 < tr->sample_to_chunk_count     // stuck at last entry till EOF
            && nc + 1 ==    // Chunks counted starting with '1'
            tr->sample_to_chunk[chunk_group + 1].first_chunk)    // next group?
        {
            chunk_group++;
        }

        sum += tr->sample_to_chunk[chunk_group].samples_per_chunk;
        if (nsample < sum)
            return nc;

        // TODO: this can be calculated once per file
        *nfirst_sample_in_chunk = sum;
    }
    return -1;
}


// jg--
// Exported API function
MP4D_file_offset_t MP4D_frame_offset(const MP4D_demux_t* mp4, unsigned ntrack, unsigned nsample, unsigned* frame_bytes, unsigned* timestamp, unsigned* duration)
{
#if 1 //jg++
    returnIfErrC(0, !mp4);
    returnIfErrC(0, !frame_bytes);
    //returnIfErrC(0, !timestamp);
    //returnIfErrC(0, !duration);
    returnIfErrC(0, !mp4->track);
    MP4D_track_t* tr = mp4->track + ntrack;
    returnIfErrC(0, !tr);
    if (nsample == tr->entry_last_ns) {
        if (frame_bytes)*frame_bytes = tr->entry_frame_bytes;
        return tr->entry_last_offset;
    }
    //
    unsigned ns;
    int nchunk = sample_to_chunk(tr, nsample, &ns);
    MP4D_file_offset_t offset = 0;
    returnIfErrCS(0, nchunk >= tr->chunk_count, "%d>=%d", nchunk , tr->chunk_count);
    if (nchunk < 0)
    {
        *frame_bytes = tr->entry_frame_bytes = 0;
        return 0;
    }
    if (nchunk == tr->entry_last_chunk) {
        offset = tr->entry_last_offset;
        ns = tr->entry_last_ns;
    }
    else {
        if (tr->chunk_count > CHUNK_OFFSET_COUNT) {
            offset = MP4D_get_chunk_offset((MP4D_demux_t*)mp4, tr, nchunk);
            returnIfErrC(0, offset == 0);
        }
        else {
            offset = tr->chunk_offset[nchunk];
        }
        tr->entry_last_chunk = nchunk;
    }
    //
    for (; ns < nsample; ns++)
    {
        int rst = MP4D_get_entry_size((MP4D_demux_t*)mp4, tr, ns);// tr->entry_size[ns];
        returnIfErrC(0, !rst);
        offset += rst;
    }
    tr->entry_frame_bytes = MP4D_get_entry_size((MP4D_demux_t*)mp4,tr,ns); //tr->entry_size[ns];
    returnIfErrC(0, !tr->entry_frame_bytes);
    *frame_bytes = tr->entry_frame_bytes;
    //
    tr->entry_last_ns = ns;
    tr->entry_last_offset = offset;
    //
#else //jg--
    MP4D_track_t* tr = mp4->track + ntrack;
    unsigned ns;
    int nchunk = sample_to_chunk(tr, nsample, &ns);
    MP4D_file_offset_t offset;

    if (nchunk < 0)
    {
        *frame_bytes = 0;
        return 0;
    }

    offset = tr->chunk_offset[nchunk];
    for (; ns < nsample; ns++)
    {
        offset += tr->entry_size[ns];
    }

    *frame_bytes = tr->entry_size[ns];

#endif
    if (timestamp)
    {
#if MP4D_TIMESTAMPS_SUPPORTED
        * timestamp = tr->timestamp ? tr->timestamp[ns] : 0;
#else
        * timestamp = 0;
#endif
    }
    if (duration)
    {
#if MP4D_TIMESTAMPS_SUPPORTED
        * duration = tr->timestamp ? (tr->timestamp[ns + 1] - tr->timestamp[ns]) : 0;
#else
        * duration = 0;
#endif
    }

    return offset;
}

#define FREE(x) if (x) {free(x); x = NULL;}

// Exported API function
void MP4D_close(MP4D_demux_t* mp4)
{
    while (mp4->track_count)
    {
        MP4D_track_t* tr = mp4->track + --mp4->track_count;
        FREE(tr->entry_size);
#if MP4D_TIMESTAMPS_SUPPORTED
        FREE(tr->timestamp);
        //FREE(tr->duration);
#endif
        FREE(tr->sample_sync);
        FREE(tr->sample_to_chunk);
        FREE(tr->chunk_offset);
        FREE(tr->dsi);
    }
    FREE(mp4->track);
#if MP4D_INFO_SUPPORTED
    FREE(mp4->tag.title);
    FREE(mp4->tag.artist);
    FREE(mp4->tag.album);
    FREE(mp4->tag.year);
    FREE(mp4->tag.comment);
    FREE(mp4->tag.genre);
#endif
}

static int skip_spspps(const unsigned char* p, int nbytes, int nskip)
{
    int i, k = 0;
    for (i = 0; i < nskip; i++)
    {
        unsigned segmbytes;
        if (k > nbytes - 2)
            return -1;
        segmbytes = p[k] * 256 + p[k + 1];
        k += 2 + segmbytes;
    }
    return k;
}

static const void* MP4D_read_spspps(const MP4D_demux_t* mp4, unsigned int ntrack, int pps_flag, int nsps, int* sps_bytes)
{
    int sps_count, skip_bytes;
    int bytepos = 0;
    unsigned char* p = mp4->track[ntrack].dsi;
    if (ntrack >= mp4->track_count)
        return NULL;
    if (mp4->track[ntrack].object_type_indication != MP4_OBJECT_TYPE_AVC)
        return NULL;    // SPS/PPS are specific for AVC format only

    if (pps_flag)
    {
        // Skip all SPS
        sps_count = p[bytepos++];
        skip_bytes = skip_spspps(p + bytepos, mp4->track[ntrack].dsi_bytes - bytepos, sps_count);
        if (skip_bytes < 0)
            return NULL;
        bytepos += skip_bytes;
    }

    // Skip sps/pps before the given target
    sps_count = p[bytepos++];
    if (nsps >= sps_count)
        return NULL;
    skip_bytes = skip_spspps(p + bytepos, mp4->track[ntrack].dsi_bytes - bytepos, nsps);
    if (skip_bytes < 0)
        return NULL;
    bytepos += skip_bytes;
    *sps_bytes = p[bytepos] * 256 + p[bytepos + 1];
    return p + bytepos + 2;
}


const void* MP4D_read_sps(const MP4D_demux_t* mp4, unsigned int ntrack, int nsps, int* sps_bytes)
{
    return MP4D_read_spspps(mp4, ntrack, 0, nsps, sps_bytes);
}

const void* MP4D_read_pps(const MP4D_demux_t* mp4, unsigned int ntrack, int npps, int* pps_bytes)
{
    return MP4D_read_spspps(mp4, ntrack, 1, npps, pps_bytes);
}

#if MP4D_PRINT_INFO_SUPPORTED
/************************************************************************/
/*  Purely informational part, may be removed for embedded applications */
/************************************************************************/

//
// Decodes ISO/IEC 14496 MP4 stream type to ASCII string
//
static const char* GetMP4StreamTypeName(int streamType)
{
    switch (streamType)
    {
    case 0x00: return "Forbidden";
    case 0x01: return "ObjectDescriptorStream";
    case 0x02: return "ClockReferenceStream";
    case 0x03: return "SceneDescriptionStream";
    case 0x04: return "VisualStream";
    case 0x05: return "AudioStream";
    case 0x06: return "MPEG7Stream";
    case 0x07: return "IPMPStream";
    case 0x08: return "ObjectContentInfoStream";
    case 0x09: return "MPEGJStream";
    default:
        if (streamType >= 0x20 && streamType <= 0x3F)
        {
            return "User private";
        }
        else
        {
            return "Reserved for ISO use";
        }
    }
}

//
// Decodes ISO/IEC 14496 MP4 object type to ASCII string
//
static const char* GetMP4ObjectTypeName(int objectTypeIndication)
{
    switch (objectTypeIndication)
    {
    case 0x00: return "Forbidden";
    case 0x01: return "Systems ISO/IEC 14496-1";
    case 0x02: return "Systems ISO/IEC 14496-1";
    case 0x20: return "Visual ISO/IEC 14496-2";
    case 0x40: return "Audio ISO/IEC 14496-3";
    case 0x60: return "Visual ISO/IEC 13818-2 Simple Profile";
    case 0x61: return "Visual ISO/IEC 13818-2 Main Profile";
    case 0x62: return "Visual ISO/IEC 13818-2 SNR Profile";
    case 0x63: return "Visual ISO/IEC 13818-2 Spatial Profile";
    case 0x64: return "Visual ISO/IEC 13818-2 High Profile";
    case 0x65: return "Visual ISO/IEC 13818-2 422 Profile";
    case 0x66: return "Audio ISO/IEC 13818-7 Main Profile";
    case 0x67: return "Audio ISO/IEC 13818-7 LC Profile";
    case 0x68: return "Audio ISO/IEC 13818-7 SSR Profile";
    case 0x69: return "Audio ISO/IEC 13818-3";
    case 0x6A: return "Visual ISO/IEC 11172-2";
    case 0x6B: return "Audio ISO/IEC 11172-3";
    case 0x6C: return "Visual ISO/IEC 10918-1";
    case 0xFF: return "no object type specified";
    default:
        if (objectTypeIndication >= 0xC0 && objectTypeIndication <= 0xFE)
            return "User private";
        else
            return "Reserved for ISO use";
    }
}

/**
*   Print MP4 information to stdout.
*   Subject for customization to particular application

Output Example #1: movie file

MP4 FILE: 7 tracks found. Movie time 104.12 sec

No|type|lng| duration           | bitrate| Stream type            | Object type
 0|odsm|fre|   0.00 s      1 frm|       0| Forbidden              | Forbidden
 1|sdsm|fre|   0.00 s      1 frm|       0| Forbidden              | Forbidden
 2|vide|```| 104.12 s   2603 frm| 1960559| VisualStream           | Visual ISO/IEC 14496-2   -  720x304
 3|soun|ger| 104.06 s   2439 frm|  191242| AudioStream            | Audio ISO/IEC 14496-3    -  6 ch 24000 hz
 4|soun|eng| 104.06 s   2439 frm|  194171| AudioStream            | Audio ISO/IEC 14496-3    -  6 ch 24000 hz
 5|subp|ger|  71.08 s     25 frm|       0| Forbidden              | Forbidden
 6|subp|eng|  71.08 s     25 frm|       0| Forbidden              | Forbidden

Output Example #2: audio file with tags

MP4 FILE: 1 tracks found. Movie time 92.42 sec
title = 86-Second Blowout
artist = Yo La Tengo
album = May I Sing With Me
year = 1992

No|type|lng| duration           | bitrate| Stream type            | Object type
 0|mdir|und|  92.42 s   3980 frm|  128000| AudioStream            | Audio ISO/IEC 14496-3MP4 FILE: 1 tracks found. Movie time 92.42 sec

*/
void MP4D_printf_info(const MP4D_demux_t* mp4)
{
    unsigned i;
    dbgPrintf("\nMP4 FILE: %d tracks found. Movie time %.2f sec\n", mp4->track_count, (4294967296.0 * mp4->duration_hi + mp4->duration_lo) / mp4->timescale);
#define STR_TAG(name) if (mp4->tag.name)  dbgPrintf("%10s = %s\n", #name, mp4->tag.name)
    STR_TAG(title);
    STR_TAG(artist);
    STR_TAG(album);
    STR_TAG(year);
    STR_TAG(comment);
    STR_TAG(genre);
    dbgPrintf("\nNo|type|lng| duration           | bitrate| %-23s| Object type", "Stream type");
    for (i = 0; i < mp4->track_count; i++)
    {
        MP4D_track_t* tr = mp4->track + i;

        dbgPrintf("\n%2d|%c%c%c%c|%c%c%c|%7.2f s %6d frm| %7d|", i,
            (tr->handler_type >> 24), (tr->handler_type >> 16), (tr->handler_type >> 8), (tr->handler_type >> 0),
            tr->language[0], tr->language[1], tr->language[2],
            (65536.0 * 65536.0 * tr->duration_hi + tr->duration_lo) / tr->timescale,
            tr->sample_count,
            tr->avg_bitrate_bps);

        dbgPrintf(" %-23s|", GetMP4StreamTypeName(tr->stream_type));
        dbgPrintf(" %-23s", GetMP4ObjectTypeName(tr->object_type_indication));

        if (tr->handler_type == MP4D_HANDLER_TYPE_SOUN)
        {
            dbgPrintf("  -  %d ch %d hz", tr->SampleDescription.audio.channelcount, tr->SampleDescription.audio.samplerate_hz);
        }
        else if (tr->handler_type == MP4D_HANDLER_TYPE_VIDE)
        {
            dbgPrintf("  -  %dx%d", tr->SampleDescription.video.width, tr->SampleDescription.video.height);
        }
    }
    dbgPrintf("\n");
}

#endif // MP4D_PRINT_INFO_SUPPORTED

}
