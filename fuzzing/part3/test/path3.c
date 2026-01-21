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
#include <stdio.h>
#include <string.h>

int main() {
  int x, y, z;
  char buf0[6000], buf1[6000], buf2[6000];
  int a = 0;
  int r = 43;
  
  fgets(buf0, sizeof(buf0), stdin);
  fgets(buf1, sizeof(buf1), stdin);
  fgets(buf2, sizeof(buf2), stdin);

  int b = 0;

printf("%d, %d, %d\n", strlen(buf0), strlen(buf1), strlen(buf2));

  if (strlen(buf0) < 100)
    if (strlen(buf0) > 10)
      if (strlen(buf0) < 70)
        if (strlen(buf0) > 20)
          if (strlen(buf1) < 250)
            if (strlen(buf1) < 200)
              if (strlen(buf1) > 20)
                if (strlen(buf1) > 30)
                  if (strlen(buf2) < 350)
                    if (strlen(buf2) < 300)
                      if (strlen(buf2) > 100)
                        if (strlen(buf2) > 120)
                          b = 1;
  if (b)
    printf("%d\n", r/a);
  return 0;
}
