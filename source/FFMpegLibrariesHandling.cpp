/*  This file is part of YUView - The YUV player with advanced analytics toolset
*   <https://github.com/IENT/YUView>
*   Copyright (C) 2015  Institut für Nachrichtentechnik, RWTH Aachen University, GERMANY
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 3 of the License, or
*   (at your option) any later version.
*
*   In addition, as a special exception, the copyright holders give
*   permission to link the code of portions of this program with the
*   OpenSSL library under certain conditions as described in each
*   individual source file, and distribute linked combinations including
*   the two.
*
*   You must obey the GNU General Public License in all respects for all
*   of the code used other than OpenSSL. If you modify file(s) with this
*   exception, you may extend this exception to your version of the
*   file(s), but you are not obligated to do so. If you do not wish to do
*   so, delete this exception statement from your version. If you delete
*   this exception statement from all source files in the program, then
*   also delete it here.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "FFMpegLibrariesHandling.h"

#include <QCoreApplication>
#include <QDir>
#include <QSettings>
#include "typedef.h"

#define FFmpegDecoderLibHandling_DEBUG_OUTPUT 0
#if FFmpegDecoderLibHandling_DEBUG_OUTPUT && !NDEBUG
#include <QDebug>
#define DEBUG_LIB qDebug
#else
#define DEBUG_LIB(fmt,...) ((void)0)
#endif

using namespace YUV_Internals;
using namespace RGB_Internals;

// ------------- Internal classes to parse the ffmpeg version specific pointers -----------------

// AVPacket is part of avcodec. The definition is different for different major versions of avcodec.
// These are the version independent functions to retrive data from AVPacket.
typedef struct AVPacket_56
{
  AVBufferRef *buf;
  int64_t pts;
  int64_t dts;
  uint8_t *data;
  int size;
  int stream_index;
  int flags;
  AVPacketSideData *side_data;
  int side_data_elems;
  int duration;
  void (*destruct)(struct AVPacket *);
  void *priv;
  int64_t pos;
  int64_t convergence_duration;
} AVPacket_56;

typedef struct AVPacket_57
{
  AVBufferRef *buf;
  int64_t pts;
  int64_t dts;
  uint8_t *data;
  int size;
  int stream_index;
  int flags;
  AVPacketSideData *side_data;
  int side_data_elems;
  int64_t duration;
  int64_t pos;
  int64_t convergence_duration;
} AVPacket_57;

// AVFormatContext is part of avformat.
// These functions give us version independent access to the structs.
typedef struct AVFormatContext_56 
{
  const AVClass *av_class;
  struct AVInputFormat *iformat;
  struct AVOutputFormat *oformat;
  void *priv_data;
  AVIOContext *pb;
  int ctx_flags;
  unsigned int nb_streams; //
  AVStream **streams; //
  char filename[1024];
  int64_t start_time;
  int64_t duration; //
  int bit_rate;
  unsigned int packet_size;
  int max_delay;
  int flags;
  unsigned int probesize;
  int max_analyze_duration;
  const uint8_t *key;
  int keylen;
  unsigned int nb_programs;
  AVProgram **programs;
  enum AVCodecID video_codec_id;
  enum AVCodecID audio_codec_id;
  enum AVCodecID subtitle_codec_id;
  unsigned int max_index_size;
  unsigned int max_picture_buffer;
  unsigned int nb_chapters;
  AVChapter **chapters;
  AVDictionary *metadata;

  // Actually, there is more here, but the variables above are the only we need.
} AVFormatContext_56;

typedef struct AVFormatContext_57
{
  const AVClass *av_class;
  struct AVInputFormat *iformat;
  struct AVOutputFormat *oformat;
  void *priv_data;
  AVIOContext *pb;
  int ctx_flags;
  unsigned int nb_streams;
  AVStream **streams;
  char filename[1024];
  int64_t start_time;
  int64_t duration;
  int64_t bit_rate;
  unsigned int packet_size;
  int max_delay;
  int flags;
  unsigned int probesize;
  int max_analyze_duration;
  const uint8_t *key;
  int keylen;
  unsigned int nb_programs;
  AVProgram **programs;
  enum AVCodecID video_codec_id;
  enum AVCodecID audio_codec_id;
  enum AVCodecID subtitle_codec_id;
  unsigned int max_index_size;
  unsigned int max_picture_buffer;
  unsigned int nb_chapters;
  AVChapter **chapters;
  AVDictionary *metadata;
  
  // Actually, there is more here, but the variables above are the only we need.
} AVFormatContext_57;

// AVCodec is (of course) part of avcodec
typedef struct AVCodec_56_57
{
  const char *name;
  const char *long_name;
  enum AVMediaType type;
  enum AVCodecID id;
  int capabilities;
  const AVRational *supported_framerates; ///< array of supported framerates, or NULL if any, array is terminated by {0,0}
  const enum AVPixelFormat *pix_fmts;     ///< array of supported pixel formats, or NULL if unknown, array is terminated by -1
  const int *supported_samplerates;       ///< array of supported audio samplerates, or NULL if unknown, array is terminated by 0
  const enum AVSampleFormat *sample_fmts; ///< array of supported sample formats, or NULL if unknown, array is terminated by -1
  const uint64_t *channel_layouts;         ///< array of support channel layouts, or NULL if unknown. array is terminated by 0
  uint8_t max_lowres;                     ///< maximum value for lowres supported by the decoder
  const AVClass *priv_class;              ///< AVClass for the private context
  //const AVProfile *profiles;              ///< array of recognized profiles, or NULL if unknown, array is terminated by {FF_PROFILE_UNKNOWN}
  
  // Actually, there is more here, but nothing more of the public API
} AVCodec_56_57;

// AVCodecContext is part of avcodec
typedef struct AVCodecContext_56 
{
  const AVClass *av_class;
  int log_level_offset;

  AVMediaType codec_type; //
  const struct AVCodec *codec;
  char codec_name[32];
  AVCodecID codec_id; // 
  unsigned int codec_tag;
  unsigned int stream_codec_tag;
  void *priv_data;
  struct AVCodecInternal *internal;
  void *opaque;
  int bit_rate; //
  int bit_rate_tolerance;
  int global_quality;
  int compression_level;
  int flags;
  int flags2;
  uint8_t *extradata; //
  int extradata_size; //
  AVRational time_base;
  int ticks_per_frame;
  int delay;
  int width, height;  //
  int coded_width, coded_height;
  int gop_size;
  AVPixelFormat pix_fmt; //
  int me_method;
  void (*draw_horiz_band)(struct AVCodecContext *s, const AVFrame *src, int offset[AV_NUM_DATA_POINTERS], int y, int type, int height);
  AVPixelFormat (*get_format)(struct AVCodecContext *s, const enum AVPixelFormat * fmt);
  int max_b_frames;
  float b_quant_factor;
  int rc_strategy;
  int b_frame_strategy;
  float b_quant_offset;
  int has_b_frames; //
  int mpeg_quant;
  float i_quant_factor;
  float i_quant_offset;
  float lumi_masking;
  float temporal_cplx_masking;
  float spatial_cplx_masking;
  float p_masking;
  float dark_masking;
  int slice_count;
  int prediction_method;
  int *slice_offset;
  AVRational sample_aspect_ratio; //
  int me_cmp;
  int me_sub_cmp;
  int mb_cmp;
  int ildct_cmp;
  int dia_size;
  int last_predictor_count;
  int pre_me;
  int me_pre_cmp;
  int pre_dia_size;
  int me_subpel_quality;
  int dtg_active_format;
  int me_range;
  int intra_quant_bias;
  int inter_quant_bias;
  int slice_flags;
  int xvmc_acceleration;
  int mb_decision;
  uint16_t *intra_matrix;
  uint16_t *inter_matrix;
  int scenechange_threshold;
  int noise_reduction;
  int me_threshold;
  int mb_threshold;
  int intra_dc_precision;
  int skip_top;
  int skip_bottom;
  float border_masking;
  int mb_lmin;
  int mb_lmax;
  int me_penalty_compensation;
  int bidir_refine;
  int brd_scale;
  int keyint_min;
  int refs;
  int chromaoffset;
  int scenechange_factor;
  int mv0_threshold;
  int b_sensitivity;
  AVColorPrimaries color_primaries; //
  AVColorTransferCharacteristic color_trc; //
  AVColorSpace colorspace;  //
  AVColorRange color_range; //
  AVChromaLocation chroma_sample_location; //

  // Actually, there is more here, but the variables above are the only we need.
} AVCodecContext_56;

typedef struct AVCodecContext_57 
{
  const AVClass *av_class;
  int log_level_offset;
  AVMediaType codec_type;
  const struct AVCodec *codec;
  char codec_name[32];
  AVCodecID codec_id;
  unsigned int codec_tag;
  unsigned int stream_codec_tag;
  void *priv_data;
  struct AVCodecInternal *internal;
  void *opaque;
  int64_t bit_rate;
  int bit_rate_tolerance;
  int global_quality;
  int compression_level;
  int flags;
  int flags2;
  uint8_t *extradata;
  int extradata_size;
  AVRational time_base;
  int ticks_per_frame;
  int delay;
  int width, height;
  int coded_width, coded_height;
  int gop_size;
  AVPixelFormat pix_fmt;
  int me_method;
  void (*draw_horiz_band)(struct AVCodecContext *s, const AVFrame *src, int offset[AV_NUM_DATA_POINTERS], int y, int type, int height);
  AVPixelFormat (*get_format)(struct AVCodecContext *s, const AVPixelFormat * fmt);
  int max_b_frames;
  float b_quant_factor;
  int rc_strategy;
  int b_frame_strategy;
  float b_quant_offset;
  int has_b_frames;
  int mpeg_quant;
  float i_quant_factor;
  float i_quant_offset;
  float lumi_masking;
  float temporal_cplx_masking;
  float spatial_cplx_masking;
  float p_masking;
  float dark_masking;
  int slice_count;
  int prediction_method;
  int *slice_offset;
  AVRational sample_aspect_ratio;
  int me_cmp;
  int me_sub_cmp;
  int mb_cmp;
  int ildct_cmp;
  int dia_size;
  int last_predictor_count;
  int pre_me;
  int me_pre_cmp;
  int pre_dia_size;
  int me_subpel_quality;
  int dtg_active_format;
  int me_range;
  int intra_quant_bias;
  int inter_quant_bias;
  int slice_flags;
  int xvmc_acceleration;
  int mb_decision;
  uint16_t *intra_matrix;
  uint16_t *inter_matrix;
  int scenechange_threshold;
  int noise_reduction;
  int me_threshold;
  int mb_threshold;
  int intra_dc_precision;
  int skip_top;
  int skip_bottom;
  float border_masking;
  int mb_lmin;
  int mb_lmax;
  int me_penalty_compensation;
  int bidir_refine;
  int brd_scale;
  int keyint_min;
  int refs;
  int chromaoffset;
  int scenechange_factor;
  int mv0_threshold;
  int b_sensitivity;
  AVColorPrimaries color_primaries;
  AVColorTransferCharacteristic color_trc;
  AVColorSpace colorspace;
  AVColorRange color_range;
  AVChromaLocation chroma_sample_location;

  // Actually, there is more here, but the variables above are the only we need.
} AVCodecContext_57;

// AVStream is part of AVFormat
typedef struct AVStream_56
{
  int index;
  int id;
  AVCodecContext *codec;
  void *priv_data;
  struct AVFrac pts;
  AVRational time_base;
  int64_t start_time;
  int64_t duration;
  int64_t nb_frames;
  int disposition;
  enum AVDiscard discard;
  AVRational sample_aspect_ratio;
  AVDictionary *metadata;
  AVRational avg_frame_rate;
  AVPacket_56 attached_pic;
  AVPacketSideData *side_data;
  int nb_side_data;
  int event_flags;
} AVStream_56;

typedef struct AVProbeData_57 {
  const char *filename;
  unsigned char *buf;
  int buf_size;
  const char *mime_type;
} AVProbeData_57;

typedef struct AVStream_57
{
  int index;
  int id;
  AVCodecContext *codec;  // Deprecated. Might be removed in the next major version.
  void *priv_data;
  struct AVFrac pts;  // Deprecated. Might be removed in the next major version.
  AVRational time_base;
  int64_t start_time;
  int64_t duration;
  int64_t nb_frames;
  int disposition;
  enum AVDiscard discard;
  AVRational sample_aspect_ratio;
  AVDictionary *metadata;
  AVRational avg_frame_rate;
  AVPacket_57 attached_pic;
  AVPacketSideData *side_data;
  int nb_side_data;
  int event_flags;
  // All field following this line are not part of the public API and may change/be removed. However,
  // we still need them here because further below some fields which are part of the public API will follow.
  // I really don't understand who thought up this idiotic scheme...
#define MAX_STD_TIMEBASES (30*12+30+3+6)
  struct {
    int64_t last_dts;
    int64_t duration_gcd;
    int duration_count;
    int64_t rfps_duration_sum;
    double (*duration_error)[2][MAX_STD_TIMEBASES];
    int64_t codec_info_duration;
    int64_t codec_info_duration_fields;
    int found_decoder;
    int64_t last_duration;
    int64_t fps_first_dts;
    int     fps_first_dts_idx;
    int64_t fps_last_dts;
    int     fps_last_dts_idx;
  } *info;
  int pts_wrap_bits;
  int64_t first_dts;
  int64_t cur_dts;
  int64_t last_IP_pts;
  int last_IP_duration;
  int probe_packets;
  int codec_info_nb_frames;
  enum AVStreamParseType need_parsing;
  struct AVCodecParserContext *parser;
  struct AVPacketList *last_in_packet_buffer;
  AVProbeData_57 probe_data;
#define MAX_REORDER_DELAY 16
  int64_t pts_buffer[MAX_REORDER_DELAY+1];
  AVIndexEntry *index_entries;
  int nb_index_entries;
  unsigned int index_entries_allocated_size;
  AVRational r_frame_rate;
  int stream_identifier;
  int64_t interleaver_chunk_size;
  int64_t interleaver_chunk_duration;
  int request_probe;
  int skip_to_keyframe;
  int skip_samples;
  int64_t start_skip_samples;
  int64_t first_discard_sample;
  int64_t last_discard_sample;
  int nb_decoded_frames;
  int64_t mux_ts_offset;
  int64_t pts_wrap_reference;
  int pts_wrap_behavior;
  int update_initial_durations_done;
  int64_t pts_reorder_error[MAX_REORDER_DELAY+1];
  uint8_t pts_reorder_error_count[MAX_REORDER_DELAY+1];
  int64_t last_dts_for_order_check;
  uint8_t dts_ordered;
  uint8_t dts_misordered;
  int inject_global_side_data;
  // All fields above this line are not part of the public API.
  // All fields below are part of the public API and ABI again.
  char *recommended_encoder_configuration;
  AVRational display_aspect_ratio;
  struct FFFrac *priv_pts;
  AVStreamInternal *internal;
  AVCodecParameters *codecpar;
} AVStream_57;

// AVCodecParameters is part of avcodec.
typedef struct AVCodecParameters_57
{
  AVMediaType codec_type;
  AVCodecID   codec_id;
  uint32_t    codec_tag;
  uint8_t *extradata;
  int extradata_size;
  int format;
  int64_t bit_rate;
  int bits_per_coded_sample;
  int bits_per_raw_sample;
  int profile;
  int level;
  int width;
  int height;
  AVRational                    sample_aspect_ratio;
  AVFieldOrder                  field_order;
  AVColorRange                  color_range;
  AVColorPrimaries              color_primaries;
  AVColorTransferCharacteristic color_trc;
  AVColorSpace                  color_space;
  AVChromaLocation              chroma_location;
  int video_delay;

  // Actually, there is more here, but the variables above are the only we need.
} AVCodecParameters_57;

// AVFrames is part of AVUtil
typedef struct AVFrame_55 
{
  uint8_t *data[AV_NUM_DATA_POINTERS];
  int linesize[AV_NUM_DATA_POINTERS];
  uint8_t **extended_data;
  int width, height;
  int nb_samples;
  int format;
  int key_frame;
  AVPictureType pict_type;
  AVRational sample_aspect_ratio;
  int64_t pts;
  int64_t pkt_pts;
  int64_t pkt_dts;
  int coded_picture_number;
  int display_picture_number;
  int quality;

  // Actually, there is more here, but the variables above are the only we need.
} AVFrame_55;

typedef struct AVFrame_54 
{
  uint8_t *data[AV_NUM_DATA_POINTERS];
  int linesize[AV_NUM_DATA_POINTERS];
  uint8_t **extended_data;
  int width, height;
  int nb_samples;
  int format;
  int key_frame;
  AVPictureType pict_type;
  uint8_t *base[AV_NUM_DATA_POINTERS];
  AVRational sample_aspect_ratio;
  int64_t pts;
  int64_t pkt_pts;
  int64_t pkt_dts;
  int coded_picture_number;
  int display_picture_number;
  int quality;

  // Actually, there is more here, but the variables above are the only we need.
} AVFrame_54;

// ------------------- AVInputFormat ---------------
// AVInputFormat is part of AVFormat
typedef struct AVInputFormat_56_57
{
  const char *name;
  const char *long_name;
  int flags;
  const char *extensions;
  const struct AVCodecTag * const *codec_tag;
  const AVClass *priv_class;
  const char *mime_type;
} AVInputFormat_56_57;

// ------------------- AVFrameSideData ---------------
// AVFrameSideData is part of AVUtil
typedef struct AVFrameSideData_54_55
{
  enum AVFrameSideDataType type;
  uint8_t *data;
  int      size;
  AVDictionary *metadata;
  AVBufferRef *buf;
} AVFrameSideData_54_55;

// ------------------- AVMotionVector ---------------

typedef struct AVMotionVector_54
{
  int32_t source;
  uint8_t w, h;
  int16_t src_x, src_y;
  int16_t dst_x, dst_y;
  uint64_t flags;
} AVMotionVector_54;

typedef struct AVMotionVector_55 
{
  int32_t source;
  uint8_t w, h;
  int16_t src_x, src_y;
  int16_t dst_x, dst_y;
  uint64_t flags;
  int32_t motion_x, motion_y;
  uint16_t motion_scale;
} AVMotionVector_55;

// -------- FFmpegLibraryFunctions -----------

FFmpegLibraryFunctions::FFmpegLibraryFunctions()
{
  av_register_all = nullptr;
  avformat_open_input = nullptr;
  avformat_close_input = nullptr;
  avformat_find_stream_info = nullptr;
  av_read_frame = nullptr;
  av_seek_frame = nullptr;
  avformat_version = nullptr;

  avcodec_find_decoder = nullptr;
  avcodec_alloc_context3 = nullptr;
  avcodec_open2 = nullptr;
  avcodec_free_context = nullptr;
  av_init_packet = nullptr;
  av_packet_unref = nullptr;
  avcodec_flush_buffers = nullptr;
  avcodec_version = nullptr;
  avcodec_get_name = nullptr;
  avcodec_parameters_alloc = nullptr;

  avcodec_send_packet = nullptr;
  avcodec_receive_frame = nullptr;
  avcodec_parameters_to_context = nullptr;
  newParametersAPIAvailable = false;
  avcodec_decode_video2 = nullptr;

  av_frame_alloc = nullptr;
  av_frame_free = nullptr;
  av_mallocz = nullptr;
  avutil_version = nullptr;

  swresample_version = nullptr;
}

bool FFmpegLibraryFunctions::bindFunctionsFromAVFormatLib()
{
  if (!resolveAvFormat(av_register_all, "av_register_all")) return false;
  if (!resolveAvFormat(avformat_open_input, "avformat_open_input")) return false;
  if (!resolveAvFormat(avformat_close_input, "avformat_close_input")) return false;
  if (!resolveAvFormat(avformat_find_stream_info, "avformat_find_stream_info")) return false;
  if (!resolveAvFormat(av_read_frame, "av_read_frame")) return false;
  if (!resolveAvFormat(av_seek_frame, "av_seek_frame")) return false;
  if (!resolveAvFormat(avformat_version, "avformat_version")) return false;
  return true;
}

bool FFmpegLibraryFunctions::bindFunctionsFromAVCodecLib()
{
  if (!resolveAvCodec(avcodec_find_decoder, "avcodec_find_decoder")) return false;
  if (!resolveAvCodec(avcodec_alloc_context3, "avcodec_alloc_context3")) return false;
  if (!resolveAvCodec(avcodec_open2, "avcodec_open2")) return false;
  if (!resolveAvCodec(avcodec_free_context, "avcodec_free_context")) return false;
  if (!resolveAvCodec(av_init_packet, "av_init_packet")) return false;
  if (!resolveAvCodec(av_packet_unref, "av_packet_unref")) return false;
  if (!resolveAvCodec(avcodec_flush_buffers, "avcodec_flush_buffers")) return false;
  if (!resolveAvCodec(avcodec_version, "avcodec_version")) return false;
  if (!resolveAvCodec(avcodec_get_name, "avcodec_get_name")) return false;
  if (!resolveAvCodec(avcodec_parameters_alloc, "avcodec_parameters_alloc")) return false;

  // The following functions are part of the new API. If they are not available, we use the old API.
  // If available, we should however use it.
  newParametersAPIAvailable = true;
  newParametersAPIAvailable &= resolveAvCodec(avcodec_send_packet, "avcodec_send_packet", false);
  newParametersAPIAvailable &= resolveAvCodec(avcodec_receive_frame, "avcodec_receive_frame", false);
  newParametersAPIAvailable &= resolveAvCodec(avcodec_parameters_to_context, "avcodec_parameters_to_context", false);

  if (!newParametersAPIAvailable)
    // If the new API is not available, then the old one must be available.
    if (!resolveAvCodec(avcodec_decode_video2, "avcodec_decode_video2")) return false;

  return true;
}

bool FFmpegLibraryFunctions::bindFunctionsFromAVUtilLib()
{
  if (!resolveAvUtil(av_frame_alloc, "av_frame_alloc")) return false;
  if (!resolveAvUtil(av_frame_free, "av_frame_free")) return false;
  if (!resolveAvUtil(av_mallocz, "av_mallocz")) return false;
  if (!resolveAvUtil(avutil_version, "avutil_version")) return false;
  if (!resolveAvUtil(av_dict_set, "av_dict_set")) return false;
  if (!resolveAvUtil(av_dict_get, "av_dict_get")) return false;
  if (!resolveAvUtil(av_frame_get_side_data, "av_frame_get_side_data")) return false;
  if (!resolveAvUtil(av_frame_get_metadata, "av_frame_get_metadata")) return false;
  return true;
}

bool FFmpegLibraryFunctions::bindFunctionsFromSWResampleLib()
{
  return resolveSwresample(swresample_version, "swresample_version");
}

bool FFmpegLibraryFunctions::bindFunctionsFromLibraries()
{
  // Loading the libraries was successfull. Get/check function pointers.
  return bindFunctionsFromAVFormatLib() && bindFunctionsFromAVCodecLib() && bindFunctionsFromAVUtilLib() && bindFunctionsFromSWResampleLib();
}

QFunctionPointer FFmpegLibraryFunctions::resolveAvUtil(const char *symbol)
{
  QFunctionPointer ptr = libAvutil.resolve(symbol);
  if (!ptr)
    setError(QStringLiteral("Error loading the avutil library: Can't find function %1.").arg(symbol));
  return ptr;
}

template <typename T> bool FFmpegLibraryFunctions::resolveAvUtil(T &fun, const char *symbol)
{
  fun = reinterpret_cast<T>(resolveAvUtil(symbol));
  return (fun != nullptr);
}

QFunctionPointer FFmpegLibraryFunctions::resolveAvFormat(const char *symbol)
{
  QFunctionPointer ptr = libAvformat.resolve(symbol);
  if (!ptr)
    setError(QStringLiteral("Error loading the avformat library: Can't find function %1.").arg(symbol));
  return ptr;
}

template <typename T> bool FFmpegLibraryFunctions::resolveAvFormat(T &fun, const char *symbol)
{
  fun = reinterpret_cast<T>(resolveAvFormat(symbol));
  return (fun != nullptr);
}

QFunctionPointer FFmpegLibraryFunctions::resolveAvCodec(const char *symbol, bool failIsError)
{
  // Failure to resolve the function is only an error if failIsError is set.
  QFunctionPointer ptr = libAvcodec.resolve(symbol);
  if (!ptr && failIsError)
    setError(QStringLiteral("Error loading the avcodec library: Can't find function %1.").arg(symbol));
  return ptr;
}

template <typename T> bool FFmpegLibraryFunctions::resolveAvCodec(T &fun, const char *symbol, bool failIsError)
{
  fun = reinterpret_cast<T>(resolveAvCodec(symbol, failIsError));
  return (fun != nullptr);
}

QFunctionPointer FFmpegLibraryFunctions::resolveSwresample(const char *symbol)
{
  QFunctionPointer ptr = libSwresample.resolve(symbol);
  if (!ptr)
    setError(QStringLiteral("Error loading the swresample library: Can't find function %1.").arg(symbol));
  return ptr;
}

template <typename T> bool FFmpegLibraryFunctions::resolveSwresample(T &fun, const char *symbol)
{
  fun = reinterpret_cast<T>(resolveSwresample(symbol));
  return (fun != nullptr);
}

bool FFmpegLibraryFunctions::loadFFmpegLibraryInPath(QString path, int libVersions[4])
{
  // Clear the error state if one was set.
  error_list.clear();
  libAvutil.unload();
  libSwresample.unload();
  libAvcodec.unload();
  libAvformat.unload();

  // We will load the following libraries (in this order):
  // avutil, swresample, avcodec, avformat.

  if (!path.isEmpty())
  {
    QDir givenPath(path);
    if (!givenPath.exists())
      // The given path is invalid
      return false;

    // Get the absolute path
    path = givenPath.absolutePath() + "/";
  }

  libPath = path;

  // The ffmpeg libraries are named using a major version number. E.g: avutil-55.dll on windows.
  // On linux, the libraries may be named differently. On Ubuntu they are named libavutil-ffmpeg.so.55.
  // On arch linux the name is libavutil.so.55. We will try to look for both namings.
  // On MAC os (installed with homebrew), there is a link to the lib named libavutil.54.dylib.
  int nrNames = (is_Q_OS_LINUX) ? 2 : ((is_Q_OS_WIN) ? 1 : 1);
  bool success = false;
  for (int i=0; i<nrNames; i++)
  {
    success = true;

    // This is how we the library name is constructed per platform
    QString constructLibName;
    if (is_Q_OS_WIN)
      constructLibName = "%1-%2";
    if (is_Q_OS_LINUX && i == 0)
      constructLibName = "lib%1-ffmpeg.so.%2";
    if (is_Q_OS_LINUX && i == 1)
      constructLibName = "lib%1.so.%2";
    if (is_Q_OS_MAC)
      constructLibName = "lib%1.%2.dylib";
    
    // Start with the avutil library
    libAvutil.setFileName(path + constructLibName.arg("avutil").arg(libVersions[0]));
    if (!libAvutil.load())
      success = false;

    // Next, the swresample library.
    libSwresample.setFileName(path + constructLibName.arg("swresample").arg(libVersions[1]));
    if (success && !libSwresample.load())
      success = false;

    // avcodec
    libAvcodec.setFileName(path + constructLibName.arg("avcodec").arg(libVersions[2]));
    if (success && !libAvcodec.load())
      success = false;

    // avformat
    libAvformat.setFileName(path + constructLibName.arg("avformat").arg(libVersions[3]));
    if (success && !libAvformat.load())
      success = false;

    if (success)
      break;
    else
    {
      libAvutil.unload();
      libSwresample.unload();
      libAvcodec.unload();
      libAvformat.unload();
    }
  }

  if (!success)
    return false;

  // For the last test: Try to get pointers to all the libraries.
  return bindFunctionsFromLibraries();
}

bool FFmpegLibraryFunctions::loadFFMpegLibrarySpecific(QString avFormatLib, QString avCodecLib, QString avUtilLib, QString swResampleLib)
{
  // Clear the error state if one was set.
  error_list.clear();
  libAvutil.unload();
  libSwresample.unload();
  libAvcodec.unload();
  libAvformat.unload();

  // We will load the libraries (in this order):
  // avutil, swresample, avcodec, avformat.
  bool success = true;
  libAvutil.setFileName(avUtilLib);
  if (!libAvutil.load())
    success = false;

  // Next, the swresample library.
  libSwresample.setFileName(swResampleLib);
  if (success && !libSwresample.load())
    success = false;

  // avcodec
  libAvcodec.setFileName(avCodecLib);
  if (success && !libAvcodec.load())
    success = false;

  // avformat
  libAvformat.setFileName(avFormatLib);
  if (success && !libAvformat.load())
    success = false;

  if (!success)
  {
    libAvutil.unload();
    libSwresample.unload();
    libAvcodec.unload();
    libAvformat.unload();
  }
  else
  {
    // For the last test: Try to get pointers to all the libraries.
    return bindFunctionsFromLibraries();
  }
  return false;
}

// ----------------- FFmpegVersionHandler -------------------------------------------

FFmpegVersionHandler::FFmpegVersionHandler()
{
  libVersion.avcodec = -1;
  libVersion.avformat = -1;
  libVersion.avutil = -1;
  libVersion.swresample = -1;

  librariesLoaded = false;
}

bool FFmpegVersionHandler::loadFFmpegLibraries()
{
  if (librariesLoaded)
    return true;

  // Try to load the ffmpeg libraries from the current working directory and several other directories.
  // Unfortunately relative paths like "./" do not work: (at least on windows)

  // First try the specific FFMpeg libraries (if set)
  QSettings settings;
  settings.beginGroup("Decoders");
  QString avFormatLib = settings.value("FFMpeg.avformat", "").toString();
  QString avCodecLib = settings.value("FFMpeg.avcodec", "").toString();
  QString avUtilLib = settings.value("FFMpeg.avutil", "").toString();
  QString swResampleLib = settings.value("FFMpeg.swresample", "").toString();
  librariesLoaded = loadFFMpegLibrarySpecific(avFormatLib, avCodecLib, avUtilLib, swResampleLib);
  if (librariesLoaded)
    return true;

  // Next, we will try some other paths / options
  QStringList possibilites;
  QString decoderSearchPath = settings.value("SearchPath", "").toString();
  if (!decoderSearchPath.isEmpty())
    possibilites.append(decoderSearchPath);                                   // Try the specific search path (if one is set)
  possibilites.append(QDir::currentPath() + "/");                             // Try the current working directory
  possibilites.append(QDir::currentPath() + "/ffmpeg/");
  possibilites.append(QCoreApplication::applicationDirPath() + "/");          // Try the path of the YUView.exe
  possibilites.append(QCoreApplication::applicationDirPath() + "/ffmpeg/");
  possibilites.append("");                                                    // Just try to call QLibrary::load so that the system folder will be searched.

  for (QString path : possibilites)
  {
    librariesLoaded = loadFFmpegLibraryInPath(path);
    if (librariesLoaded)
      return true;
  }

  // Loading the libraries failed
  return false;
}

bool FFmpegVersionHandler::open_input(AVFormatContextWrapper &fmt, QString url)
{
  AVFormatContext *f_ctx = nullptr;
  int ret = lib.avformat_open_input(&f_ctx, url.toStdString().c_str(), nullptr, nullptr);
  if (ret < 0)
    return setError(QStringLiteral("Error opening file (avformat_open_input). Ret code %1").arg(ret));
  if (f_ctx == nullptr)
    return setError(QStringLiteral("Error opening file (avformat_open_input). No format context returned."));
  
  // The wrapper will take ownership of this pointer
  fmt = AVFormatContextWrapper(f_ctx, libVersion);
  
  ret = lib.avformat_find_stream_info(fmt.get_format_ctx(), nullptr);
  if (ret < 0)
    return setError(QStringLiteral("Error opening file (avformat_find_stream_info). Ret code %1").arg(ret));
  
  // Get the codec id string using avcodec_get_name for each stream
  for(unsigned int idx=0; idx < fmt.get_nb_streams(); idx++)
  {
    AVStreamWrapper stream = fmt.get_stream(idx);
    const char *name = lib.avcodec_get_name(stream.getCodecID());
    stream.getCodec().codec_id_string = QString(name);
  }

  return true;
}

AVCodecParametersWrapper FFmpegVersionHandler::alloc_code_parameters()
{
  return AVCodecParametersWrapper(lib.avcodec_parameters_alloc(), libVersion);
}

AVCodecWrapper FFmpegVersionHandler::find_decoder(AVCodecID codec_id)
{
  return AVCodecWrapper(lib.avcodec_find_decoder(codec_id), libVersion);
}

AVCodecContextWrapper FFmpegVersionHandler::alloc_decoder(AVCodecWrapper &codec)
{
  return AVCodecContextWrapper(lib.avcodec_alloc_context3(codec.getAVCodec()), libVersion);
}

int FFmpegVersionHandler::av_dict_set(AVDictionaryWrapper &dict, const char *key, const char *value, int flags)
{
  AVDictionary *d = dict.get_dictionary();
  int ret = lib.av_dict_set(&d, key, value, flags);
  dict.setDictionary(d);
  return ret;
}

QStringPairList FFmpegVersionHandler::get_dictionary_entries(AVDictionaryWrapper d, QString key, int flags)
{
  QStringPairList ret;
  AVDictionaryEntry *tag = NULL;
  while ((tag = lib.av_dict_get(d.get_dictionary(), key.toLatin1().data(), tag, flags)))
  {
    QStringPair pair;
    pair.first = QString(tag->key);
    pair.second = QString(tag->value);
    ret.append(pair);
  }
  return ret;
}

int FFmpegVersionHandler::avcodec_open2(AVCodecContextWrapper &decCtx, AVCodecWrapper &codec, AVDictionaryWrapper &dict)
{
  AVDictionary *d = dict.get_dictionary();
  int ret = lib.avcodec_open2(decCtx.get_codec(), codec.getAVCodec(), &d);
  dict.setDictionary(d);
  return ret;
}

AVFrameSideDataWrapper FFmpegVersionHandler::get_side_data(AVFrameWrapper &frame, AVFrameSideDataType type)
{
  AVFrameSideData *sd = lib.av_frame_get_side_data(frame.get_frame(), type);
  return AVFrameSideDataWrapper(sd, libVersion);
}

AVDictionaryWrapper FFmpegVersionHandler::get_metadata(AVFrameWrapper &frame)
{
  AVDictionary *dict = lib.av_frame_get_metadata(frame.get_frame());
  return AVDictionaryWrapper(dict);
}

int FFmpegVersionHandler::seek_frame(AVFormatContextWrapper & fmt, int stream_idx, int pts)
{
  int ret = lib.av_seek_frame(fmt.get_format_ctx(), stream_idx, pts, AVSEEK_FLAG_BACKWARD);
  return ret;
}

bool FFmpegVersionHandler::checkLibraryVersions()
{
  // Get the version number of the opened libraries and check them against the
  // versions we tried to open. Also get the minor and micro version numbers.
  
  int avCodecVer = lib.avcodec_version();
  if (AV_VERSION_MAJOR(avCodecVer) != libVersion.avcodec)
  {
    error_list.append("The openend libAvCodec returned a different major version than it's file name indicates.");
    // Try the next version
    return false;
  }
  libVersion.avcodec_minor = AV_VERSION_MINOR(avCodecVer);
  libVersion.avcodec_micro = AV_VERSION_MICRO(avCodecVer);

  int avFormatVer = lib.avformat_version();
  if (AV_VERSION_MAJOR(avFormatVer) != libVersion.avformat)
    return setError("The openend libAvFormat returned a different major version than it's file name indicates.");
  
  libVersion.avformat_minor = AV_VERSION_MINOR(avFormatVer);
  libVersion.avformat_micro = AV_VERSION_MICRO(avFormatVer);

  int avUtilVer = lib.avutil_version();
  if (AV_VERSION_MAJOR(avUtilVer) != libVersion.avutil)
    return setError("The openend libAvUtil returned a different major version than it's file name indicates.");
    
  libVersion.avutil_minor = AV_VERSION_MINOR(avUtilVer);
  libVersion.avutil_micro = AV_VERSION_MICRO(avUtilVer);

  int swresampleVer = lib.swresample_version();
  if (AV_VERSION_MAJOR(swresampleVer) != libVersion.swresample)
    return setError("The openend libSwresampleVer returned a different major version than it's file name indicates.");
    
  libVersion.swresample_minor = AV_VERSION_MINOR(swresampleVer);
  libVersion.swresample_micro = AV_VERSION_MICRO(swresampleVer);

  // This is a combination of versions that we can use
  return true;  
}

bool FFmpegVersionHandler::loadFFmpegLibraryInPath(QString path)
{
  bool success = false;
  for (int i = 0; i < FFMpegVersion_Num; i++)
  {
    FFmpegVersions v = (FFmpegVersions)i;

    int verNum[4];
    verNum[0] = getLibVersionUtil(v);
    verNum[1] = getLibVersionSwresample(v);
    verNum[2] = getLibVersionCodec(v);
    verNum[3] = getLibVersionFormat(v);

    if (lib.loadFFmpegLibraryInPath(path, verNum))
    {
      // This worked. Now check the version number of the libraries
      libVersion.avutil = verNum[0];
      libVersion.swresample = verNum[1];
      libVersion.avcodec = verNum[2];
      libVersion.avformat = verNum[3];

      success = checkLibraryVersions();
      if (success)
        // Everything worked. We can break the loop over all versions that we support.
        break;
    }
  }

  if (success)
  {
    error_list.clear();
    // Initialize libavformat and register all the muxers, demuxers and protocols.
    lib.av_register_all();
    // If needed, we would also have to register the network features (avformat_network_init)
  }

  return success;
}

bool FFmpegVersionHandler::loadFFMpegLibrarySpecific(QString avFormatLib, QString avCodecLib, QString avUtilLib, QString swResampleLib)
{
  bool success = false;
  for (int i = 0; i < FFMpegVersion_Num; i++)
  {
    FFmpegVersions v = (FFmpegVersions)i;
    
    if (lib.loadFFMpegLibrarySpecific(avFormatLib, avCodecLib, avUtilLib, swResampleLib))
    {
      // This worked. Now check the version number of the libraries
      libVersion.avutil = getLibVersionUtil(v);
      libVersion.swresample = getLibVersionSwresample(v);
      libVersion.avcodec = getLibVersionCodec(v);
      libVersion.avformat = getLibVersionFormat(v);

      success = checkLibraryVersions();
      if (success)
        // Everything worked. We can break the loop over all versions that we support.
        break;
    }
  }

  if (success)
  {
    error_list.clear();
    // Initialize libavformat and register all the muxers, demuxers and protocols.
    lib.av_register_all();
    // If needed, we would also have to register the network features (avformat_network_init)
  }

  return success;
}

bool FFmpegVersionHandler::checkLibraryFiles(QString avCodecLib, QString avFormatLib, QString avUtilLib, QString swResampleLib, QStringList &error)
{
  FFmpegVersionHandler handler;
  if (!handler.loadFFMpegLibrarySpecific(avFormatLib, avCodecLib, avUtilLib, swResampleLib))
  {
    error = handler.getErrors();
    return false;
  }
  return true;
}

RawFormat FFmpegVersionHandler::getRawFormat(AVPixelFormat pixelFormat)
{
  if (convertAVPixelFormatYUV(pixelFormat).isValid())
    return raw_YUV;
  if (convertAVPixelFormatRGB(pixelFormat).isValid())
    return raw_RGB;
  return raw_Invalid;
}

// Convert from the AVPixelFormat to the internal yuvPixelFormat
yuvPixelFormat FFmpegVersionHandler::convertAVPixelFormatYUV(AVPixelFormat pixelFormat)
{
  // YUV 4:2:0 formats
  if (pixelFormat == AV_PIX_FMT_YUV420P)
    return yuvPixelFormat(YUV_420, 8);
  if (pixelFormat == AV_PIX_FMT_YUV420P16LE)
    return yuvPixelFormat(YUV_420, 16);
  if (pixelFormat == AV_PIX_FMT_YUV420P16BE)
    return yuvPixelFormat(YUV_420, 16, Order_YUV, true);
  if (pixelFormat == AV_PIX_FMT_YUV420P9BE)
    return yuvPixelFormat(YUV_420, 9, Order_YUV, true);
  if (pixelFormat == AV_PIX_FMT_YUV420P9LE)
    return yuvPixelFormat(YUV_420, 9);
  if (pixelFormat == AV_PIX_FMT_YUV420P10BE)
    return yuvPixelFormat(YUV_420, 10, Order_YUV, true);
  if (pixelFormat == AV_PIX_FMT_YUV420P10LE)
    return yuvPixelFormat(YUV_420, 10);
  if (pixelFormat == AV_PIX_FMT_YUV420P12BE)
    return yuvPixelFormat(YUV_420, 12, Order_YUV, true);
  if (pixelFormat == AV_PIX_FMT_YUV420P12LE)
    return yuvPixelFormat(YUV_420, 12);
  if (pixelFormat == AV_PIX_FMT_YUV420P14BE)
    return yuvPixelFormat(YUV_420, 14, Order_YUV, true);
  if (pixelFormat == AV_PIX_FMT_YUV420P14LE)
    return yuvPixelFormat(YUV_420, 14);

  // YUV 4:2:2 formats
  if (pixelFormat == AV_PIX_FMT_YUV422P)
    return yuvPixelFormat(YUV_422, 8);
  if (pixelFormat == AV_PIX_FMT_YUV422P16LE)
    return yuvPixelFormat(YUV_422, 16);
  if (pixelFormat == AV_PIX_FMT_YUV422P16BE)
    return yuvPixelFormat(YUV_422, 16, Order_YUV, true);
  if (pixelFormat == AV_PIX_FMT_YUV422P10BE)
    return yuvPixelFormat(YUV_422, 10, Order_YUV, true);
  if (pixelFormat == AV_PIX_FMT_YUV422P10LE)
    return yuvPixelFormat(YUV_422, 10);
  if (pixelFormat == AV_PIX_FMT_YUV422P9BE)
    return yuvPixelFormat(YUV_422, 9, Order_YUV, true);
  if (pixelFormat == AV_PIX_FMT_YUV422P9LE)
    return yuvPixelFormat(YUV_422, 9);
  if (pixelFormat == AV_PIX_FMT_YUV422P12BE)
    return yuvPixelFormat(YUV_422, 12, Order_YUV, true);
  if (pixelFormat == AV_PIX_FMT_YUV422P12LE)
    return yuvPixelFormat(YUV_422, 12);
  if (pixelFormat == AV_PIX_FMT_YUV422P14BE)
    return yuvPixelFormat(YUV_422, 14, Order_YUV, true);
  if (pixelFormat == AV_PIX_FMT_YUV422P14LE)
    return yuvPixelFormat(YUV_422, 14);

  // YUV 4:4:4 formats
  if (pixelFormat == AV_PIX_FMT_YUV444P)
    return yuvPixelFormat(YUV_444, 8);
  if (pixelFormat == AV_PIX_FMT_YUV444P16LE)
    return yuvPixelFormat(YUV_444, 16);
  if (pixelFormat == AV_PIX_FMT_YUV444P16BE)
    return yuvPixelFormat(YUV_444, 16, Order_YUV, true);
  if (pixelFormat == AV_PIX_FMT_YUV444P9BE)
    return yuvPixelFormat(YUV_444, 9, Order_YUV, true);
  if (pixelFormat == AV_PIX_FMT_YUV444P9LE)
    return yuvPixelFormat(YUV_444, 9);
  if (pixelFormat == AV_PIX_FMT_YUV444P10BE)
    return yuvPixelFormat(YUV_444, 10, Order_YUV, true);
  if (pixelFormat == AV_PIX_FMT_YUV444P10LE)
    return yuvPixelFormat(YUV_444, 10);
  if (pixelFormat == AV_PIX_FMT_YUV444P12BE)
    return yuvPixelFormat(YUV_444, 12, Order_YUV, true);
  if (pixelFormat == AV_PIX_FMT_YUV444P12LE)
    return yuvPixelFormat(YUV_444, 12);
  if (pixelFormat == AV_PIX_FMT_YUV444P14BE)
    return yuvPixelFormat(YUV_444, 14, Order_YUV, true);
  if (pixelFormat == AV_PIX_FMT_YUV444P14LE)
    return yuvPixelFormat(YUV_444, 14);

  return yuvPixelFormat();
}

rgbPixelFormat FFmpegVersionHandler::convertAVPixelFormatRGB(AVPixelFormat pixelFormat)
{
  if (pixelFormat == AV_PIX_FMT_RGB24)
    return rgbPixelFormat(8, false, false);
  if (pixelFormat == AV_PIX_FMT_BGR24)
    return rgbPixelFormat(8, false, false, 2, 1, 0);
  if (pixelFormat == AV_PIX_FMT_ARGB)
    return rgbPixelFormat(8, false, true);
  if (pixelFormat == AV_PIX_FMT_ABGR)
    return rgbPixelFormat(8, false, true, 2, 1, 0);
  if (pixelFormat == AV_PIX_FMT_RGB48LE)
    return rgbPixelFormat(16, false, false);
  
  // Other formats are not yet supported
  // Adding them should be straightforward (but also not trivial)
  return rgbPixelFormat();
}

// Convert from yuvPixelFormat to AVPixelFormat
AVPixelFormat FFmpegVersionHandler::convertYUVAVPixelFormat(yuvPixelFormat fmt)
{
  if (fmt.subsampling == YUV_420)
  {
    if (fmt.bitsPerSample == 8 && fmt.planeOrder == Order_YUV && !fmt.bigEndian)
      return AV_PIX_FMT_YUV420P;
    if (fmt.bitsPerSample == 16 && fmt.planeOrder == Order_YUV && !fmt.bigEndian)
      return AV_PIX_FMT_YUV420P16LE;
    if (fmt.bitsPerSample == 16 && fmt.planeOrder == Order_YUV && fmt.bigEndian)
      return AV_PIX_FMT_YUV420P16BE;
    if (fmt.bitsPerSample == 9 && fmt.planeOrder == Order_YUV && !fmt.bigEndian)
      return AV_PIX_FMT_YUV420P9BE;
    if (fmt.bitsPerSample == 9 && fmt.planeOrder == Order_YUV && !fmt.bigEndian)
      return AV_PIX_FMT_YUV420P9LE;
    if (fmt.bitsPerSample == 10 && fmt.planeOrder == Order_YUV && fmt.bigEndian)
      return AV_PIX_FMT_YUV420P10BE;
    if (fmt.bitsPerSample == 10 && fmt.planeOrder == Order_YUV && !fmt.bigEndian)
      return AV_PIX_FMT_YUV420P10LE;
    if (fmt.bitsPerSample == 12 && fmt.planeOrder == Order_YUV && fmt.bigEndian)
      return AV_PIX_FMT_YUV420P12BE;
    if (fmt.bitsPerSample == 12 && fmt.planeOrder == Order_YUV && !fmt.bigEndian)
      return AV_PIX_FMT_YUV420P12LE;
    if (fmt.bitsPerSample == 14 && fmt.planeOrder == Order_YUV && fmt.bigEndian)
      return AV_PIX_FMT_YUV420P14BE;
    if (fmt.bitsPerSample == 14 && fmt.planeOrder == Order_YUV && !fmt.bigEndian)
      return AV_PIX_FMT_YUV420P14LE;
  }
  if (fmt.subsampling == YUV_422)
  {
    if (fmt.bitsPerSample == 8 && fmt.planeOrder == Order_YUV && !fmt.bigEndian)
      return AV_PIX_FMT_YUV422P;
    if (fmt.bitsPerSample == 16 && fmt.planeOrder == Order_YUV && !fmt.bigEndian)
      return AV_PIX_FMT_YUV422P16LE;
    if (fmt.bitsPerSample == 16 && fmt.planeOrder == Order_YUV && fmt.bigEndian)
      return AV_PIX_FMT_YUV422P16BE;
    if (fmt.bitsPerSample == 10 && fmt.planeOrder == Order_YUV && fmt.bigEndian)
      return AV_PIX_FMT_YUV422P10BE;
    if (fmt.bitsPerSample == 10 && fmt.planeOrder == Order_YUV && !fmt.bigEndian)
      return AV_PIX_FMT_YUV422P10LE;
    if (fmt.bitsPerSample == 9 && fmt.planeOrder == Order_YUV && fmt.bigEndian)
      return AV_PIX_FMT_YUV422P9BE;
    if (fmt.bitsPerSample == 9 && fmt.planeOrder == Order_YUV && !fmt.bigEndian)
      return AV_PIX_FMT_YUV422P9LE;
    if (fmt.bitsPerSample == 12 && fmt.planeOrder == Order_YUV && fmt.bigEndian)
      return AV_PIX_FMT_YUV422P12BE;
    if (fmt.bitsPerSample == 12 && fmt.planeOrder == Order_YUV && !fmt.bigEndian)
      return AV_PIX_FMT_YUV422P12LE;
    if (fmt.bitsPerSample == 14 && fmt.planeOrder == Order_YUV && fmt.bigEndian)
      return AV_PIX_FMT_YUV422P14BE;
    if (fmt.bitsPerSample == 14 && fmt.planeOrder == Order_YUV && !fmt.bigEndian)
      return AV_PIX_FMT_YUV422P14LE;
  }
  if (fmt.subsampling == YUV_444)
  {
    if (fmt.bitsPerSample == 8 && fmt.planeOrder == Order_YUV && !fmt.bigEndian)
      return AV_PIX_FMT_YUV444P;
    if (fmt.bitsPerSample == 16 && fmt.planeOrder == Order_YUV && !fmt.bigEndian)
      return AV_PIX_FMT_YUV444P16LE;
    if (fmt.bitsPerSample == 16 && fmt.planeOrder == Order_YUV && fmt.bigEndian)
      return AV_PIX_FMT_YUV444P16BE;
    if (fmt.bitsPerSample == 9 && fmt.planeOrder == Order_YUV && fmt.bigEndian)
      return AV_PIX_FMT_YUV444P9BE;
    if (fmt.bitsPerSample == 9 && fmt.planeOrder == Order_YUV && !fmt.bigEndian)
      return AV_PIX_FMT_YUV444P9LE;
    if (fmt.bitsPerSample == 10 && fmt.planeOrder == Order_YUV && fmt.bigEndian)
      return AV_PIX_FMT_YUV444P10BE;
    if (fmt.bitsPerSample == 10 && fmt.planeOrder == Order_YUV && !fmt.bigEndian)
      return AV_PIX_FMT_YUV444P10LE;
    if (fmt.bitsPerSample == 12 && fmt.planeOrder == Order_YUV && fmt.bigEndian)
      return AV_PIX_FMT_YUV444P12BE;
    if (fmt.bitsPerSample == 12 && fmt.planeOrder == Order_YUV && !fmt.bigEndian)
      return AV_PIX_FMT_YUV444P12LE;
    if (fmt.bitsPerSample == 14 && fmt.planeOrder == Order_YUV && fmt.bigEndian)
      return AV_PIX_FMT_YUV444P14BE;
    if (fmt.bitsPerSample == 14 && fmt.planeOrder == Order_YUV && !fmt.bigEndian)
      return AV_PIX_FMT_YUV444P14LE;
  }

  return AV_PIX_FMT_NONE;
}

void AVFormatContextWrapper::update()
{
  if (ctx == nullptr)
    return;

  streams.clear();
  
  // Copy values from the source pointer
  if (libVer.avformat == 56)
  {
    AVFormatContext_56 *src = reinterpret_cast<AVFormatContext_56*>(ctx);
    ctx_flags = src->ctx_flags;
    nb_streams = src->nb_streams;
    for (unsigned int i=0; i<nb_streams; i++)
      streams.append(AVStreamWrapper(src->streams[i], libVer));
    filename = QString(src->filename);
    start_time = src->start_time;
    duration = src->duration;
    bit_rate = src->bit_rate;
    packet_size = src->packet_size;
    max_delay = src->max_delay;
    flags = src->flags;
    probesize = src->probesize;
    max_analyze_duration = src->max_analyze_duration;
    key = QString::fromLatin1((const char*)src->key, src->keylen);
    nb_programs = src->nb_programs;
    video_codec_id = src->video_codec_id;
    audio_codec_id = src->audio_codec_id;
    subtitle_codec_id = src->subtitle_codec_id;
    max_index_size = src->max_index_size;
    max_picture_buffer = src->max_picture_buffer;
    nb_chapters = src->nb_chapters;
    metadata = AVDictionaryWrapper(src->metadata);

    iformat = AVInputFormatWrapper(src->iformat, libVer);
  }
  else if (libVer.avformat == 57)
  {
    AVFormatContext_57 *src = reinterpret_cast<AVFormatContext_57*>(ctx);
    ctx_flags = src->ctx_flags;
    nb_streams = src->nb_streams;
    for (unsigned int i=0; i<nb_streams; i++)
      streams.append(AVStreamWrapper(src->streams[i], libVer));
    filename = QString(src->filename);
    start_time = src->start_time;
    duration = src->duration;
    bit_rate = src->bit_rate;
    packet_size = src->packet_size;
    max_delay = src->max_delay;
    flags = src->flags;
    probesize = src->probesize;
    max_analyze_duration = src->max_analyze_duration;
    key = QString::fromLatin1((const char*)src->key, src->keylen);
    nb_programs = src->nb_programs;
    video_codec_id = src->video_codec_id;
    audio_codec_id = src->audio_codec_id;
    subtitle_codec_id = src->subtitle_codec_id;
    max_index_size = src->max_index_size;
    max_picture_buffer = src->max_picture_buffer;
    nb_chapters = src->nb_chapters;
    metadata = AVDictionaryWrapper(src->metadata);

    iformat = AVInputFormatWrapper(src->iformat, libVer);
  }
  else
    assert(false);
}

int AVFormatContextWrapper::read_frame(FFmpegVersionHandler &ff, AVPacketWrapper &pkt)
{
  if (ctx == nullptr || !pkt)
    return -1;

  return ff.lib.av_read_frame(ctx, pkt.get_packet());
}

void AVCodecWrapper::update()
{
  if (codec == nullptr)
    return;

  if (libVer.avcodec == 56 || libVer.avcodec == 57)
  {
    AVCodec_56_57 *src = reinterpret_cast<AVCodec_56_57*>(codec);
    name = QString(src->name);
    long_name = QString(src->long_name);
    type = src->type;
    id = src->id;
    capabilities = src->capabilities;
    if (src->supported_framerates)
    {
      int i = 0;
      AVRational r = src->supported_framerates[i++];
      while (r.den != 0 && r.num != 0)
      {
        // Add and get the next one
        supported_framerates.append(r);
        r = src->supported_framerates[i++];
      }
    }
    if (src->pix_fmts)
    {
      int i = 0;
      AVPixelFormat f = src->pix_fmts[i++];
      while (f != -1)
      {
        // Add and get the next one
        pix_fmts.append(f);
        f = src->pix_fmts[i++];
      }
    }
    if (src->supported_samplerates)
    {
      int i = 0;
      int rate = src->supported_samplerates[i++];
      while (rate != 0)
      {
        supported_samplerates.append(rate);
        rate = src->supported_samplerates[i++];
      }
    }
    if (src->sample_fmts)
    {
      int i = 0;
      AVSampleFormat f = src->sample_fmts[i++];
      while (f != -1)
      {
        sample_fmts.append(f);
        f = src->sample_fmts[i++];
      }
    }
    if (src->channel_layouts)
    {
      int i = 0;
      uint64_t l = src->channel_layouts[i++];
      while (l != 0)
      {
        channel_layouts.append(l);
        l = src->channel_layouts[i++];
      }
    }
    max_lowres = src->max_lowres;
  }
  else
    assert(false);
}

void AVCodecContextWrapper::update()
{
  if (codec == nullptr)
    return;

  if (libVer.avcodec == 56)
  {
    AVCodecContext_56 *src = reinterpret_cast<AVCodecContext_56*>(codec);
    codec_type = src->codec_type;
    codec_name = QString(src->codec_name);
    codec_id = src->codec_id;
    codec_tag = src->codec_tag;
    stream_codec_tag = src->stream_codec_tag;
    bit_rate = src->bit_rate;
    bit_rate_tolerance = src->bit_rate_tolerance;
    global_quality = src->global_quality;
    compression_level = src->compression_level;
    flags = src->flags;
    flags2 = src->flags2;
    extradata = QByteArray((const char*)src->extradata, src->extradata_size);
    time_base = src->time_base;
    ticks_per_frame = src->ticks_per_frame;
    delay = src->delay;
    width = src->width;
    height = src->height;
    coded_width = src->coded_width;
    coded_height = src->coded_height;
    gop_size = src->gop_size;
    pix_fmt = src->pix_fmt;
    me_method = src->me_method;
    max_b_frames = src->max_b_frames;
    b_quant_factor = src->b_quant_factor;
    rc_strategy = src->rc_strategy;
    b_frame_strategy = src->b_frame_strategy;
    b_quant_offset = src->b_quant_offset;
    has_b_frames = src->has_b_frames;
    mpeg_quant = src->mpeg_quant;
    i_quant_factor = src->i_quant_factor;
    i_quant_offset = src->i_quant_offset;
    lumi_masking = src->lumi_masking;
    temporal_cplx_masking = src->temporal_cplx_masking;
    spatial_cplx_masking = src->spatial_cplx_masking;
    p_masking = src->p_masking;
    dark_masking = src->dark_masking;
    slice_count = src->slice_count;
    prediction_method = src->prediction_method;
    sample_aspect_ratio = src->sample_aspect_ratio;
    me_cmp = src->me_cmp;
    me_sub_cmp = src->me_sub_cmp;
    mb_cmp = src->mb_cmp;
    ildct_cmp = src->ildct_cmp;
    dia_size = src->dia_size;
    last_predictor_count = src->last_predictor_count;
    pre_me = src->pre_me;
    me_pre_cmp = src->me_pre_cmp;
    pre_dia_size = src->pre_dia_size;
    me_subpel_quality = src->me_subpel_quality;
    dtg_active_format = src->dtg_active_format;
    me_range = src->me_range;
    intra_quant_bias = src->intra_quant_bias;
    inter_quant_bias = src->inter_quant_bias;
    slice_flags = src->slice_flags;
    xvmc_acceleration = src->xvmc_acceleration;
    mb_decision = src->mb_decision;
    scenechange_threshold = src->scenechange_threshold;
    noise_reduction = src->noise_reduction;
    me_threshold = src->me_threshold;
    mb_threshold = src->mb_threshold;
    intra_dc_precision = src->intra_dc_precision;
    skip_top = src->skip_top;
    skip_bottom = src->skip_bottom;
    border_masking = src->border_masking;
    mb_lmin = src->mb_lmin;
    mb_lmax = src->mb_lmax;
    me_penalty_compensation = src->me_penalty_compensation;
    bidir_refine = src->bidir_refine;
    brd_scale = src->brd_scale;
    keyint_min = src->keyint_min;
    refs = src->refs;
    chromaoffset = src->chromaoffset;
    scenechange_factor = src->scenechange_factor;
    mv0_threshold = src->mv0_threshold;
    b_sensitivity = src->b_sensitivity;
    color_primaries = src->color_primaries;
    color_trc = src->color_trc;
    colorspace = src->colorspace;
    color_range = src->color_range;
    chroma_sample_location = src->chroma_sample_location;
  }
  else if (libVer.avcodec == 57)
  {
    AVCodecContext_57 *src = reinterpret_cast<AVCodecContext_57*>(codec);
    codec_type = src->codec_type;
    codec_name = QString(src->codec_name);
    codec_id = src->codec_id;
    codec_tag = src->codec_tag;
    stream_codec_tag = src->stream_codec_tag;
    bit_rate = src->bit_rate;
    bit_rate_tolerance = src->bit_rate_tolerance;
    global_quality = src->global_quality;
    compression_level = src->compression_level;
    flags = src->flags;
    flags2 = src->flags2;
    extradata = QByteArray((const char*)src->extradata, src->extradata_size);
    time_base = src->time_base;
    ticks_per_frame = src->ticks_per_frame;
    delay = src->delay;
    width = src->width;
    height = src->height;
    coded_width = src->coded_width;
    coded_height = src->coded_height;
    gop_size = src->gop_size;
    pix_fmt = src->pix_fmt;
    me_method = src->me_method;
    max_b_frames = src->max_b_frames;
    b_quant_factor = src->b_quant_factor;
    rc_strategy = src->rc_strategy;
    b_frame_strategy = src->b_frame_strategy;
    b_quant_offset = src->b_quant_offset;
    has_b_frames = src->has_b_frames;
    mpeg_quant = src->mpeg_quant;
    i_quant_factor = src->i_quant_factor;
    i_quant_offset = src->i_quant_offset;
    lumi_masking = src->lumi_masking;
    temporal_cplx_masking = src->temporal_cplx_masking;
    spatial_cplx_masking = src->spatial_cplx_masking;
    p_masking = src->p_masking;
    dark_masking = src->dark_masking;
    slice_count = src->slice_count;
    prediction_method = src->prediction_method;
    sample_aspect_ratio = src->sample_aspect_ratio;
    me_cmp = src->me_cmp;
    me_sub_cmp = src->me_sub_cmp;
    mb_cmp = src->mb_cmp;
    ildct_cmp = src->ildct_cmp;
    dia_size = src->dia_size;
    last_predictor_count = src->last_predictor_count;
    pre_me = src->pre_me;
    me_pre_cmp = src->me_pre_cmp;
    pre_dia_size = src->pre_dia_size;
    me_subpel_quality = src->me_subpel_quality;
    dtg_active_format = src->dtg_active_format;
    me_range = src->me_range;
    intra_quant_bias = src->intra_quant_bias;
    inter_quant_bias = src->inter_quant_bias;
    slice_flags = src->slice_flags;
    xvmc_acceleration = src->xvmc_acceleration;
    mb_decision = src->mb_decision;
    scenechange_threshold = src->scenechange_threshold;
    noise_reduction = src->noise_reduction;
    me_threshold = src->me_threshold;
    mb_threshold = src->mb_threshold;
    intra_dc_precision = src->intra_dc_precision;
    skip_top = src->skip_top;
    skip_bottom = src->skip_bottom;
    border_masking = src->border_masking;
    mb_lmin = src->mb_lmin;
    mb_lmax = src->mb_lmax;
    me_penalty_compensation = src->me_penalty_compensation;
    bidir_refine = src->bidir_refine;
    brd_scale = src->brd_scale;
    keyint_min = src->keyint_min;
    refs = src->refs;
    chromaoffset = src->chromaoffset;
    scenechange_factor = src->scenechange_factor;
    mv0_threshold = src->mv0_threshold;
    b_sensitivity = src->b_sensitivity;
    color_primaries = src->color_primaries;
    color_trc = src->color_trc;
    colorspace = src->colorspace;
    color_range = src->color_range;
    chroma_sample_location = src->chroma_sample_location;
  }
  else
    assert(false);
}

void AVStreamWrapper::update()
{
  if (str == nullptr)
    return;

  // Copy values from the source pointer
  if (libVer.avformat == 56)
  {
    AVStream_56 *src = reinterpret_cast<AVStream_56*>(str);
    index = src->index;
    id = src->id;
    codec = AVCodecContextWrapper(src->codec, libVer);
    time_base = src->time_base;
    start_time = src->start_time;
    duration = src->duration;
    nb_frames = src->nb_frames;
    disposition = src->nb_frames;
    discard = src->discard;
    sample_aspect_ratio = src->sample_aspect_ratio;
    avg_frame_rate = src->avg_frame_rate;
    nb_side_data = src->nb_side_data;
    event_flags = src->event_flags;
  }
  else if (libVer.avformat == 57)
  {
    AVStream_57 *src = reinterpret_cast<AVStream_57*>(str);
    index = src->index;
    id = src->id;
    codec = AVCodecContextWrapper(src->codec, libVer);
    time_base = src->time_base;
    start_time = src->start_time;
    duration = src->duration;
    nb_frames = src->nb_frames;
    disposition = src->nb_frames;
    discard = src->discard;
    sample_aspect_ratio = src->sample_aspect_ratio;
    avg_frame_rate = src->avg_frame_rate;
    nb_side_data = src->nb_side_data;
    event_flags = src->event_flags;
    codecpar = AVCodecParametersWrapper(src->codecpar, libVer);
  }
  else 
    assert(false);
}

AVMediaType AVStreamWrapper::getCodecType()
{
  update();
  if (libVer.avformat <= 56 || !codecpar)
    return codec.getCodecType();
  return codecpar.getCodecType();
}

AVCodecID AVStreamWrapper::getCodecID()
{
  update();
  if (str == nullptr)
    return AV_CODEC_ID_NONE;
  if (libVer.avformat <= 56 || !codecpar)
    return codec.getCodecID();
  return codecpar.getCodecID();
}

AVRational AVStreamWrapper::get_time_base()
{
  update();
  if (time_base.den == 0 || time_base.num == 0)
    // The stream time_base seems not to be set. Try the time_base in the codec.
    return codec.get_time_base();
  return time_base;
}

int AVStreamWrapper::get_frame_width()
{
  update();
  if (libVer.avformat <= 56 || !codecpar)
    return codec.get_width();
  return codecpar.get_width();
}

int AVStreamWrapper::get_frame_height()
{
  update();
  if (libVer.avformat <= 56 || !codecpar)
    return codec.get_height();
  return codecpar.get_height();
}

AVColorSpace AVStreamWrapper::get_colorspace()
{
  update();
  if (libVer.avformat <= 56 || !codecpar)
    return codec.get_colorspace();
  return codecpar.get_colorspace();
}

void AVCodecParametersWrapper::setClearValues()
{
  if (libVer.avformat == 57)
  {
    AVCodecParameters_57 *src = reinterpret_cast<AVCodecParameters_57*>(param);
    src->codec_type = AVMEDIA_TYPE_UNKNOWN;
    src->codec_id = AV_CODEC_ID_NONE;
    src->codec_tag = 0;
    src->extradata = nullptr;
    src->extradata_size = 0;
    src->format = 0;
    src->bit_rate = 0;
    src->bits_per_coded_sample = 0;
    src->bits_per_raw_sample = 0;
    src->profile = 0;
    src->level = 0;
    src->width = 0;
    src->height = 0;
    AVRational ratio;
    ratio.num = 1;
    ratio.den = 1;
    src->sample_aspect_ratio = ratio;
    src->field_order = AV_FIELD_UNKNOWN;
    src->color_range = AVCOL_RANGE_UNSPECIFIED;
    src->color_primaries = AVCOL_PRI_UNSPECIFIED;
    src->color_trc = AVCOL_TRC_UNSPECIFIED;
    src->color_space = AVCOL_SPC_UNSPECIFIED;
    src->chroma_location = AVCHROMA_LOC_UNSPECIFIED;
    src->video_delay = 0;
  }
  update();
}

void AVCodecParametersWrapper::setAVMediaType(AVMediaType type)
{
  if (libVer.avformat == 57)
  {
    AVCodecParameters_57 *src = reinterpret_cast<AVCodecParameters_57*>(param);
    src->codec_type = type;
    codec_type = type;
  }
}

void AVCodecParametersWrapper::setAVCodecID(AVCodecID id)
{
  if (libVer.avformat == 57)
  {
    AVCodecParameters_57 *src = reinterpret_cast<AVCodecParameters_57*>(param);
    src->codec_id = id;
    codec_id = id;
  }
}

void AVCodecParametersWrapper::setExtradata(QByteArray data)
{
  if (libVer.avformat == 57)
  {
    set_extradata = data;
    AVCodecParameters_57 *src = reinterpret_cast<AVCodecParameters_57*>(param);
    src->extradata = (uint8_t*)set_extradata.data();
    src->extradata_size = set_extradata.length();
    extradata = (uint8_t*)set_extradata.data();
    extradata_size = set_extradata.length();
  }
}

void AVCodecParametersWrapper::setSize(int w, int h)
{
  if (libVer.avformat == 57)
  {
    AVCodecParameters_57 *src = reinterpret_cast<AVCodecParameters_57*>(param);
    src->width = w;
    src->height = h;
    width = w;
    height = h;
  }
}

void AVCodecParametersWrapper::setAVPixelFormat(AVPixelFormat f)
{
  if (libVer.avformat == 57)
  {
    AVCodecParameters_57 *src = reinterpret_cast<AVCodecParameters_57*>(param);
    src->format = f;
    format = f;
  }
}

void AVCodecParametersWrapper::setProfileLevel(int p, int l)
{
  if (libVer.avformat == 57)
  {
    AVCodecParameters_57 *src = reinterpret_cast<AVCodecParameters_57*>(param);
    src->profile = p;
    src->level = l;
    profile = p;
    level = l;
  }
}

void AVCodecParametersWrapper::setSampleAspectRatio(int num, int den)
{
  if (libVer.avformat == 57)
  {
    AVCodecParameters_57 *src = reinterpret_cast<AVCodecParameters_57*>(param);
    AVRational ratio;
    ratio.num = num;
    ratio.den = den;
    src->sample_aspect_ratio = ratio;
    sample_aspect_ratio = ratio;
  }
}

void AVCodecParametersWrapper::update()
{
  if (param == nullptr)
    return;

  if (libVer.avformat == 56)
  {
    // This data structure does not exist in avformat major version 56.
    param = nullptr;
  }
  else if (libVer.avformat == 57)
  {
    AVCodecParameters_57 *src = reinterpret_cast<AVCodecParameters_57*>(param);
    
    codec_type = src->codec_type;
    codec_id = src->codec_id;
    codec_tag = src->codec_tag;
    extradata = src->extradata;
    extradata_size = src->extradata_size;
    format = src->format;
    bit_rate = src->bit_rate;
    bits_per_coded_sample = src->bits_per_coded_sample;
    bits_per_raw_sample = src->bits_per_raw_sample;
    profile = src->profile;
    level = src->level;
    width = src->width;
    height = src->height;
    sample_aspect_ratio = src->sample_aspect_ratio;
    field_order = src->field_order;
    color_range = src->color_range;
    color_primaries = src->color_primaries;
    color_trc = src->color_trc;
    color_space = src->color_space;
    chroma_location = src->chroma_location;
    video_delay = src->video_delay;
  }
  else
    assert(false);
}

void AVInputFormatWrapper::update()
{
  if (fmt == nullptr)
    return;

  if (libVer.avformat == 56 || libVer.avformat == 57)
  {
    AVInputFormat_56_57 *src = reinterpret_cast<AVInputFormat_56_57*>(fmt);
    name = QString(src->name);
    long_name = QString(src->long_name);
    flags = src->flags;
    extensions = QString(src->extensions);
    mime_type = QString(src->mime_type);
  }
}

void AVFormatContextWrapper::avformat_close_input(FFmpegVersionHandler &ver)
{
  if (ctx != nullptr)
    ver.lib.avformat_close_input(&ctx);
}

int FFmpegVersionHandler::getLibVersionUtil(FFmpegVersions ver)
{
  switch (ver)
  {
  case FFMpegVersion_54_56_56_1:
    return 54;
  case FFMpegVersion_55_57_57_2:
    return 55;
  default:
    assert(false);
  }
  return -1;
}

int FFmpegVersionHandler::getLibVersionCodec(FFmpegVersions ver)
{
  switch (ver)
  {
  case FFMpegVersion_54_56_56_1:
    return 56;
  case FFMpegVersion_55_57_57_2:
    return 57;
  default:
    assert(false);
  }
  return -1;
}

int FFmpegVersionHandler::getLibVersionFormat(FFmpegVersions ver)
{
  switch (ver)
  {
  case FFMpegVersion_54_56_56_1:
    return 56;
  case FFMpegVersion_55_57_57_2:
    return 57;
  default:
    assert(false);
  }
  return -1;
}

int FFmpegVersionHandler::getLibVersionSwresample(FFmpegVersions ver)
{
  switch (ver)
  {
  case FFMpegVersion_54_56_56_1:
    return 1;
  case FFMpegVersion_55_57_57_2:
    return 2;
  default:
    assert(false);
  }
  return -1;
}

//bool FFmpegVersionHandler::AVCodecContextCopyParameters(AVCodecContext *srcCtx, AVCodecContext *dstCtx)
//{
//  if (libVersion.avcodec == 56)
//  {
//    AVCodecContext_56 *dst = reinterpret_cast<AVCodecContext_56*>(dstCtx);
//    AVCodecContext_56 *src = reinterpret_cast<AVCodecContext_56*>(srcCtx);
//
//    dst->codec_type            = src->codec_type;
//    dst->codec_id              = src->codec_id;
//    dst->codec_tag             = src->codec_tag;
//    dst->bit_rate              = src->bit_rate;
//
//    // We don't parse these ...
//    //decCtx->bits_per_coded_sample = ctx->bits_per_coded_sample;
//    //decCtx->bits_per_raw_sample   = ctx->bits_per_raw_sample;
//    //decCtx->profile               = ctx->profile;
//    //decCtx->level                 = ctx->level;
//
//    if (src->codec_type == AVMEDIA_TYPE_VIDEO)
//    {
//      dst->pix_fmt                = src->pix_fmt;
//      dst->width                  = src->width;
//      dst->height                 = src->height;
//      //dst->field_order            = src->field_order;
//      dst->color_range            = src->color_range;
//      dst->color_primaries        = src->color_primaries;
//      dst->color_trc              = src->color_trc;
//      dst->colorspace             = src->colorspace;
//      dst->chroma_sample_location = src->chroma_sample_location;
//      dst->sample_aspect_ratio    = src->sample_aspect_ratio;
//      dst->has_b_frames           = src->has_b_frames;
//    }
//
//    // Extradata
//    if (src->extradata_size != 0 && dst->extradata_size == 0)
//    {
//      assert(dst->extradata == nullptr);
//      dst->extradata = (uint8_t*)lib.av_mallocz(src->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
//      if (dst->extradata == nullptr)
//        return false;
//      memcpy(dst->extradata, src->extradata, src->extradata_size);
//      dst->extradata_size = src->extradata_size;
//    }
//  }
//  else if (libVersion.avcodec == 57)
//  {
//    AVCodecContext_57 *dst = reinterpret_cast<AVCodecContext_57*>(dstCtx);
//    AVCodecContext_57 *src = reinterpret_cast<AVCodecContext_57*>(srcCtx);
//
//    dst->codec_type            = src->codec_type;
//    dst->codec_id              = src->codec_id;
//    dst->codec_tag             = src->codec_tag;
//    dst->bit_rate              = src->bit_rate;
//
//    // We don't parse these ...
//    //decCtx->bits_per_coded_sample = ctx->bits_per_coded_sample;
//    //decCtx->bits_per_raw_sample   = ctx->bits_per_raw_sample;
//    //decCtx->profile               = ctx->profile;
//    //decCtx->level                 = ctx->level;
//
//    if (src->codec_type == AVMEDIA_TYPE_VIDEO)
//    {
//      dst->pix_fmt                = src->pix_fmt;
//      dst->width                  = src->width;
//      dst->height                 = src->height;
//      //dst->field_order            = src->field_order;
//      dst->color_range            = src->color_range;
//      dst->color_primaries        = src->color_primaries;
//      dst->color_trc              = src->color_trc;
//      dst->colorspace             = src->colorspace;
//      dst->chroma_sample_location = src->chroma_sample_location;
//      dst->sample_aspect_ratio    = src->sample_aspect_ratio;
//      dst->has_b_frames           = src->has_b_frames;
//    }
//
//    // Extradata
//    if (src->extradata_size != 0 && dst->extradata_size == 0)
//    {
//      assert(dst->extradata == nullptr);
//      dst->extradata = (uint8_t*)lib.av_mallocz(src->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
//      if (dst->extradata == nullptr)
//        return false;
//      memcpy(dst->extradata, src->extradata, src->extradata_size);
//      dst->extradata_size = src->extradata_size;
//    }
//  }
//  else
//    assert(false);
//  return true;
//}
//

QString FFmpegVersionHandler::getLibVersionString() const
{
  QString s;
  s += QString("avUtil %1.%2.%3 ").arg(libVersion.avutil).arg(libVersion.avutil_minor).arg(libVersion.avutil_micro);
  s += QString("avFormat %1.%2.%3 ").arg(libVersion.avformat).arg(libVersion.avformat_minor).arg(libVersion.avformat_micro);
  s += QString("avCodec %1.%2.%3 ").arg(libVersion.avcodec).arg(libVersion.avcodec_minor).arg(libVersion.avformat_micro);
  s += QString("swresample %1.%2.%3").arg(libVersion.swresample).arg(libVersion.swresample_minor).arg(libVersion.swresample_micro);

  return s;
}

bool FFmpegVersionHandler::configureDecoder(AVCodecContextWrapper &decCtx, AVCodecParametersWrapper &codecpar)
{
  if (lib.newParametersAPIAvailable)
  {
    // Use the new avcodec_parameters_to_context function.
    AVCodecParameters *origin_par = codecpar.getCodecParameters();
    if (!origin_par)
      return false;
    int ret = lib.avcodec_parameters_to_context(decCtx.get_codec(), origin_par);
    if (ret < 0)
      return setError(QString("Could not copy codec parameters (avcodec_parameters_to_context). Return code %1.").arg(ret));
  }
  else
  {
    // TODO: Is this even necessary / what is really happening here?

    // The new parameters API is not available. Perform what the function would do.
    // This is equal to the implementation of avcodec_parameters_to_context.
    // AVCodecContext *ctxSrc = videoStream.getCodec().get_codec();
    // int ret = lib.AVCodecContextCopyParameters(ctxSrc, decCtx.get_codec());
    // return setOpeningError(QStringLiteral("Could not copy decoder parameters from stream decoder."));
  }
  return true;
}

int FFmpegVersionHandler::pushPacketToDecoder(AVCodecContextWrapper & decCtx, AVPacketWrapper & pkt)
{
  if (!pkt)
    return lib.avcodec_send_packet(decCtx.get_codec(), nullptr);
  else
    return lib.avcodec_send_packet(decCtx.get_codec(), pkt.get_packet());
}

int FFmpegVersionHandler::getFrameFromDecoder(AVCodecContextWrapper & decCtx, AVFrameWrapper &frame)
{
  return lib.avcodec_receive_frame(decCtx.get_codec(), frame.get_frame());
}

void AVFrameWrapper::update()
{
  if (frame == nullptr)
    return;

  if (libVer.avutil == 54)
  {
    AVFrame_54 *src = reinterpret_cast<AVFrame_54*>(frame);
    for(int i=0; i<AV_NUM_DATA_POINTERS; i++)
    {
      data[i] = src->data[i];
      linesize[i] = src->linesize[i];
    }
    width = src->width;
    height = src->height;
    nb_samples = src->nb_samples;
    format = src->format;
    key_frame = src->key_frame;
    pict_type = src->pict_type;
    sample_aspect_ratio = src->sample_aspect_ratio;
    pts = src->pts;
    pkt_pts = src->pkt_pts;
    pkt_dts = src->pkt_dts;
    coded_picture_number = src->coded_picture_number;
    display_picture_number = src->display_picture_number;
    quality = src->quality;
  }
  else if (libVer.avutil == 55)
  {
    AVFrame_55 *src = reinterpret_cast<AVFrame_55*>(frame);
    for(int i=0; i<AV_NUM_DATA_POINTERS; i++)
    {
      data[i] = src->data[i];
      linesize[i] = src->linesize[i];
    }
    width = src->width;
    height = src->height;
    nb_samples = src->nb_samples;
    format = src->format;
    key_frame = src->key_frame;
    pict_type = src->pict_type;
    sample_aspect_ratio = src->sample_aspect_ratio;
    pts = src->pts;
    pkt_pts = src->pkt_pts;
    pkt_dts = src->pkt_dts;
    coded_picture_number = src->coded_picture_number;
    display_picture_number = src->display_picture_number;
    quality = src->quality;
  }
  else
    assert(false);
}

void AVFrameWrapper::allocate_frame(FFmpegVersionHandler &ff) 
{ 
  assert(frame == nullptr);
  libVer = ff.libVersion;
  frame = ff.lib.av_frame_alloc();
}

void AVFrameWrapper::free_frame(FFmpegVersionHandler &ff) 
{ 
  ff.lib.av_frame_free(&frame); 
  frame = nullptr;
}

AVPacketWrapper::~AVPacketWrapper()
{
}

void AVPacketWrapper::allocate_paket(FFmpegVersionHandler &ff)
{
  assert(pkt == nullptr);
  libVer = ff.libVersion;

  if (libVer.avcodec == 56)
  {
    AVPacket_56 *p = new AVPacket_56;
    p->data = nullptr;
    p->size = 0;
    pkt = reinterpret_cast<AVPacket*>(p);
  }
  else if (libVer.avcodec == 57)
  {
    AVPacket_57 *p = new AVPacket_57;
    p->data = nullptr;
    p->size = 0;
    pkt = reinterpret_cast<AVPacket*>(p);
  }
  else
    assert(false);

  ff.lib.av_init_packet(pkt);
  update();
}

void AVPacketWrapper::unref_packet(FFmpegVersionHandler &ff)
{
  ff.lib.av_packet_unref(pkt);
}

void AVPacketWrapper::free_packet()
{
  if (libVer.avcodec == 56)
  {
    AVPacket_56 *p = reinterpret_cast<AVPacket_56*>(pkt);
    delete p;
  }
  else if (libVer.avcodec == 57)
  {
    AVPacket_57 *p = reinterpret_cast<AVPacket_57*>(pkt);
    delete p;
  }
  else
    assert(false);
  
  pkt = nullptr;
}

void AVPacketWrapper::set_data(QByteArray &set_data)
{
  if (libVer.avcodec == 56)
  {
    AVPacket_56 *src = reinterpret_cast<AVPacket_56*>(pkt);
    src->data = (uint8_t*)set_data.data();
    src->size = set_data.size();
    data = src->data;
    size = src->size;
  }
  else if (libVer.avcodec == 57)
  {
    AVPacket_57 *src = reinterpret_cast<AVPacket_57*>(pkt);
    src->data = (uint8_t*)set_data.data();
    src->size = set_data.size();
    data = src->data;
    size = src->size;
  }
  else
    assert(false);
}

void AVPacketWrapper::set_pts(int64_t p)
{
  if (libVer.avcodec == 56)
  {
    AVPacket_56 *src = reinterpret_cast<AVPacket_56*>(pkt);
    src->pts = p;
    pts = p;
  }
  else if (libVer.avcodec == 57)
  {
    AVPacket_57 *src = reinterpret_cast<AVPacket_57*>(pkt);
    src->pts = p;
    pts = p;
  }
  else
    assert(false);
}

void AVPacketWrapper::set_dts(int64_t d)
{
  if (libVer.avcodec == 56)
  {
    AVPacket_56 *src = reinterpret_cast<AVPacket_56*>(pkt);
    src->dts = d;
    dts = d;
  }
  else if (libVer.avcodec == 57)
  {
    AVPacket_57 *src = reinterpret_cast<AVPacket_57*>(pkt);
    src->dts = d;
    dts = d;
  }
  else
    assert(false);
}

packetDataFormat_t AVPacketWrapper::guessDataFormatFromData()
{
  // AVPacket data can be in one of two formats:
  // 1: The raw annexB format with start codes (0x00000001 or 0x000001)
  // 2: ISO/IEC 14496-15 mp4 format: The first 4 bytes determine the size of the NAL unit followed by the payload
  // We will try to guess the format of the data from the data in this AVPacket.
  // This should always work unless a format is used which we did not encounter so far (which is not listed above)
  // Also I think this should be identical for all packets in a bitstream.
  QByteArray avpacketData = QByteArray::fromRawData((const char*)(get_data()), get_data_size());
  if (avpacketData.at(0) == (char)0 && avpacketData.at(1) == (char)0 && avpacketData.at(2) == (char)0 && avpacketData.at(3) == (char)1)
    // A package length of 1 is not possible so this must be the raw NAL format.
    return packetFormatRawNAL;
  // Check the ISO mp4 format: Parse the whole data and check if the size bytes per Unit are correct.
  int posInData = 0;
  bool isMP4Format = true;
  while (posInData + 4 <= avpacketData.length())
  {
    QByteArray firstBytes = avpacketData.mid(posInData, 4);

    int size = (unsigned char)firstBytes.at(3);
    size += (unsigned char)firstBytes.at(2) << 8;
    size += (unsigned char)firstBytes.at(1) << 16;
    size += (unsigned char)firstBytes.at(0) << 24;
    posInData += 4;

    if (size < 0)
    {
      // The int did overflow. This means that the NAL unit is > 2GB in size. This is probably an error
      isMP4Format = false;
      break;
    }
    if (posInData + size > avpacketData.length())
    {
      // Not enough data in the input array to read NAL unit.
      isMP4Format = false;
      break;
    }
    posInData += size;
  }
  if (isMP4Format)
    return packetFormatMP4;
  if (avpacketData.at(0) == (char)0 && avpacketData.at(1) == (char)0 && avpacketData.at(2) == (char)1)
    // There is a 3 byte start code and this is not a mp4 formatted file. Looks like a raw NAL stream
    return packetFormatRawNAL;
  return packetFormatUnknown;
}

void AVPacketWrapper::update()
{
  if (libVer.avcodec == 56)
  {
    AVPacket_56 *src = reinterpret_cast<AVPacket_56*>(pkt);

    buf = src->buf;
    pts = src->pts;
    dts = src->dts;
    data = src->data;
    size = src->size;
    stream_index = src->stream_index;
    flags = src->flags;
    side_data = src->side_data;
    side_data_elems = src->side_data_elems;
    duration = src->duration;
    pos = src->pos;
    convergence_duration = src->convergence_duration;
  }
  else if (libVer.avcodec == 57)
  {
    AVPacket_57 *src = reinterpret_cast<AVPacket_57*>(pkt);

    buf = src->buf;
    pts = src->pts;
    dts = src->dts;
    data = src->data;
    size = src->size;
    stream_index = src->stream_index;
    flags = src->flags;
    side_data = src->side_data;
    side_data_elems = src->side_data_elems;
    duration = src->duration;
    pos = src->pos;
    convergence_duration = src->convergence_duration;
  }
  else
    assert(false);
}

int AVFrameSideDataWrapper::get_number_motion_vectors()
{
  update();

  if (type != AV_FRAME_DATA_MOTION_VECTORS)
    return -1;

  if (libVer.avutil == 54)
    return size / sizeof(AVMotionVector_54);
  else if (libVer.avutil == 55)
    return size / sizeof(AVMotionVector_55);
  else
    return -1;
}

AVMotionVectorWrapper AVFrameSideDataWrapper::get_motion_vector(int idx)
{
  update();

  if (libVer.avutil == 54)
  {
    AVMotionVector_54 *src = reinterpret_cast<AVMotionVector_54*>(data);
    AVMotionVector *v = reinterpret_cast<AVMotionVector*>(src + idx);
    return AVMotionVectorWrapper(v, libVer);
  }
  else if (libVer.avutil == 55)
  {
    AVMotionVector_55 *src = reinterpret_cast<AVMotionVector_55*>(data);
    AVMotionVector *v = reinterpret_cast<AVMotionVector*>(src + idx);
    return AVMotionVectorWrapper(v, libVer);
  }
  else
    assert(false);

  return AVMotionVectorWrapper();
}

void AVFrameSideDataWrapper::update()
{
  if (sideData == nullptr)
    return;

  if (libVer.avutil == 54 || libVer.avutil == 55)
  {
    AVFrameSideData_54_55 *src = reinterpret_cast<AVFrameSideData_54_55*>(sideData);
    type = src->type;
    data = src->data;
    size = src->size;
    metadata = src->metadata;
    buf = src->buf;
  }
  else
    assert(false);
}

void AVMotionVectorWrapper::update()
{
  if (libVer.avutil == 54)
  {
    AVMotionVector_54 *src = reinterpret_cast<AVMotionVector_54*>(vec);
    source = src->source;
    w = src->w;
    h = src->h;
    src_x = src->src_x;
    src_y = src->src_y;
    dst_x = src->dst_x;
    dst_y = src->dst_y;
    flags = src->flags;
    motion_x = -1;
    motion_y = -1;
    motion_scale = -1;
  }
  else if (libVer.avutil == 55)
  {
    AVMotionVector_55 *src = reinterpret_cast<AVMotionVector_55*>(vec);
    source = src->source;
    w = src->w;
    h = src->h;
    src_x = src->src_x;
    src_y = src->src_y;
    dst_x = src->dst_x;
    dst_y = src->dst_y;
    flags = src->flags;
    motion_x = src->motion_x;
    motion_y = src->motion_y;
    motion_scale = src->motion_scale;
  }
  else
    assert(false);
}
