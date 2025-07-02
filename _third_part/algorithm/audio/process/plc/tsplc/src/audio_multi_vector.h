/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_AUDIO_CODING_NETEQ_AUDIO_MULTI_VECTOR_H_
#define WEBRTC_MODULES_AUDIO_CODING_NETEQ_AUDIO_MULTI_VECTOR_H_

#include <string.h>
#include <vector>
#include "constructormagic.h"
#include "audio_vector.h"
#include "typedefs.h"
#include "ObjectStatic.h"
using namespace smf;
namespace webrtc {

class AudioMultiVector : public smf::ObjectStatic{
    public:        
        explicit AudioMultiVector(size_t N);
        AudioMultiVector(size_t N, size_t initial_size);
        virtual ~AudioMultiVector();
        virtual void Clear();
        virtual void Zeros(size_t length);
        virtual void CopyTo(AudioMultiVector* copy_to) const;
        virtual void PushBackInterleaved(const int16_t* append_this, size_t length);
        virtual void PushBack(const AudioMultiVector& append_this);
        virtual void PushBackFromIndex(const AudioMultiVector& append_this, size_t index);
        virtual void PopFront(size_t length);
        virtual void PopBack(size_t length);
        virtual size_t ReadInterleaved(size_t length, int16_t* destination) const;
        virtual size_t ReadInterleavedFromIndex(size_t start_index, size_t length, int16_t* destination) const;
        virtual size_t ReadInterleavedFromEnd(size_t length, int16_t* destination) const;
        virtual void OverwriteAt(const AudioMultiVector& insert_this, size_t length, size_t position);
        virtual void CrossFade(const AudioMultiVector& append_this, size_t fade_length);
        virtual size_t Channels() const;
        virtual size_t Size() const;
        virtual void AssertSize(size_t required_size);
        virtual bool Empty() const;
        virtual void CopyChannel(size_t from_channel, size_t to_channel);
        const AudioVector& operator[](size_t index) const;
        AudioVector& operator[](size_t index);

    protected:
        //std::vector<AudioVector*> channels_;
        AudioVector* channels_[4] = { 0 };
        size_t num_channels_;

    private:
        RTC_DISALLOW_COPY_AND_ASSIGN(AudioMultiVector);
    };

}  // namespace webrtc
#endif  // WEBRTC_MODULES_AUDIO_CODING_NETEQ_AUDIO_MULTI_VECTOR_H_
