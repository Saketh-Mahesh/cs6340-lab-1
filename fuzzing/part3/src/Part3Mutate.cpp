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
const std::string SPECIAL_CHARS = "!@#$%^&*()-_=+[]{};:'\",.<>/?\\|`~\n\t\r";

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

char get_random_byte() {
  return static_cast<char>(generate_random_number(0, 255));
}

char get_random_special_char() {
  int index =
      generate_random_number(0, static_cast<int>(SPECIAL_CHARS.length()) - 1);
  return SPECIAL_CHARS[index];
}

std::string generate_random_special_string(int minLen = 1, int maxLen = 100) {
  int length = generate_random_number(minLen, maxLen);
  std::string result;
  for (int i = 0; i < length; i++) {
    result += get_random_special_char();
  }
  return result;
}

std::string generate_random_string(int minLen = 1, int maxLen = 100) {
  int length = generate_random_number(minLen, maxLen);
  std::string result;
  for (int i = 0; i < length; i++) {
    result += get_random_byte();
  }
  return result;
}

std::string remove_random_characters(std::string Origin, int minDel = 1,
                                     int maxDel = 100) {
  if (Origin.empty())
    return Origin;
  int index = generate_random_number(0, Origin.length() - 1);
  int deleteLegnth = generate_random_number(
      minDel, std::min(maxDel, static_cast<int>(Origin.length() - index)));
  Origin.erase(index, deleteLegnth);
  return Origin;
}

void perform_random_replacement(std::string &Origin) {
  if (Origin.empty())
    return;

  int randomIndex = generate_random_number(0, Origin.length() - 1);
  int length = generate_random_number(
      1, std::max(1, static_cast<int>(Origin.length() - randomIndex)));

  int num = generate_random_number(1, 2);
  std::string randomString;
  if (num == 1) {
    randomString = generate_random_special_string(1, length + 10);
  } else {
    randomString = generate_random_string(1, length + 10);
  }

  Origin.replace(randomIndex, length, randomString);
}

void perform_random_insertion(std::string &Origin) {
  int randomIndex = generate_random_number(0, Origin.length());

  int num = generate_random_number(1, 2);
  std::string randomString;
  if (num == 1) {
    randomString = generate_random_special_string(1, 50);
  } else {
    randomString = generate_random_string(1, 50);
  }
  Origin.insert(randomIndex, randomString);
}

void swap_adjacent_bytes(std::string &Origin) {
  if (Origin.length() < 2)
    return;
  for (int i = 0; i + 3 < Origin.length(); i += 4) {
    std::swap(Origin[i], Origin[i + 3]);
    std::swap(Origin[i + 1], Origin[i + 2]);
  }
}

void keep_length_within_limit(std::string &Origin, size_t maxLength = 65000) {
  if (Origin.length() > maxLength) {
    Origin = Origin.substr(0, maxLength);
  }
}

std::string mutateA(std::string Origin) {
  if (Origin.empty()) {
    return Origin;
  }

  int num = generate_random_number(1, 2);

  if (num == 1) {
    perform_random_replacement(Origin);
  } else {
    perform_random_insertion(Origin);
  }

  keep_length_within_limit(Origin);

  return Origin;
}

std::string mutateB(std::string Origin) {
  if (Origin.empty()) {
    return Origin;
  }
  swap_adjacent_bytes(Origin);

  keep_length_within_limit(Origin);

  return Origin;
}

std::string mutateC(std::string Origin) {
  if (Origin.empty()) {
    return generate_random_string();
  }

  int num = generate_random_number(1, 2);

  if (num == 1) {
    Origin += generate_random_string();
  } else {
    Origin = generate_random_string() + Origin;
  }

  keep_length_within_limit(Origin);

  return Origin;
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
