/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2018 Igor Zinken - http://www.igorski.nl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "audiobuffer.h"
#include <algorithm>
#include <string.h>

AudioBuffer::AudioBuffer( int aAmountOfChannels, int aBufferSize )
{
    loopeable        = false;
    amountOfChannels = aAmountOfChannels;
    bufferSize       = aBufferSize;

    // create silent buffers for each channel

    _buffers = new std::vector<float*>( amountOfChannels );

    // fill buffers with silence

    for ( int i = 0; i < amountOfChannels; ++i ) {
        _buffers->at( i ) = new float[ aBufferSize ];
        memset( _buffers->at( i ), 0, aBufferSize * sizeof( float )); // zero bits should equal 0.f
    }
}

AudioBuffer::~AudioBuffer()
{
    while ( !_buffers->empty()) {
        delete[] _buffers->back(), _buffers->pop_back();
    }
    delete _buffers;
}

/* public methods */

float* AudioBuffer::getBufferForChannel( int aChannelNum )
{
    return _buffers->at( aChannelNum );
}

int AudioBuffer::mergeBuffers( AudioBuffer* aBuffer, int aReadOffset, int aWriteOffset, float aMixVolume )
{
    if ( aBuffer == 0 || aWriteOffset >= bufferSize )
        return 0;

    int sourceLength     = aBuffer->bufferSize;
    int maxSourceChannel = aBuffer->amountOfChannels - 1;
    int writeLength      = bufferSize;
    int writtenSamples   = 0;

    // keep writes within the bounds of this buffer

    if (( aWriteOffset + writeLength ) >= bufferSize )
        writeLength = bufferSize - aWriteOffset;

    int maxWriteOffset = aWriteOffset + writeLength;
    int c;

    for ( c = 0; c < amountOfChannels; ++c )
    {
        if ( c > maxSourceChannel )
            break;

        float* srcBuffer    = aBuffer->getBufferForChannel( c );
        float* targetBuffer = getBufferForChannel( c );

        for ( int i = aWriteOffset, r = aReadOffset; i < maxWriteOffset; ++i, ++r )
        {
            if ( r >= sourceLength )
            {
                if ( aBuffer->loopeable )
                    r = 0;
                else
                    break;
            }
            targetBuffer[ i ] += ( srcBuffer[ r ] * aMixVolume );
            ++writtenSamples;
        }
    }
    // return the amount of samples written (per buffer)
    return ( c == 0 ) ? writtenSamples : writtenSamples / c;
}

/**
 * fills the buffers with silence
 * clearing their previous contents
 */
void AudioBuffer::silenceBuffers()
{
    // use mem set to quickly erase existing buffer contents, zero bits should equal 0.f
    for ( int i = 0; i < amountOfChannels; ++i )
        memset( getBufferForChannel( i ), 0, bufferSize * sizeof( float ));
}

void AudioBuffer::adjustBufferVolumes( float amp )
{
    for ( int i = 0; i < amountOfChannels; ++i )
    {
        float* buffer = getBufferForChannel( i );

        for ( int j = 0; j < bufferSize; ++j )
            buffer[ j ] *= amp;
    }
}

bool AudioBuffer::isSilent()
{
    for ( int i = 0; i < amountOfChannels; ++i )
    {
        float* buffer = getBufferForChannel( i );
        for ( int j = 0; j < bufferSize; ++j )
        {
            if ( buffer[ j ] != 0.f )
                return false;
        }
    }
    return true;
}

AudioBuffer* AudioBuffer::clone()
{
    AudioBuffer* output = new AudioBuffer( amountOfChannels, bufferSize );

    for ( int i = 0; i < amountOfChannels; ++i )
    {
        float* sourceBuffer = getBufferForChannel( i );
        float* targetBuffer = output->getBufferForChannel( i );

        memcpy( targetBuffer, sourceBuffer, bufferSize * sizeof( float ));
    }
    return output;
}
