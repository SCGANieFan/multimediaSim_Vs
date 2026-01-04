ogg
ogg_stream_pageout_fill
ogg_stream_flush_i



// 0-3
"OggS"

// 4, stream structure version
0x00

// 5, continued packet flag
0x00,
0x01, //continued packet flag
0x02, //first page flag
0x04, //last page flag

// 6-13, 64 bits of PCM position
granule_pos

// 14-17, 32 bits of stream serial number
serialno


// 18-21, 32 bits of page counter


// 22-25, crc


// 26, segment number

// 27-27+val-1, segment table, lacing_vals



//muxer
ogg_stream_iovecin(os, &iov, 1, op->e_o_s, op->granulepos);
os->granule_vals[os->lacing_fill+i]=os->granulepos;
os->granule_vals[os->lacing_fill+i]=granulepos;
os->granulepos=granulepos;

// demuxer
memmove(os->granule_vals,os->granule_vals+lr,(os->lacing_fill-lr)*sizeof(*os->granule_vals));
op->granulepos=os->granule_vals[ptr];




