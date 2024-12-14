#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <dirent.h>

#define MAX_LEN 23
#define AN_LEN 23
#define SN_LEN 19
#define SNX_LEN 4

int compute_averages(char *dirpath, float percent_averages[MAX_LEN], float difference_averages[MAX_LEN]);
uint8_t print_correlated_time_diffs(float sn_diff_avgs[MAX_LEN], float an_diff_avgs[MAX_LEN], float snx_diff_avgs[MAX_LEN]);

int main(int argc, char **argv)
{
  (void)argc, (void)argv;

  float sn_averages[MAX_LEN] = {0}, an_averages[MAX_LEN] = {0}, snx_averages[MAX_LEN] = {0};
  float sn_diff_avgs[MAX_LEN] = {0}, an_diff_avgs[MAX_LEN] = {0}, snx_diff_avgs[MAX_LEN] = {0};
  if (compute_averages("./src/times/an/", an_averages, an_diff_avgs))
    return 1;
  if (compute_averages("./src/times/sn/", sn_averages, sn_diff_avgs))
    return 1;
  if (compute_averages("./src/times/snx/", snx_averages, snx_diff_avgs))
    return 1;

  print_correlated_time_diffs(sn_diff_avgs, an_diff_avgs, snx_diff_avgs);

  char outpath[] = "./include/times.h";
  FILE *f = fopen(outpath, "w");
  if (f == NULL)
  {
    fprintf(stderr, "[ERROR] Could not open file %s: %s\n", outpath, strerror(errno));
  }
  fprintf(f, "#ifndef __TIMES__\n"
             "#define __TIMES__\n"
             "extern float an_times[%u], sn_times[%u], snx_times[%u];\n"
             "#endif // __TIMES__\n"
             "\n"
             "#ifdef __TIMES_IMPLEMENTATION__\n"
             "float an_times[%u] = {",
          AN_LEN, SN_LEN, SNX_LEN,
          AN_LEN);
  for (size_t i = 0; i < AN_LEN - 1; ++i)
    fprintf(f, "%f, ", an_averages[i]);
  fprintf(f, "%f};\n"
             "float sn_times[%u] = {",
          an_averages[AN_LEN - 1], SN_LEN);
  for (size_t i = 0; i < SN_LEN - 1; ++i)
    fprintf(f, "%f, ", sn_averages[i]);
  fprintf(f, "%f};\n"
             "float snx_times[%u] = {",
          sn_averages[SN_LEN - 1], SNX_LEN);
  for (size_t i = 0; i < SNX_LEN - 1; ++i)
    fprintf(f, "%f, ", snx_averages[i]);
  fprintf(f, "%f};\n"
             "#endif // __TIMES_IMPLEMENTATION__",
          snx_averages[SNX_LEN - 1]);
  fclose(f);
  printf("[INFO] Succsessfully written %s\n", outpath);

  return 0;
}

int compute_averages(char *dirpath, float percent_averages[MAX_LEN], float difference_averages[MAX_LEN])
{
  DIR *FD = opendir(dirpath);
  if (FD == NULL)
  {
    fprintf(stderr, "[ERROR] Could not open directory %s: %s\n", dirpath, strerror(errno));
    return 1;
  }

  struct dirent *in_file;

  size_t files_count = 0;
  for (uint32_t i = 0; (in_file = readdir(FD)) != NULL; ++i)
  {
    // do not open current/previous dirs
    if (!strcmp(in_file->d_name, "."))
      continue;
    if (!strcmp(in_file->d_name, ".."))
      continue;

    char fname[512] = {0};
    strcpy(fname, dirpath);
    strcat(fname, in_file->d_name);
    FILE *f = fopen(fname, "rb");
    if (f == NULL)
    {
      fprintf(stderr, "[ERROR] Could not open file %s: %s\n", fname, strerror(errno));
      return 1;
    }
    ++files_count;

    size_t len;
    fread(&len, sizeof(len), 1, f);
    float *times = calloc(len, sizeof(float));
    fread(times, sizeof(times[0]), len, f);
    percent_averages[0] = times[0] / times[len - 1];
    for (size_t i = 1; i < len; ++i)
    {
      percent_averages[i] += times[i] / times[len - 1];
      difference_averages[i] += (times[i] - times[i - 1]) / times[len - 1];
    }

    free(times);
    fclose(f);
  }

  for (size_t i = 0; i < MAX_LEN; ++i)
  {
    percent_averages[i] /= files_count;
    difference_averages[i] /= files_count;
  }

  return 0;
}

uint8_t print_correlated_time_diffs(float sn_diff_avgs[MAX_LEN], float an_diff_avgs[MAX_LEN], float snx_diff_avgs[MAX_LEN])
{
  float s = 0;

  char corr_an[AN_LEN][10] = {0};
  FILE *f_corr_an = fopen("./src/times/correlation_an.txt", "r");
  if (f_corr_an == NULL)
  {
    fprintf(stderr, "[ERROR] Could not open file %s: %s", "./src/times/correlation_an.txt", strerror(errno));
    return 1;
  }
  for (size_t i = 0; i < AN_LEN; ++i)
  {
    fscanf(f_corr_an, "%[^\n]", corr_an[i]);
    fgetc(f_corr_an);

    s += an_diff_avgs[i];
  }
  fclose(f_corr_an);

  printf("an:  %6.2f%%\n", s * 100.0f);
  s = 0;

  char corr_sn[SN_LEN][10] = {0};
  FILE *f_corr_sn = fopen("./src/times/correlation_sn.txt", "r");
  if (f_corr_sn == NULL)
  {
    fprintf(stderr, "[ERROR] Could not open file %s: %s", "./src/times/correlation_sn.txt", strerror(errno));
    return 1;
  }
  for (size_t i = 0; i < SN_LEN; ++i)
  {
    fscanf(f_corr_sn, "%[^\n]", corr_sn[i]);
    fgetc(f_corr_sn);

    s += sn_diff_avgs[i];
  }
  fclose(f_corr_sn);

  printf("sn:  %6.2f%%\n", s * 100.0f);
  s = 0;

  char corr_snx[SNX_LEN][10] = {0};
  FILE *f_corr_snx = fopen("./src/times/correlation_snx.txt", "r");
  if (f_corr_snx == NULL)
  {
    fprintf(stderr, "[ERROR] Could not open file %s: %s", "./src/times/correlation_snx.txt", strerror(errno));
    return 1;
  }
  for (size_t i = 0; i < SNX_LEN; ++i)
  {
    fscanf(f_corr_snx, "%[^\n]", corr_snx[i]);
    fgetc(f_corr_snx);

    s += snx_diff_avgs[i];
  }
  fclose(f_corr_snx);

  printf("snx: %6.2f%%\n", s * 100.0f);

  printf("an                  | sn                  | snx\n");
  for (size_t i = 0; i < SNX_LEN; ++i)
    printf("%-10s: %6.2f%% | %-10s: %6.2f%% | %-10s: %6.2f%%\n",
           corr_an[i], an_diff_avgs[i] * 100.0f,
           corr_sn[i], sn_diff_avgs[i] * 100.0f,
           corr_snx[i], snx_diff_avgs[i] * 100.0f);
  for (size_t i = SNX_LEN; i < SN_LEN; ++i)
    printf("%-10s: %6.2f%% | %-10s: %6.2f%% |\n",
           corr_an[i], an_diff_avgs[i] * 100.0f,
           corr_sn[i], sn_diff_avgs[i] * 100.0f);
  for (size_t i = SN_LEN; i < AN_LEN; ++i)
    printf("%-10s: %6.2f%% |                     |\n",
           corr_an[i], an_diff_avgs[i] * 100.0f);

  return 0;
}