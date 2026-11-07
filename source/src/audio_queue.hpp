// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <cstddef>
namespace lab {
// Buffers are submitted once in index order before this cursor is used.
// The driver may complete several at once, including both sides of wraparound.
template<std::size_t Count> class AudioQueue {
public:
    template<class Ready,class Submit> bool refill(Ready ready,Submit submit){
        for(std::size_t count=0;count<Count&&ready(next);++count){
            if(!submit(next))return false;
            next=(next+1)%Count;
        }
        return true;
    }
private:
    std::size_t next=0;
};
}
