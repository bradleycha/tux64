/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/audio.h - Implementations for audio playback.   */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/audio.h"

void
tux64_boot_stage1_audio_initialize(
   enum Tux64PlatformMipsN64BootConsoleType console_type
) {
   /* TODO: implement */
   (void)console_type;
   return;
}

void
tux64_boot_stage1_audio_play(
   enum Tux64BootStage1AudioSound sound
) {
   /* TODO: implement */
   (void)sound;
   return;
}

Tux64Boolean
tux64_boot_stage1_audio_is_playing(void) {
   /* TODO: implement */
   return TUX64_BOOLEAN_FALSE;
}

void
tux64_boot_stage1_audio_halt(void) {
   /* TODO: implement */
   return;
}

void
tux64_boot_stage1_audio_queue_buffer(void) {
   /* TODO: implement */
   return;
}

