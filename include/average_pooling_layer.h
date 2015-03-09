/*
    Copyright (c) 2013, Taiga Nomi
    All rights reserved.
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY 
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once
#include "util.h"
#include "partial_connected_layer.h"

namespace tiny_cnn {


template<typename N, typename Activation>
class average_pooling_layer : public partial_connected_layer<N, Activation> {
public:
    using Base = partial_connected_layer<N, Activation>;
    using Optimizer = typename Base::Optimizer;

    average_pooling_layer(size_t in_width, size_t in_height, size_t in_channels, size_t pooling_size)
    : partial_connected_layer<N, Activation>(
     in_width * in_height * in_channels, 
     in_width * in_height * in_channels / sqr(pooling_size), 
     in_channels, in_channels, 1.0 / sqr(pooling_size)),
     in_(in_width, in_height, in_channels), 
     out_(in_width/pooling_size, in_height/pooling_size, in_channels)
    {
        if ((in_width % pooling_size) || (in_height % pooling_size)) 
            throw nn_error("width/height must be multiples of pooling size");
        init_connection(pooling_size);
    }

private:
    void init_connection(size_t pooling_size) {
        for (size_t c = 0; c < in_.depth_; c++)
            for (size_t y = 0; y < in_.height_; y += pooling_size)
                for (size_t x = 0; x < in_.width_; x += pooling_size)
                    connect_kernel(pooling_size, x, y, c);


        for (size_t c = 0; c < in_.depth_; c++)
            for (size_t y = 0; y < out_.height_; y++)
                for (size_t x = 0; x < out_.width_; x++)
                    this->connect_bias(c, out_.get_index(x, y, c));
    }

    void connect_kernel(size_t pooling_size, size_t x, size_t y, size_t inc) {
        for (size_t dy = 0; dy < pooling_size; dy++)
            for (size_t dx = 0; dx < pooling_size; dx++)
                this->connect_weight(
                    in_.get_index(x + dx, y + dy, inc), 
                    out_.get_index(x / pooling_size, y / pooling_size, inc),
                    inc);
    }

    index3d in_;
    index3d out_;
};

} // namespace tiny_cnn
