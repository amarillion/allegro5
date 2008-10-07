/*
 *    Example program for the Allegro library.
 *
 *    Test chaining mixers to mixers.
 */

#include <stdio.h>
#include "allegro5/allegro5.h"
#include "allegro5/acodec.h"

int main(int argc, char **argv)
{
   ALLEGRO_VOICE *voice;
   ALLEGRO_MIXER *mixer;
   ALLEGRO_MIXER *submixer[2];
   ALLEGRO_SAMPLE *sample[2];
   ALLEGRO_SAMPLE_DATA *sample_data[2];
   float sample_time;
   float max_sample_time;
   int i;

   if (argc < 3) {
      printf("Usage: %s file1 file2\n", argv[0]);
      return 1;
   }

   if (!al_init()) {
      fprintf(stderr, "Could not init Allegro.\n");
      return 1;
   }

   if (al_audio_init(ALLEGRO_AUDIO_DRIVER_AUTODETECT)) {
      fprintf(stderr, "Could not init sound!\n");
      return 1;
   }

   voice = al_voice_create(44100, ALLEGRO_AUDIO_DEPTH_INT16,
      ALLEGRO_CHANNEL_CONF_2);
   if (!voice) {
      fprintf(stderr, "Could not create ALLEGRO_VOICE.\n");
      return 1;
   }

   mixer = al_mixer_create(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32,
      ALLEGRO_CHANNEL_CONF_2);
   submixer[0] = al_mixer_create(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32,
      ALLEGRO_CHANNEL_CONF_2);
   submixer[1] = al_mixer_create(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32,
      ALLEGRO_CHANNEL_CONF_2);
   if (!mixer || !submixer[0] || !submixer[1]) {
      fprintf(stderr, "al_mixer_create failed.\n");
      return 1;
   }

   if (al_voice_attach_mixer(voice, mixer) != 0) {
      fprintf(stderr, "al_voice_attach_mixer failed.\n");
      return 1;
   }

   for (i = 0; i < 2; i++) {
      const char *filename = argv[i + 1];
      sample_data[i] = al_load_sample(filename);
      if (!sample_data[i]) {
         fprintf(stderr, "Could not load sample from '%s'!\n", filename);
         return 1;
      }
      sample[i] = al_sample_create(NULL);
      if (!sample[i]) {
         fprintf(stderr, "al_sample_create failed.\n");
         return 1;
      }
      if (al_sample_set_data(sample[i], sample_data[i]) != 0) {
         fprintf(stderr, "al_sample_set_ptr failed.\n");
         return 1;
      }
      if (al_mixer_attach_sample(submixer[i], sample[i]) != 0) {
         fprintf(stderr, "al_mixer_attach_sample failed.\n");
         return 1;
      }
      if (al_mixer_attach_mixer(mixer, submixer[i]) != 0) {
         fprintf(stderr, "al_mixer_attach_mixer failed.\n");
         return 1;
      }
   }

   /* Play sample in looping mode. */
   for (i = 0; i < 2; i++) {
      al_sample_set_enum(sample[i], ALLEGRO_AUDIOPROP_LOOPMODE,
         ALLEGRO_PLAYMODE_ONEDIR);
      al_sample_play(sample[i]);
   }

   al_sample_get_float(sample[0], ALLEGRO_AUDIOPROP_TIME, &max_sample_time);
   al_sample_get_float(sample[1], ALLEGRO_AUDIOPROP_TIME, &sample_time);
   if (sample_time > max_sample_time)
      max_sample_time = sample_time;

   printf("Playing...");
   fflush(stdout);

   al_rest(max_sample_time);

   al_sample_set_float(sample[0], ALLEGRO_AUDIOPROP_GAIN, 0.5);
   al_rest(max_sample_time);

   al_sample_set_float(sample[1], ALLEGRO_AUDIOPROP_GAIN, 0.25);
   al_rest(max_sample_time);

   al_sample_stop(sample[0]);
   al_sample_stop(sample[1]);
   printf("\n");

   /* Free the memory allocated. */
   for (i = 0; i < 2; i++) {
      al_sample_set_data(sample[i], NULL);
      al_sample_data_destroy(sample_data[i]);
      al_sample_destroy(sample[i]);
      al_mixer_destroy(submixer[i]);
   }
   al_mixer_destroy(mixer);
   al_voice_destroy(voice);

   al_audio_deinit();

   return 0;
}
END_OF_MAIN()

/* vim: set sts=3 sw=3 et: */
