/* Fixes the Fend bug in Diablo II LoD 1.14d by disabling Dodge and Avoid
 * animations. The skills themselves still work, and the player will hear the
 * Amazon's "hyah" sound when they trigger. Many thanks to EnPhreg and other
 * forum users in this thread for sharing their finds:
 * https://d2mods.info/forum/viewtopic.php?p=501489#p501489
 *
 * The file "Game.exe" will be changed. Make a backup beforehand in case you
 * ever want to go back. In general, please do not try to connect to Battle.net
 * with modified game files (you might face a ban).
 * Game versions other than 1.14d are currently not supported.
 *
 * https://github.com/hippie68/diablo2-fend-fix */

#include <stdio.h>
#include <stdlib.h>

struct patch_data{
  size_t offset;
  const int old;
  const int new;
  char name[8];
};

struct patch_data patches[] = {
  {0x17ecfd, 0x80, 0x00, "Dodge 1"},
  {0x17ecfe, 0x00, 0x02, "Dodge 2"},
  {0x17ecf4, 0x01, 0x02, "Avoid 1"},
  {0x1ad84b, 0x01, 0x02, "Avoid 2"},
};
int patch_count = (int) sizeof(patches) / sizeof(struct patch_data);

// Patches a Game.exe file with given patch data
void patch(FILE *file, struct patch_data pdata) {
  int byte;

  fseek(file, pdata.offset, SEEK_SET);
  byte = getc(file);

  // Quit program on read error
  if (byte == EOF) {
    printf("Could not read from file, aborting.\n");
    exit(1);
  }

  // Patch
  if (byte == pdata.old) {
    fseek(file, -1, SEEK_CUR);
    // Quit program on write error
    if (putc(pdata.new, file) == EOF) {
      fprintf(stderr, "Could not write to file, aborting.\n");
      exit(1);
    }
  // Skip if already patched
  } else if (byte == pdata.new) {
    printf("Already patched: \"%s\" (offset: 0x%lx, current value: 0x%02x)\n",
      pdata.name, pdata.offset, byte);
  // Quit program if unknown value found
  } else {
    fprintf(stderr, "Unexpected value found: \"%s\" (offset 0x%lx, "
      "current value: 0x%02x).\nExpected value: 0x%02x, aborting.\n",
      pdata.name, pdata.offset, byte, pdata.old);
    exit(1);
  }
}

int main(int argc, char *argv[]) {
  // Print usage if no arguments provided
  if (argc == 1) {
    printf("Please provide the full path to Diablo II's \"Game.exe\" "
      "as argument.\n");
    return 1;
  }

  // Open Game.exe
  FILE *file;
  file = fopen(argv[1], "r+b");
  if (file == NULL) {
    fprintf(stderr, "Could not open file \"%s\"\n", argv[1]);
    return 1;
  }

  // Patch Game.exe
  for (int i = 0; i < patch_count; i++) {
    patch(file, patches[i]);
  }

  // Close Game.exe
  if (fclose(file) == EOF) {
    fprintf(stderr, "Could not close file \"%s\"\n", argv[1]);
    return 1;
  } else {
    printf("Done.\n");
  }

  return 0;
}
