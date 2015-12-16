/**
 *  \file html_output.c
 *  \brief The html output script is responsible for the html schedules that is being generated
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

#include "defs.h"
#include "structs.h"
#include "data_utility.h"

/**
 *  \brief Prints the file header
 *  
 *  \param [in] f The file in which the schedule is being generated
 *  \param [in] pageTitle The name of the page
 *  
 *  \details This function is responsible for the header of the file
 */
void print_file_header(FILE *f, char* pageTitle)
{
    fprintf(f, "<html>\n<head>\n");
    fprintf(f, "<title>%s</title>\n", pageTitle);
    fprintf(f, "</head>\n<body>\n");
}

/**
 *  \brief Prints the file footer
 *  
 *  \param [in] f The file in which the schedule is being generated
 *  
 *  \details This function is responsible for the footer of the file
 */
void print_footer(FILE *f)
{
    fprintf(f, "<hr />\nGenerated by <b>AAU Scheduler</b>\n</body>\n</html>\n");
}

/**
 *  \brief Initiates a table
 *  
 *  \param [in] f The file in which the schedule is being generated
 *  \param [in] cellspacing The spacing between the cells in the table
 *  
 *  \details This function is laying the foundation for a html table
 */
void begin_print_table(FILE *f, int cellspacing)
{
    fprintf(f, "<table width=\"%.2f%%\" cellspacing=\"%d\">\n", TABLE_WIDTH, cellspacing);
}

/**
 *  \brief Ends a table
 *  
 *  \param [in] f The file in which the schedule is being generated
 *  
 *  \details This function is adding the end table tag for a html table
 */
void end_print_table(FILE *f)
{
    fprintf(f, "</table>\n");
}

/**
 *  \brief Prints a header for a row
 *  
 *  \param [in] f The file in which the schedule is being generated
 *  \param [in] width The width of the row
 *  \param [in] str The name of the row
 *  
 *  \details This function creates a row with the given width and name
 */
void print_row_header(FILE *f, double width, const char* str, ...)
{
    va_list pl;
    va_start(pl, str);
    
    fprintf(f, "\t\t<td width=\"%.2f%%\"><b>", width);
    vfprintf(f, str, pl);
    fprintf(f, "</b></td>\n");
    
    va_end(pl);
}

/**
 *  \brief Prints a shedule title
 *  
 *  \param [in] f The file in which the schedule is being generated
 *  \param [in] title The title
 *  
 *  \details This defines a title for the schedule.
 *  An example could be "Schedule for Robotics"
 */
void print_title(FILE *f, const char* title)
{
    fprintf(f, "<h2>Schedule for %s</h2>", title);
}

/**
 *  \brief Begins data print
 *  
 *  \param [in] f The file in which the schedule is being generated
 *  \param [in] str The data to be printed
 *  
 *  \details This function is printing the provided data from str into the file f
 */
void begin_print_data(FILE *f, const char* str)
{
    fprintf(f, "\t\t<td height=\"100px\">%s", str);
}

/**
 *  \brief Ends the data print
 *  
 *  \param [in] f The file in which the schedule is being generated
 *  
 *  \details Adds the ending tag for the data
 */
void end_print_data(FILE *f)
{
    fprintf(f, "</td>\n");
}

/**
 *  \brief Prints the rows of lectures
 *  
 *  \param [in] f The file in which the schedule is being generated
 *  \param [in] backgroundColor The color of the row
 *  
 *  \details This function initiates rows with a given color
 */
void begin_print_row(FILE *f, const char* backgroundColor)
{
    fprintf(f, "\t<tr bgcolor=\"%s\">\n", backgroundColor);
}

/**
 *  \brief Ends the row print
 *  
 *  \param [in] f The file in which the schedule is being generated
 *  
 *  \details This function adds the ending tag for the row
 */
void end_print_row(FILE *f)
{
    fprintf(f, "\t</tr>\n");
}

/**
 *  \brief Prints a period to the schedule
 *  
 *  \param [in] schedule Pointer to a schedule
 *  \param [in] sp Specialization contains information about the specialization
 *  the schedule is generated for
 *  \param [in] f The file in which the schedule is being generated
 *  \param [in] periodId Period to print
 *  \param [in] weekNumber The number of the current week
 *  
 *  \details This function adds a period to the schedule and formats it as needed
 */
void print_period(Schedule *schedule, Specialization *sp, FILE *f, int periodId, int weekNumber)
{
    int i, j, k, foundMatches;
    
    begin_print_row(f, "#F0F0F0");
    
    begin_print_data(f, get_name_of_period(periodId));
    end_print_data(f);
    
    /* Print block for all days in this week */
    for (i = weekNumber * DAYS_PER_WEEK; i < (weekNumber + 1) * DAYS_PER_WEEK; i++)
    {
        foundMatches = 0;

        for (j = 0; j < schedule->parentGen->sd->numLectures; j++)
        {
            Lecture *lect = &schedule->lectures[j];
            
            /* Validate day and period */
            if (lect->period != periodId || lect->day != i)
                continue;

            /* Validate lecture specialization */
            if (!specialization_has_lecture(sp, lect))
                continue;

            /* Check if there's already a course on this hour and day*/
            if (foundMatches)
            {
                printf("Warning: Specialization %s is doublebooked on %d, %s at %s\n",
                    sp->name,
                    lect->day,
                    get_name_of_day(lect->day),
                    get_name_of_period(lect->period));
                    
                /* Write regardless */
                fprintf(f, "<br />[+%s]", lect->assignedCourse->name);
            }
            else
            {
                begin_print_data(f, lect->assignedCourse->name);
                for (k = 0; k < lect->assignedCourse->numTeachers; k++)
                    fprintf(f, "<br />%s", lect->assignedCourse->teachers[k]->name);
                fprintf(f, "<br />%s", lect->assignedRoom->name);
                
                foundMatches++;                
            }
        }
        
        if (!foundMatches)
            begin_print_data(f, "-");

        end_print_data(f);
    }
    
    /* End test row */
    end_print_row(f);
}

/**
 *  \brief Prints a schedule for a specific specialization to a file
 *  
 *  \param [in] schedule Pointer to a schedule
 *  \param [in] sp Specialization contains information about the specialization
 *  the schedule is generated for
 *  \param [in] fileName The name of the file in which the schedule should be generated
 *  
 *  \details The final step of the schedule creation
 */
void print_schedule_to_file(Schedule *schedule, Specialization *sp, char* fileName)
{
    int i, j;
    FILE *f;

    f = fopen(fileName, "w+");
    if (!f)
        exit(ERROR_FILE_NULL_PTR);

    print_file_header(f, "AAU Scheduler");
    
    print_title(f, sp->name);

    /* Print a table for every week */
    for(i = 0; i < schedule->parentGen->sd->numWeeks; i++)
    {
        begin_print_table(f, 20);
        begin_print_row(f, "#FFFFFF");
        
        print_row_header(f, WEEK_WIDTH, "Week %d", i + 1);
        
        /* Print a row header for every day in the week */
        for (j = 0; j < DAYS_PER_WEEK; j++)
            print_row_header(f, (100.0f - WEEK_WIDTH) / (DAYS_PER_WEEK), "%s", get_name_of_day(j));
        
        end_print_row(f);

        /* Print a for with lectures for each period */
        for (j = 0; j < MAX_PERIODS; j++)
            print_period(schedule, sp, f, j, i);

        end_print_table(f);    
    }
    
    print_footer(f);
    
    fclose(f);
}