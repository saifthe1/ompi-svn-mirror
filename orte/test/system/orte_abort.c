/* -*- C -*-
 *
 * $HEADER$
 *
 * A program that just spins, with vpid 3 aborting - provides mechanism for testing
 * abnormal program termination
 */

#include <stdio.h>
#include <unistd.h>

#include "orte/runtime/runtime.h"
#include "orte/util/proc_info.h"

int main(int argc, char* argv[])
{

    int i, rc;
    double pi;
    pid_t pid;
    char hostname[500];

    if (0 > (rc = orte_init(ORTE_NON_INFRASTRUCTURE))) {
        fprintf(stderr, "orte_abort: couldn't init orte - error code %d\n", rc);
        return rc;
    }
    pid = getpid();
    gethostname(hostname, 500);

    printf("orte_abort: Name %s Host: %s Pid %ld\n", ORTE_NAME_PRINT(orte_process_info.my_name),
              hostname, (long)pid);
    
    i = 0;
    while (1) {
        i++;
        pi = i / 3.14159256;
        if (i > 10000) i = 0;
        if ((orte_process_info.my_name->vpid == 3 || 
             (orte_process_info.num_procs <= 3 && orte_process_info.my_name->vpid == 0))
            && i == 9995) {
            orte_abort(1, true);
        }
    }

    return 0;
}
