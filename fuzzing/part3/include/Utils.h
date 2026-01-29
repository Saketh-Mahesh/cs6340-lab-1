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
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <set>
#include <streambuf>
#include <string>
#include <sys/stat.h>

std::string readOneFile(std::string &Path) {
  std::ifstream SeedFile(Path);
  std::string Line((std::istreambuf_iterator<char>(SeedFile)),
                   std::istreambuf_iterator<char>());
  return Line;
}

int runTarget(std::string &Target, std::string &Input) {
  std::string Cmd = Target + " > /dev/null 2>&1";
  FILE *F = popen(Cmd.c_str(), "w");
  fprintf(F, "%s", Input.c_str());
  return pclose(F);
}

std::map<Campaign,std::vector<std::string>> SeedInputs;

int readSeedInputs(std::string &SeedInputDir) {
  DIR *Directory;
  struct dirent *Ent;
  if ((Directory = opendir(SeedInputDir.c_str())) != NULL) {
    while ((Ent = readdir(Directory)) != NULL) {
      if (!(Ent->d_type == DT_REG))
        continue;
      std::string Path = SeedInputDir + "/" + std::string(Ent->d_name);
      std::string Line = readOneFile(Path);
      SeedInputs[MutationA].push_back(Line);
      SeedInputs[MutationB].push_back(Line);
      SeedInputs[MutationC].push_back(Line);
    }
    closedir(Directory);
    return 0;
  } else {
    return 1;
  }
}

int successCount = 0;
int failureCount = 0;
const int maxTests = 10000;
const int maxCrashes = 100;

void initialize(std::string &OutDir) {
  int Status;
  std::string SuccessDir = OutDir + "/success";
  std::string FailureDir = OutDir + "/failure";
  mkdir(SuccessDir.c_str(), 0755);
  mkdir(FailureDir.c_str(), 0755);
}

void storePassingInput(std::string &Input, const std::string &CampaignStr, std::string &OutDir) {
  std::string Path = OutDir + "/success/input" + std::to_string(++successCount) + "-" + CampaignStr;
  std::ofstream OutFile(Path);
  OutFile << Input;
  OutFile.close();
}

void storeCrashingInput(std::string &Input, const std::string &CampaignStr, std::string &OutDir) {
  std::string Path = OutDir + "/failure/input" + std::to_string(++failureCount) + "-" + CampaignStr;
  std::ofstream OutFile(Path);
  OutFile << Input;
  OutFile.close();
}
