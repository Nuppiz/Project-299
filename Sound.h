#ifndef SOUND_H
#define SOUND_H

void loadSFX(char* file1, char* file2, char* file3);
void playSounds(int effect_id);
void stopSFX();
void stopMusic();
void playMusic(char *music);
void initSounds();

#endif/* SOUND_H */
