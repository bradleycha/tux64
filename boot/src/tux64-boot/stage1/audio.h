/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/audio.h - Header for audio playback.            */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE1_AUDIO_H
#define _TUX64_BOOT_STAGE1_AUDIO_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include <tux64/platform/mips/n64/boot.h>

/*----------------------------------------------------------------------------*/
/* Initializes the audio subsystem.  This must be called before all other     */
/* audio functions are called.                                                */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_audio_initialize(
   enum Tux64PlatformMipsN64BootConsoleType console_type
);

#define TUX64_BOOT_STAGE1_AUDIO_SOUND_FIELD_COUNT 2u
enum Tux64BootStage1AudioSound {
   TUX64_BOOT_STAGE1_AUDIO_SOUND_STARTUP,
   TUX64_BOOT_STAGE1_AUDIO_SOUND_FATAL_ERROR
};

/*----------------------------------------------------------------------------*/
/* Plays the given sound, halting any sound which is already playing.         */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_audio_play(
   enum Tux64BootStage1AudioSound sound
);

/*----------------------------------------------------------------------------*/
/* Returns whether a sound is currently playing or not.                       */
/*----------------------------------------------------------------------------*/
Tux64Boolean
tux64_boot_stage1_audio_is_playing(void);

/*----------------------------------------------------------------------------*/
/* Halts any currently playing sounds.                                        */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_audio_halt(void);

/*----------------------------------------------------------------------------*/
/* The code to run when the next audio buffer is requested.  This should only */
/* be called from the interrupt handler.                                      */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_audio_queue_buffer(void);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_AUDIO_H */

