/*
 * Copyright Â© 2021 Georgia Institute of Technology (Georgia Tech). All Rights
 * Reserved. Template code for CS 6340 Software Analysis Instructors: Mayur Naik
 * and Chris Poch Head TAs: Kelly Parks and Joel Cooper
 *
 * Georgia Tech asserts copyright ownership of this template and all derivative
 * works, including solutions to the projects assigned in this course. Students
 * and other users of this template code are advised not to share it with others
 * or to make it available on publicly viewable websites including repositories
 * such as GitHub and GitLab. This copyright statement should not be removed
 * or edited. Removing it will be considered an academic integrity issue.
 *
 * We do grant permission to share solutions privately with non-students such
 * as potential employers as long as this header remains in full. However,
 * sharing with other current or future students or using a medium to share
 * where the code is widely available on the internet is prohibited and
 * subject to being investigated as a GT honor code violation.
 * Please respect the intellectual ownership of the course materials
 * (including exam keys, project requirements, etc.) and do not distribute them
 * to anyone not enrolled in the class. Use of any previous semester course
 * materials, such as tests, quizzes, homework, projects, videos, and any other
 * coursework, is prohibited in this course. */
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "Mutate.h"
#include "Utils.h"

#include <time.h>

std::set<std::pair<int, int>> coverageInfo;

std::string CampaignToStr(Campaign &FuzzCamp) {
  switch (FuzzCamp) {
  case MutationA:
    return "MutationA";
  case MutationB:
    return "MutationB";
  case MutationC:
    return "MutationC";
  }
}

/*
 * Implement your feedback-directed seed update algorithm.
 */
std::pair<std::string, Campaign> selectSeedAndCampaign() {
  Campaign FuzzCamp = (Campaign)(rand() % NumCampaigns);
  std::string Seed = SeedInputs[FuzzCamp].back();
  return std::make_pair(Seed, FuzzCamp);
}

std::set<std::pair<int, int>> parseCoverageFile(std::string &file_path) {
  std::set<std::pair<int, int>> currentCoverage;
  std::ifstream file(file_path);

  if (!file.is_open()) {
    std::cerr << "Error: Could not open the file " << file_path << std::endl;
    return currentCoverage;
  }

  std::string line;

  while (std::getline(file, line)) {
    size_t comma_pos = line.find(',');
    if (comma_pos != std::string::npos) {
      std::string line_raw = line.substr(0, comma_pos);
      std::string col_raw = line.substr(comma_pos + 1);
      int line = std::stoi(line_raw);
      int col = std::stoi(col_raw);
      currentCoverage.insert(std::make_pair(line, col));
    }
  }
  file.close();
  return currentCoverage;
}

/*
 * Implement your feedback-directed seed update algorithm.
 */
void updateSeedInputs(std::string &Target, std::string &Mutated,
                      Campaign &FuzzCamp, bool Success) {
  if (!Success) {
    SeedInputs[FuzzCamp].push_back(Mutated);
    return;
  }
  std::string coverage_file = Target + ".cov";
  std::set<std::pair<int, int>> currentCoverage =
      parseCoverageFile(coverage_file);
  bool inserted_new_coverage = false;
  for (const auto &entry : currentCoverage) {
    auto [it, inserted] = coverageInfo.insert(entry);
    if (inserted) {
      inserted_new_coverage = true;
    }
  }
  if (inserted_new_coverage) {
    SeedInputs[FuzzCamp].push_back(Mutated);
  }
}

int Freq = 1000;
int Count = 0;

bool test(std::string &Target, std::string &Input, Campaign &FuzzCamp,
          std::string &OutDir) {
  Count++;
  int ReturnCode = runTarget(Target, Input);
  switch (ReturnCode) {
  case 0:
    if (Count % Freq == 0)
      storePassingInput(Input, CampaignToStr(FuzzCamp), OutDir);
    return true;
  case 256:
    storeCrashingInput(Input, CampaignToStr(FuzzCamp), OutDir);
    fprintf(stderr, "%d crashes found\n", failureCount);
    return false;
  case 127:
    fprintf(stderr, "%s not found\n", Target.c_str());
    exit(1);
  }
}

// ./fuzzer [exe file] [seed input dir] [output dir]
int main(int argc, char **argv) {
  if (argc < 4) {
    printf("usage %s [exe file] [seed input dir] [output dir]\n", argv[0]);
    return 1;
  }

  srand(time(NULL));

  struct stat Buffer;
  if (stat(argv[1], &Buffer)) {
    fprintf(stderr, "%s not found\n", argv[1]);
    return 1;
  }

  if (stat(argv[2], &Buffer)) {
    fprintf(stderr, "%s not found\n", argv[2]);
    return 1;
  }

  if (stat(argv[3], &Buffer)) {
    fprintf(stderr, "%s not found\n", argv[3]);
    return 1;
  }

  if (argc >= 5) {
    Freq = strtol(argv[4], NULL, 10);
  }

  std::string Target(argv[1]);
  std::string SeedInputDir(argv[2]);
  std::string OutDir(argv[3]);

  initialize(OutDir);

  if (readSeedInputs(SeedInputDir)) {
    fprintf(stderr, "Cannot read seed input directory\n");
    return 1;
  }

  while (Count < maxTests && failureCount < maxCrashes) {
    // NOTE: You should feel free to manipulate this run loop
    if (Count % Freq == 0) {
      std::cerr << "Count is " << Count << std::endl;
    }
    std::pair<std::string, Campaign> SC = selectSeedAndCampaign();
    auto Mutant = mutate(SC.first, SC.second);
    auto Success = test(Target, Mutant, SC.second, OutDir);
    updateSeedInputs(Target, Mutant, SC.second, Success);
  }
  return 0;
}
