/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/audio.h - Implementations for audio playback.   */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/audio.h"

#include <tux64/endian.h>
#include <tux64/math.h>
#include <tux64/memory.h>
#include <tux64/platform/mips/n64/boot.h>
#include <tux64/platform/mips/n64/mmio.h>
#include "tux64-boot/sync.h"
#include "tux64-boot/cache.h"
#include "tux64-boot/halt.h"

/*----------------------------------------------------------------------------*/
/* These are used to configure our generated waves.  The sample count should  */
/* be at least four, since the AI requires a minimum of 4 16-bit samples.  In */
/* addition, if it's not high enough, our square wave won't be very square    */
/* because the very low sample rate/count "rounds out" the rising/falling     */
/* edges.  Additionally, low sample counts combined with high sample rates    */
/* can cause an interrupt storm from the AI, which may deadlock the rest of   */
/* the bootloader.                                                            */
/*                                                                            */
/* We also define the "high" and "low" sample, which represents the high and  */
/* low sample of the square wave, as well as the baseline for silence.  These */
/* primarily control the amplitude of the waves.                              */
/*----------------------------------------------------------------------------*/
#define TUX64_BOOT_STAGE1_AUDIO_WAVE_SAMPLE_COUNT  64u
#define TUX64_BOOT_STAGE1_AUDIO_WAVE_SAMPLE_HI     0x1000u
#define TUX64_BOOT_STAGE1_AUDIO_WAVE_SAMPLE_LO     0x0000u

/* AI requires that the number of bytes for our sample is a multiple of 8, and */
/* non-zero.  since each sample is 2 bytes, our sample count must be aligned */
/* to 4 bytes, and non-zero. */
TUX64_ASSERT_STATIC((TUX64_BOOT_STAGE1_AUDIO_WAVE_SAMPLE_COUNT % 4u) == 0u);
TUX64_ASSERT_STATIC(TUX64_BOOT_STAGE1_AUDIO_WAVE_SAMPLE_COUNT != 0u);

#define TUX64_BOOT_STAGE1_AUDIO_WAVE_SAMPLE_COUNT_COMBINED\
   ((3u * TUX64_BOOT_STAGE1_AUDIO_WAVE_SAMPLE_COUNT) / 2u)

typedef Tux64UInt16 Tux64BootStage1AudioSample;

enum Tux64BootStage1AudioWaveType {
   TUX64_BOOT_STAGE1_AUDIO_WAVE_TYPE_SQUARE,
   TUX64_BOOT_STAGE1_AUDIO_WAVE_TYPE_SILENT
};

struct Tux64BootStage1AudioNote {
   enum Tux64BootStage1AudioWaveType type;
   Tux64UInt32 frequency;
   Tux64UInt32 repeat_count;
};

#define TUX64_BOOT_STAGE1_AUDIO_TONE_MAX_NOTES 5u

/*----------------------------------------------------------------------------*/
/* This stores notes in reverse order for efficiency.  That is, the last note */
/* in the array is the first one that gets played.                            */
/*----------------------------------------------------------------------------*/
struct Tux64BootStage1AudioTone {
   struct Tux64BootStage1AudioNote notes [TUX64_BOOT_STAGE1_AUDIO_TONE_MAX_NOTES];
   Tux64UInt8 notes_count;
};

#define TUX64_BOOT_STAGE1_AUDIO_NOTE_FREQUENCY(p_pitch)\
   TUX64_MATH_NEAREST_DIVIDE(\
      (p_pitch * TUX64_BOOT_STAGE1_AUDIO_WAVE_SAMPLE_COUNT),\
      2u\
   )
#define TUX64_BOOT_STAGE1_AUDIO_NOTE_REPEAT_COUNT(p_duration, p_pitch)\
   TUX64_MATH_NEAREST_DIVIDE(\
      (p_duration * p_pitch),\
      1000u\
   )
#define TUX64_BOOT_STAGE1_AUDIO_NOTE_RAW(p_type, p_frequency, p_repeat_count)\
   {\
      .type          = p_type,\
      .frequency     = TUX64_LITERAL_UINT32(p_frequency),\
      .repeat_count  = TUX64_LITERAL_UINT32(p_repeat_count)\
   }
#define TUX64_BOOT_STAGE1_AUDIO_NOTE(p_type, p_pitch, p_duration)\
   TUX64_BOOT_STAGE1_AUDIO_NOTE_RAW(\
      p_type,\
      TUX64_BOOT_STAGE1_AUDIO_NOTE_FREQUENCY(p_pitch), \
      TUX64_BOOT_STAGE1_AUDIO_NOTE_REPEAT_COUNT(p_duration, p_pitch)\
   )

/*----------------------------------------------------------------------------*/
/* These macros are used to define individual notes, intended for use in      */
/* defining notes in a tone.  'p_duration' is the length of the note, in      */
/* milliseconds, and 'p_pitch' is the frequency of the note, in hertz.        */
/* "Empty" notes should never be played, and are only required for padding    */
/* out unused note table entries.                                             */
/*----------------------------------------------------------------------------*/
#define TUX64_BOOT_STAGE1_AUDIO_NOTE_SQUARE(p_duration, p_pitch)\
   TUX64_BOOT_STAGE1_AUDIO_NOTE(\
      TUX64_BOOT_STAGE1_AUDIO_WAVE_TYPE_SQUARE,\
      p_pitch,\
      p_duration\
   )
#define TUX64_BOOT_STAGE1_AUDIO_NOTE_SILENT(p_duration)\
   TUX64_BOOT_STAGE1_AUDIO_NOTE(\
      TUX64_BOOT_STAGE1_AUDIO_WAVE_TYPE_SILENT,\
      10000u,\
      p_duration\
   )
#define TUX64_BOOT_STAGE1_AUDIO_NOTE_EMPTY\
   TUX64_BOOT_STAGE1_AUDIO_NOTE_RAW(\
      TUX64_BOOT_STAGE1_AUDIO_WAVE_TYPE_SILENT,\
      0u,\
      0u\
   )

static const struct Tux64BootStage1AudioTone
tux64_boot_stage1_audio_tones [TUX64_BOOT_STAGE1_AUDIO_SOUND_FIELD_COUNT] = {
   { /* startup */
      .notes = {
         TUX64_BOOT_STAGE1_AUDIO_NOTE_SQUARE(433u, 1315u),  /* E */
         TUX64_BOOT_STAGE1_AUDIO_NOTE_SQUARE(67u,   980u),  /* B */
         TUX64_BOOT_STAGE1_AUDIO_NOTE_EMPTY,
         TUX64_BOOT_STAGE1_AUDIO_NOTE_EMPTY,
         TUX64_BOOT_STAGE1_AUDIO_NOTE_EMPTY
      },
      .notes_count = TUX64_LITERAL_UINT8(2u)
   },
   { /* fatal error */
      .notes = {
         TUX64_BOOT_STAGE1_AUDIO_NOTE_SQUARE(125u, 346u),   /* F */
         TUX64_BOOT_STAGE1_AUDIO_NOTE_SILENT(75u),
         TUX64_BOOT_STAGE1_AUDIO_NOTE_SQUARE(125u, 346u),   /* F */
         TUX64_BOOT_STAGE1_AUDIO_NOTE_SILENT(75u),
         TUX64_BOOT_STAGE1_AUDIO_NOTE_SQUARE(125u, 346u)    /* F */
      },
      .notes_count = TUX64_LITERAL_UINT8(5u)
   }
};

/*----------------------------------------------------------------------------*/
/* This combines both our square wave and silent wave together, reusing the   */
/* low part of the square wave for the first half of the silent wave.  Also,  */
/* the reason for the silent wave is so we can use the AI as a timing source  */
/* while not making any sound.                                                */
/*----------------------------------------------------------------------------*/
struct Tux64BootStage1AudioContextWaves {
   Tux64BootStage1AudioSample square_silent [TUX64_BOOT_STAGE1_AUDIO_WAVE_SAMPLE_COUNT_COMBINED]
   __attribute__((aligned(8u))); /* aligned for AI DMA */
};

struct Tux64BootStage1AudioContext {
   struct Tux64BootStage1AudioContextWaves waves;
   struct Tux64BootSyncFence queue_fence;
   Tux64UInt32 vi_clock_rate;
   const struct Tux64BootStage1AudioNote * tone_notes;
   Tux64UInt32 note_repeats_remaining;
   Tux64UInt8 tone_notes_remaining;
};

static struct Tux64BootStage1AudioContext
tux64_boot_stage1_audio_context;

static Tux64UInt32
tux64_boot_stage1_audio_vi_clock_rate(
   enum Tux64PlatformMipsN64BootConsoleType console_type
) {
   Tux64UInt32 clock;

   switch (console_type) {
      case TUX64_PLATFORM_MIPS_N64_BOOT_CONSOLE_TYPE_PAL:
         clock = TUX64_LITERAL_UINT32(49656530u);
         break;

      case TUX64_PLATFORM_MIPS_N64_BOOT_CONSOLE_TYPE_NTSC:
         clock = TUX64_LITERAL_UINT32(48681818u);
         break;

      case TUX64_PLATFORM_MIPS_N64_BOOT_CONSOLE_TYPE_MPAL:
         clock = TUX64_LITERAL_UINT32(48628322u);
         break;

      case TUX64_PLATFORM_MIPS_N64_BOOT_CONSOLE_TYPE_IQUE:
         clock = TUX64_LITERAL_UINT32(48681818u);
         break;
   }

   return clock;
}

static Tux64UInt32
tux64_boot_stage1_audio_ai_dac_rate(
   Tux64UInt32 vi_clock_rate,
   Tux64UInt32 sample_rate
) {
   Tux64UInt32 dac_rate;

   /* libdragon does a rounding integer division (src/audio.c:audio_init()) */
   /* to compute the DAC rate.  that's probably a good idea, so we'll do that */
   /* as well. */
   dac_rate = tux64_math_nearest_divide_uint32(vi_clock_rate, sample_rate);

   return dac_rate - TUX64_LITERAL_UINT32(1u);
}

static Tux64UInt32
tux64_boot_stage1_audio_ai_bit_rate(
   Tux64UInt32 dac_rate
) {
   Tux64UInt32 bit_rate;

   bit_rate = dac_rate / TUX64_LITERAL_UINT32(66u);
   if (bit_rate > TUX64_LITERAL_UINT32(16u)) {
      bit_rate = TUX64_LITERAL_UINT32(16u);
   }

   return bit_rate - TUX64_LITERAL_UINT32(1u);
}

static void
tux64_boot_stage1_audio_ai_set_sample_rate(
   Tux64UInt32 sample_rate
) {
   Tux64UInt32 vi_clock_rate;
   Tux64UInt32 dac_rate;
   Tux64UInt32 bit_rate;

   vi_clock_rate = tux64_boot_stage1_audio_context.vi_clock_rate;

   dac_rate = tux64_boot_stage1_audio_ai_dac_rate(vi_clock_rate, sample_rate);
   bit_rate = tux64_boot_stage1_audio_ai_bit_rate(dac_rate);

   tux64_platform_mips_n64_mmio_registers_ai.dac_rate = dac_rate;
   tux64_platform_mips_n64_mmio_registers_ai.bit_rate = bit_rate;
   return;
}

static void
tux64_boot_stage1_audio_ai_halt(void) {
   tux64_platform_mips_n64_mmio_registers_ai.control  = TUX64_LITERAL_UINT32(0u);
   tux64_platform_mips_n64_mmio_registers_ai.dac_rate = TUX64_LITERAL_UINT32(0u);
   tux64_platform_mips_n64_mmio_registers_ai.bit_rate = TUX64_LITERAL_UINT32(0u);
   return;
}

static void
tux64_boot_stage1_audio_ai_queue_wave_data(
   const Tux64BootStage1AudioSample * data
) {
   Tux64UInt32 dram_addr;
   Tux64UInt32 length;

   dram_addr   = (Tux64UInt32)(Tux64UIntPtr)data;
   length      = TUX64_LITERAL_UINT32(
      TUX64_BOOT_STAGE1_AUDIO_WAVE_SAMPLE_COUNT
      * sizeof(Tux64BootStage1AudioSample)
   );

   tux64_platform_mips_n64_mmio_registers_ai.dram_addr   = dram_addr;
   tux64_platform_mips_n64_mmio_registers_ai.length      = length;
   tux64_platform_mips_n64_mmio_registers_ai.control     = TUX64_LITERAL_UINT32(1u);
   return;
}

static void
tux64_boot_stage1_audio_ai_queue_wave_square(void) {
   const struct Tux64BootStage1AudioContext * ctx;

   ctx = &tux64_boot_stage1_audio_context;

   tux64_boot_stage1_audio_ai_queue_wave_data(ctx->waves.square_silent);
   return;
}

static void
tux64_boot_stage1_audio_ai_queue_wave_silent(void) {
   const struct Tux64BootStage1AudioContext * ctx;
   Tux64UInt32 offset;

   ctx = &tux64_boot_stage1_audio_context;

   offset = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_AUDIO_WAVE_SAMPLE_COUNT / 2u);

   tux64_boot_stage1_audio_ai_queue_wave_data(&ctx->waves.square_silent[offset]);
   return;
}

static void
tux64_boot_stage1_audio_ai_queue_wave(
   enum Tux64BootStage1AudioWaveType wave
) {
   switch (wave) {
      case TUX64_BOOT_STAGE1_AUDIO_WAVE_TYPE_SQUARE:
         tux64_boot_stage1_audio_ai_queue_wave_square();
         break;
      case TUX64_BOOT_STAGE1_AUDIO_WAVE_TYPE_SILENT:
         tux64_boot_stage1_audio_ai_queue_wave_silent();
         break;
      default:
         TUX64_UNREACHABLE;
   }

   return;
}

static void
tux64_boot_stage1_audio_lock_hold(void) {
   struct Tux64BootStage1AudioContext * ctx;

   ctx = &tux64_boot_stage1_audio_context;

   /* we don't ever have to wait, so resetting the fence is good enough. */
   tux64_boot_sync_fence_initialize(&ctx->queue_fence);
   return;
}

static Tux64Boolean
tux64_boot_stage1_audio_lock_is_held(void) {
   struct Tux64BootStage1AudioContext * ctx;

   ctx = &tux64_boot_stage1_audio_context;

   return tux64_boot_sync_fence_is_signaled(&ctx->queue_fence) ?
      TUX64_BOOLEAN_FALSE :
      TUX64_BOOLEAN_TRUE;
}

static void
tux64_boot_stage1_audio_lock_release(void) {
   struct Tux64BootStage1AudioContext * ctx;

   ctx = &tux64_boot_stage1_audio_context;
   
   tux64_boot_sync_fence_signal(&ctx->queue_fence);
   return;
}

static void
tux64_boot_stage1_audio_initialize_context_waves(
   struct Tux64BootStage1AudioContext * ctx
) {
   Tux64BootStage1AudioSample * iter;
   union Tux64Int16 sample_hi;
   union Tux64Int16 sample_lo;
   Tux64UInt32 count_hi;
   Tux64UInt32 count_lo;

   iter = ctx->waves.square_silent;

   sample_hi.uint = TUX64_LITERAL_UINT16(TUX64_BOOT_STAGE1_AUDIO_WAVE_SAMPLE_HI);
   sample_lo.uint = TUX64_LITERAL_UINT16(TUX64_BOOT_STAGE1_AUDIO_WAVE_SAMPLE_LO);
   count_hi       = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_AUDIO_WAVE_SAMPLE_COUNT / 2u);
   count_lo       = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_AUDIO_WAVE_SAMPLE_COUNT_COMBINED) - count_hi;

   sample_hi.uint = tux64_endian_convert_uint16(sample_hi.uint, TUX64_ENDIAN_FORMAT_BIG);
   sample_lo.uint = tux64_endian_convert_uint16(sample_lo.uint, TUX64_ENDIAN_FORMAT_BIG);

   tux64_memory_fill(
      iter,
      sample_hi.bytes,
      count_hi,
      TUX64_LITERAL_UINT32(sizeof(Tux64BootStage1AudioSample))
   );
   iter += count_hi;

   tux64_memory_fill(
      iter,
      sample_lo.bytes,
      count_lo,
      TUX64_LITERAL_UINT32(sizeof(Tux64BootStage1AudioSample))
   );
   iter += count_lo;

   /* remember to flush the cache afterwards, as we don't want nightmare */
   /* debugging with the uncached segment. */
   tux64_boot_cache_flush_data(
      &ctx->waves,
      TUX64_LITERAL_UINT32(sizeof(ctx->waves))
   );

   return;
}

static void
tux64_boot_stage1_audio_initialize_context(
   struct Tux64BootStage1AudioContext * ctx,
   enum Tux64PlatformMipsN64BootConsoleType console_type
) {
   tux64_boot_stage1_audio_initialize_context_waves(ctx);
   tux64_boot_sync_fence_initialize_signaled(&ctx->queue_fence);

   ctx->vi_clock_rate   = tux64_boot_stage1_audio_vi_clock_rate(console_type);
   ctx->tone_notes      = TUX64_NULLPTR;
   return;
}

void
tux64_boot_stage1_audio_initialize(
   enum Tux64PlatformMipsN64BootConsoleType console_type
) {
   tux64_boot_stage1_audio_initialize_context(
      &tux64_boot_stage1_audio_context,
      console_type
   );

   return;
}

static void
tux64_boot_stage1_audio_play_note(
   const struct Tux64BootStage1AudioNote * note
) {
   struct Tux64BootStage1AudioContext * ctx;

   ctx = &tux64_boot_stage1_audio_context;

   if (TUX64_BOOT_CONFIG_DEBUG) {
      if (note->frequency == TUX64_LITERAL_UINT32(0u)) {
         tux64_boot_halt();
         TUX64_UNREACHABLE;
      }
   }

   ctx->note_repeats_remaining = note->repeat_count;

   tux64_boot_stage1_audio_ai_set_sample_rate(note->frequency);
   tux64_boot_stage1_audio_ai_queue_wave(note->type);
   return;
}

void
tux64_boot_stage1_audio_play(
   enum Tux64BootStage1AudioSound sound
) {
   struct Tux64BootStage1AudioContext * ctx;
   const struct Tux64BootStage1AudioTone * tone;
   const struct Tux64BootStage1AudioNote * note_start;

   ctx = &tux64_boot_stage1_audio_context;

   /* we have this indirection so we don't have to expose the entire note and */
   /* tone interface publically. */
   tone        = &tux64_boot_stage1_audio_tones[(Tux64UInt8)sound];
   note_start  = &tone->notes[tone->notes_count - 1u];

   if (TUX64_BOOT_CONFIG_DEBUG) {
      if (tone->notes_count > TUX64_LITERAL_UINT8(TUX64_BOOT_STAGE1_AUDIO_TONE_MAX_NOTES)) {
         tux64_boot_halt();
         TUX64_UNREACHABLE;
      }
   }

   /* this implicitly locks and prevents future contention, so calling this */
   /* is sufficient to ensure correct locking. */
   tux64_boot_stage1_audio_halt();

   ctx->tone_notes               = tone->notes;
   ctx->tone_notes_remaining     = tone->notes_count;
   tux64_boot_stage1_audio_play_note(note_start); /* this fires the first interrupt */
   return;
}

Tux64Boolean
tux64_boot_stage1_audio_is_playing(void) {
   struct Tux64BootStage1AudioContext * ctx;
   Tux64Boolean is_playing;

   ctx = &tux64_boot_stage1_audio_context;

   tux64_boot_stage1_audio_lock_hold();
   is_playing = (ctx->tone_notes == TUX64_NULLPTR ? TUX64_BOOLEAN_FALSE : TUX64_BOOLEAN_TRUE);
   tux64_boot_stage1_audio_lock_release();

   return is_playing;
}

void
tux64_boot_stage1_audio_halt(void) {
   struct Tux64BootStage1AudioContext * ctx;

   ctx = &tux64_boot_stage1_audio_context;

   /* halting the AI should prevent future interrupts from firing.  thus, */
   /* calling this is sufficient to ensure thread safety. */
   tux64_boot_stage1_audio_ai_halt();

   ctx->tone_notes = TUX64_NULLPTR;
   return;
}

static void
tux64_boot_stage1_audio_queue_buffer_current_note(void) {
   struct Tux64BootStage1AudioContext * ctx;
   const struct Tux64BootStage1AudioNote * note;

   ctx = &tux64_boot_stage1_audio_context;

   note = &ctx->tone_notes[ctx->tone_notes_remaining - 1u];

   tux64_boot_stage1_audio_ai_queue_wave(note->type);
   return;
}

void
tux64_boot_stage1_audio_queue_buffer(void) {
   struct Tux64BootStage1AudioContext * ctx;
   const struct Tux64BootStage1AudioNote * note;

   ctx = &tux64_boot_stage1_audio_context;

   /* if the lock is held, simply repeat the current wave a second time. */
   /* note that accessing the current note like this is safe because the only */
   /* time we change the current note is from here. */
   if (tux64_boot_stage1_audio_lock_is_held() == TUX64_BOOLEAN_TRUE) {
      tux64_boot_stage1_audio_queue_buffer_current_note();
      return;
   }

   /* check if there are any repeats left */
   if (ctx->note_repeats_remaining != TUX64_LITERAL_UINT32(0u)) {
      tux64_boot_stage1_audio_queue_buffer_current_note();
      ctx->note_repeats_remaining--;
      return;
   }

   /* no repeats left - move to the next note */
   ctx->tone_notes_remaining--;

   /* if there are no notes left, we are done */
   if (ctx->tone_notes_remaining == TUX64_LITERAL_UINT32(0u)) {
      ctx->tone_notes = TUX64_NULLPTR;
      return;
   }

   /* start playing the next note */
   note = &ctx->tone_notes[ctx->tone_notes_remaining - 1u];
   tux64_boot_stage1_audio_play_note(note);
   return;
}

