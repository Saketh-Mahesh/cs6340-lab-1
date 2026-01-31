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
#include "Mutate.h"

#include <cstring>
#include <iostream>
#include <map>
#include <random>

static std::mt19937 gen(std::random_device{}());
const std::string CHARSET =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

std::map<std::string, Campaign> to_campaign = {{"MutationA", MutationA},
                                               {"MutationB", MutationB},
                                               {"MutationC", MutationC}};

bool toCampaign(std::string Str, Campaign &FuzzCampaign) {
  auto I = to_campaign.find(Str);
  if (I == to_campaign.end()) {
    fprintf(stderr, "\"%s\" not a valid fuzz campaign, choice options are: ",
            Str.c_str());
    for (auto &I2 : to_campaign) {
      fprintf(stderr, "%s ", I2.first.c_str());
    }
    fprintf(stderr, "\n");
    return false;
  }
  FuzzCampaign = I->second;
  return true;
}

/*
 * Implement your mutation algorithms.
 */

int generate_random_number(int min, int max) {
  std::uniform_int_distribution<> distr(min, max);
  return distr(gen);
}

std::string generate_random_string() {

  int length = generate_random_number(1, 20);

  std::string result;
  for (int i = 0; i < length; ++i) {
    result += CHARSET[generate_random_number(0, CHARSET.length() - 1)];
  }
  return result;
}

std::string mutateA(std::string Origin) {
  int randomIndex = generate_random_number(0, Origin.length() - 1);
  std::string result = generate_random_string();
  Origin.insert(randomIndex, result);
  return Origin;
}

std::string mutateB(std::string Origin) {
  int randomIndex1 = generate_random_number(0, Origin.length() - 1);
  int randomIndex2 = generate_random_number(0, Origin.length() - 1);

  int length1 = generate_random_number(1, 10);
  int length2 = generate_random_number(1, 10);

  std::string randomString1 = generate_random_string();
  std::string randomString2 = generate_random_string();
  Origin.replace(randomIndex1, length1, randomString1);
  Origin.replace(randomIndex2, length2, randomString2);
  return Origin;
}

std::string mutateC(const std::string &origin) {
  return origin + generate_random_string();
}

std::string mutate(std::string Origin, Campaign &FuzzCampaign) {
  std::string Mutant;
  switch (FuzzCampaign) {
  case MutationA:
    return mutateA(Origin);
  case MutationB:
    return mutateB(Origin);
  case MutationC:
    return mutateC(Origin);
  }
}
