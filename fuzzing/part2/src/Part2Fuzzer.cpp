/*
 * Copyright Â© 2021 Georgia Institute of Technology (Georgia Tech). All Rights Reserved.
 * Template code for CS 6340 Software Analysis
 * Instructors: Mayur Naik and Chris Poch
 * Head TAs: Kelly Parks and Joel Cooper
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
#include <cstdlib>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "Mutate.h"
#include "Utils.h"

int Freq = 1000;
int Count = 0;

bool test(std::string &Target, std::string &Input, std::string &CampaignStr, std::string &OutDir) {
  Count++;
  int ReturnCode = runTarget(Target, Input);
  //std::cerr << "Returned " << ReturnCode << std::endl;
  switch (ReturnCode) {
  case 0:
    if (Count % Freq == 0)
      storePassingInput(Input, CampaignStr, OutDir);
    return true;
  case 256:
    storeCrashingInput(Input, CampaignStr, OutDir);
    fprintf(stderr, "%d crashes found\n", failureCount);
    return false;
  case 127:
    fprintf(stderr, "%s not found\n", Target.c_str());
    exit(1);
  }
}

// ./fuzzer [exe file] [seed input dir] [output dir]
int main(int argc, char **argv) { 
  if (argc < 5) { 
    printf("usage %s [exe file] [seed input dir] [output dir] [campaign]\n", argv[0]);
    return 1;
  }

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

  if (argc >= 6) {
    Freq = strtol(argv[5],NULL,10); 
  } 

  std::string Target(argv[1]);
  std::string SeedInputDir(argv[2]);
  std::string OutDir(argv[3]);

  std::string CampaignStr(argv[4]);
  Campaign FuzzCampaign;
  if (!toCampaign(CampaignStr, FuzzCampaign)) {
    return 1;
  }

  initialize(OutDir);

  if (readSeedInputs(SeedInputDir)) {
    fprintf(stderr, "Cannot read seed input directory\n");
    return 1;
  }

  for (int i = 0; i < SeedInputs.size(); i++) {
    std::string currentSeed = SeedInputs[i];
    std::string currentMutant = currentSeed;
    for (int j = 0; j < maxTestsPerSeed; j++) {
      if (Count % Freq == 0) {
        std::cerr << "Count is " << Count << std::endl;
      }        
      std::string newMutant = mutate(currentMutant, FuzzCampaign);
      test(Target, newMutant, CampaignStr, OutDir);
      if (failureCount >= maxCrashes) {
        break;
      }
      currentMutant = newMutant;
    }
  }
  return 0;
}
