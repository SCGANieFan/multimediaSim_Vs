
#ifndef WEBRTC_MODULES_AUDIO_CODING_NETEQ_TIME_STRETCH_H_
#define WEBRTC_MODULES_AUDIO_CODING_NETEQ_TIME_STRETCH_H_

#include <assert.h>
#include <string.h>  // memset, size_t

#include "constructormagic.h"
#include "audio_multi_vector.h"
#include "typedefs.h"
#include "ObjectStatic.h"
namespace webrtc {

// Forward declarations.
class BackgroundNoise;

// This is the base class for Accelerate and PreemptiveExpand. This class
// cannot be instantiated, but must be used through either of the derived
// classes.
class TimeStretch :public smf::ObjectStatic{
 public:
  enum ReturnCodes {
    kSuccess = 0,
    kSuccessLowEnergy = 1,
    kNoStretch = 2,
    kError = -1
  };

  TimeStretch(int sample_rate_hz, size_t num_channels)
      : sample_rate_hz_(sample_rate_hz),
        fs_mult_(sample_rate_hz / 8000),
        num_channels_(num_channels),
        master_channel_(0),  // First channel is master.
        max_input_value_(0) {
    assert(sample_rate_hz_ == 8000 ||
           sample_rate_hz_ == 16000 ||
           sample_rate_hz_ == 32000 ||
           sample_rate_hz_ == 48000);
    assert(num_channels_ > 0);
    assert(master_channel_ < num_channels_);
    memset(auto_correlation_, 0, sizeof(auto_correlation_));
  }

  virtual ~TimeStretch() {}

  // This method performs the processing common to both Accelerate and
  // PreemptiveExpand.
  ReturnCodes Process(const int16_t* input,
                      size_t in_sample,
                      bool fast_mode,
                      AudioMultiVector* output,
                      size_t* length_change_samples);

 protected:
  // Sets the parameters |best_correlation| and |peak_index| to suitable
  // values when the signal contains no active speech. This method must be
  // implemented by the sub-classes.
  virtual void SetParametersForPassiveSpeech(size_t input_length,
                                             int16_t* best_correlation,
                                             size_t* peak_index) const = 0;

  // Checks the criteria for performing the time-stretching operation and,
  // if possible, performs the time-stretching. This method must be implemented
  // by the sub-classes.
  virtual ReturnCodes CheckCriteriaAndStretch(
      const int16_t* input,
      size_t input_length,
      size_t peak_index,
      int16_t best_correlation,
      bool active_speech,
      bool fast_mode,
      AudioMultiVector* output) const = 0;

  // static const size_t kCorrelationLen = 30;//12.5ms in 30ms, 8ms in 20ms
  static const size_t kLogCorrelationLen = 6;  // >= log2(kCorrelationLen).
  static const size_t kMinLag = 10;//2.5ms
  static const size_t kMaxLag = 40;//10ms
    static const size_t kCorrelationLen = kMaxLag - kMinLag;//12.5ms in 30ms, 8ms in 20ms
  static const size_t kDownsampledLen = kCorrelationLen + kMaxLag;
  //static const int kCorrelationThreshold = 14746;  // 0.9 in Q14.
  static const int kCorrelationThreshold = 10000;  // 0.61 in Q14
  const int sample_rate_hz_;
  const int fs_mult_;  // Sample rate multiplier = sample_rate_hz_ / 8000.
  const size_t num_channels_;
  const size_t master_channel_;
  // const BackgroundNoise& background_noise_;
  int16_t max_input_value_;
  int16_t downsampled_input_[kDownsampledLen];
  // Adding 1 to the size of |auto_correlation_| because of how it is used
  // by the peak-detection algorithm.
  int16_t auto_correlation_[kCorrelationLen + 1];

 private:
  // Calculates the auto-correlation of |downsampled_input_| and writes the
  // result to |auto_correlation_|.
  void AutoCorrelation();

  // Performs a simple voice-activity detection based on the input parameters.
  bool SpeechDetection(int32_t vec1_energy, int32_t vec2_energy,
                       size_t peak_index, int scaling) const;

  RTC_DISALLOW_COPY_AND_ASSIGN(TimeStretch);
};

}  // namespace webrtc
#endif  // WEBRTC_MODULES_AUDIO_CODING_NETEQ_TIME_STRETCH_H_
