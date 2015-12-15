/**
 *  \file scheduler.c
 *  \brief The main script of the program, the magic starts here
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "structs.h"
#include "scheduler.h"
#include "input_reader.h"
#include "data_utility.h"
#include "fitness_calculation.h"
#include "defs.h"
#include "html_output.h"
#include "genetic_algorithm.h"

/**
 *  \brief The starting point of the program
 *  
 *  \return Returns whether the program has exited with an error or success
 *  
 *  \details In here, the function that reads the config file is run
 *  and then the genetic algorithm is run. Lastly, the schedules are
 *  being generated in html documents and ready to be implemented
 *  to a web interface for example
 */
int main(void)
{
    char specName[32];
    int i, j, k, l, seed, startTime;
    Generation *gen = 0;
    SemesterData sd;

    /* Null all SemesterData values */
    memset(&sd, 0, sizeof(SemesterData));
 
    /*if (!scanf("%d", &seed))*/
		seed = time(NULL);
        /*seed = 1450193462;*/
    srand(seed);
        
    /* Set start time to now */
    startTime = time(NULL);
    
    /* Read configuration file */
    if (!read_config("scheduler.input", &sd))
    {
        printf("Error: Could not read configuration file.\n");
        exit(1);
    }

    /* DEBUG */
    for (i = 0; i < sd.numCourses; i++)
    {
        Course *curCourse = &sd.courses[i];
        
        printf("%s\nSpecs: ", curCourse->name);
        
        for (j = 0; j < sd.numSpecializations; j++)
        {
            Specialization *curSpec = &sd.specializations[j];
            
            for (k = 0; k < curSpec->numCourses; k++)
                if (curSpec->courses[k] == curCourse)
                    printf("%s ", curSpec->name);
                
        }
        
        printf("\n");
        for (j = 0; j < curCourse->numTeachers; j++)
        {
            Teacher *curTeacher = curCourse->teachers[j];
            
            printf("%s\n", curTeacher->name);
            for (k = 0; k < curTeacher->numOffTimes; k++)
            {
                OffTime *curOffTime = &curTeacher->offTimes[k];
                
                printf("- %d (%d, %d)\n", curOffTime->day, curOffTime->periods[0], curOffTime->periods[1]);
            }
        }
        
        printf("\n\n");
    }
    
    /* Calculate amount of Lectures (genes) */
    calc_amount_of_lectures(&sd);
    
    /* Run genetic algorithm */
    run_ga(&gen, &sd);
    
    /* Print issues with best schedule */
    print_schedule_issues(&gen->schedules[0]);
    printf("Final schedule has a fitness of %d\n", gen->schedules[0].fitness);
    printf("Seed: %d\n", seed);
    
    /* Print best schedule for each specialization to file */
    for (i = 0; i < sd.numSpecializations; i++)
    {
        sprintf(specName, "%s.html", sd.specializations[i].name);
        
        printf("Writing to file %s..\n", specName);
        print_schedule_to_file(&gen->schedules[0], &sd.specializations[i], specName);
    }
    
    free_semesterdata(&sd);
    free_generation(gen);
    
    printf("Finished, ran for %d seconds.\n", time(NULL) - startTime);
    getchar();
    
    return 0;
}