/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_AUDIO_CODING_NETEQ_AUDIO_VECTOR_H_
#define WEBRTC_MODULES_AUDIO_CODING_NETEQ_AUDIO_VECTOR_H_

#include <string.h>  // Access to size_t.

#include "constructormagic.h"
#include "scoped_ptr.h"
#include "typedefs.h"
#include "ObjectStatic.h"
using namespace smf;
namespace webrtc {

    class AudioVector : public smf::ObjectStatic{
    public:        
        AudioVector();
        explicit AudioVector(size_t initial_size);
        virtual ~AudioVector();        
        virtual void Clear();
        virtual void CopyTo(AudioVector* copy_to) const;
        virtual void PushFront(const AudioVector& prepend_this);
        virtual void PushFront(const int16_t* prepend_this, size_t length);
        virtual void PushBack(const AudioVector& append_this);
        virtual void PushBack(const int16_t* append_this, size_t length);
        virtual void PopFront(size_t length);
        virtual void PopBack(size_t length);
        virtual void Extend(size_t extra_length);
        virtual void InsertAt(const int16_t* insert_this, size_t length, size_t position);
        virtual void InsertZerosAt(size_t length, size_t position);
        virtual void OverwriteAt(const int16_t* insert_this, size_t length, size_t position);
        virtual void CrossFade(const AudioVector& append_this, size_t fade_length);
        virtual size_t Size() const;
        virtual bool Empty() const;
        const int16_t& operator[](size_t index) const;
        int16_t& operator[](size_t index);

    private:
        static const size_t kDefaultInitialSize = 10;
        void Reserve(size_t n);
        rtc::scoped_ptr<int16_t[]> array_;
        size_t first_free_ix_;
        size_t capacity_;
        RTC_DISALLOW_COPY_AND_ASSIGN(AudioVector);
    };

}  // namespace webrtc
#endif  // WEBRTC_MODULES_AUDIO_CODING_NETEQ_AUDIO_VECTOR_H_