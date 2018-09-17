/*
 * Copyright (c) 2012-2015 Simon Schoenenberger
 * http://blipkit.audio
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef _BK_CONTEXT_H_
#define _BK_CONTEXT_H_

#include "BKObject.h"
#include "BKBuffer.h"
#include "BKClock.h"

/**
 * The context buffers the samples generated by units
 */

typedef struct BKUnit BKUnit;

typedef BKEnum (* BKGenerateCallback) (BKTime * nextTime, void * info);

enum
{
	BK_CLOCK_TYPE_EFFECT,
	BK_CLOCK_TYPE_BEAT,
};

enum
{
	BK_CONTEXT_FLAG_CLOCK_RESET = 1 << 0,
	BK_CONTEXT_FLAG_COPY_MASK   = 0,
};

struct BKContext
{
	BKObject object;
	BKUInt   flags;

	// settings
	BKUInt numChannels;
	BKUInt sampleRate;

	// run time
	BKFUInt20 deltaTime;
	BKTime    currentTime;

	// master clocks
	BKClock masterClock;

	// divider groups
	BKDividerGroup effectDividers;
	BKDividerGroup beatDividers;

	// linked clocks
	BKClock * firstClock;
	BKClock * lastClock;

	// linked units
	BKUnit * firstUnit;
	BKUnit * lastUnit;

	// channels
	BKBuffer * channels;
};

/**
 * Initialize context with number of channels and sample rate
 * `numChannels` may be between 1 and BK_MAX_CHANNELS
 * `sampleRate` may be between BK_MIN_SAMPLE_RATE AND BK_MAX_SAMPLE_RATE
 *
 * Disposing with `BKDispose` detaches all tracks
 *
 * Error:
 * BK_ALLOCATION_ERROR if memory could not be allocated
 */
extern BKInt BKContextInit (BKContext * ctx, BKUInt numChannels, BKUInt sampleRate);

/**
 * Allocate context and initialize with `BKContextInit`
 */
extern BKInt BKContextAlloc (BKContext ** outCtx, BKUInt numChannels, BKUInt sampleRate);

/**
 * Set attribute
 *
 * BK_ARPEGGIO_DIVIDER
 *   Set divider value for arpeggio step for all attached tracks
 * BK_EFFECT_DIVIDER
 *   Set divider value for effect step for all attached tracks
 * BK_INSTRUMENT_DIVIDER
 *   Set divider value for instrument step for all attached tracks
 *
 * Errors:
 * BK_INVALID_ATTRIBUTE if attribute is unkown
 * BK_INVALID_VALUE if value is invalid for this attribute
 */
extern BKInt BKContextSetAttr (BKContext * ctx, BKEnum attr, BKInt value) BK_DEPRECATED_FUNC ("Use 'BKSetAttr' instead");

/**
 * Get attribute
 *
 * BK_SAMPLE_RATE
 * BK_NUM_CHANNELS
 *
 * Errors:
 * BK_INVALID_ATTRIBUTE if attribute is unkown
 */
extern BKInt BKContextGetAttr (BKContext const * ctx, BKEnum attr, BKInt * outValue) BK_DEPRECATED_FUNC ("Use 'BKGetAttr' instead");

/**
 * Set pointer
 *
 * BK_CLOCK_PERIOD
 *   Clock period of master clock
 *   `BKClockPeriod`
 *
 * Errors:
 * BK_INVALID_ATTRIBUTE if attribute is unkown
 * BK_INVALID_VALUE if pointer is invalid for this attribute
 */
extern BKInt BKContextSetPtr (BKContext * ctx, BKEnum attr, void * ptr) BK_DEPRECATED_FUNC ("Use 'BKSetPtr' instead");

/**
 * Get pointer
 *
 * BK_CLOCK_PERIOD
 *   ptrRef = `BKTime`
 * BK_TIME
 *   Get the current absolute time
 *   ptrRef = `BKTime`
 *
 * Errors:
 * BK_INVALID_ATTRIBUTE if attribute is unkown
 */
extern BKInt BKContextGetPtr (BKContext const * ctx, BKEnum attr, void * outPtr) BK_DEPRECATED_FUNC ("Use 'BKGetPtr' instead");

/**
 * Generate frames
 * Channels are interlaced in the form LRLRLR
 * `outFrames` must have enough space for size * (number of channels) frames
 *
 * No errors defined
 */
extern BKInt BKContextGenerate (BKContext * ctx, BKFrame outFrames [], BKUInt size);

/**
 * Generate frames to specified time
 * `write` is called every time new frames are available from the buffer
 * If `write` doesn't return 0 BK_INVALID_RETURN_VALUE is return
 *
 * Errors:
 * BK_INVALID_RETURN_VALUE if `write` doesn't return 0
 */
extern BKInt BKContextGenerateToTime (BKContext * ctx, BKTime endTime, BKInt (* write) (BKFrame inFrames [], BKUInt size, void * info), void * info);

/**
 * Run context to specific time
 */
extern BKInt BKContextRun (BKContext * ctx, BKFUInt20 endTime);

/**
 * End all units
 */
extern BKInt BKContextEnd (BKContext * ctx, BKFUInt20 endTime);

/**
 * Get maximum readable frames
 */
extern BKInt BKContextSize (BKContext const * ctx);

/**
 * Read from channels
 * Channels are interlaced in the form LRLRLR
 * `outFrames` must have enough space for size * (number of channels) frames
 * Get a maximum of `size` frames
 */
extern BKInt BKContextRead (BKContext * ctx, BKFrame outFrames [], BKUInt size);

/**
 * Reset all units, buffers and clocks
 */
extern void BKContextReset (BKContext * ctx);

/**
 * Attach divider to specific clock
 * `type` may be one of the following values: BK_CLOCK_TYPE_EFFECT, BK_CLOCK_TYPE_BEAT
 */
extern BKInt BKContextAttachDivider (BKContext * ctx, BKDivider * divider, BKEnum type);


#if BK_IS_64_BIT

BK_INLINE BKTime BKTimeFromSeconds (BKContext * ctx, double seconds)
{
	return (BKTime) ctx -> sampleRate * BK_FINT20_UNIT * seconds;
}

#else /* ! BK_IS_64_BIT */

BK_INLINE BKTime BKTimeFromSeconds (BKContext * ctx, double seconds)
{
	BKTime time;
	double samples = ctx -> sampleRate * seconds;

	time.time = samples;
	time.frac = (samples - time.time) * BK_FINT20_UNIT;

	return time;
}

#endif /* BK_IS_64_BIT */

#endif /* ! _BK_CONTEXT_H_ */
