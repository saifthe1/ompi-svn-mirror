/* -*- C -*-
 * 
 * $HEADER$
 *
 */

#include "ompi_config.h"

#include <sys/types.h>
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#include <sys/time.h>
#include <sys/wait.h>
#include <pwd.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "mca/pcm/pcm.h"
#include "mca/pcm/base/base.h"
#include "mca/pcm/rsh/src/pcm_rsh.h"
#include "runtime/runtime_types.h"
#include "util/output.h"
#include "util/argv.h"


#define BOOTAGENT "mca_pcm_rsh_bootproxy"
#define PRS_BUFSIZE 1024

static int internal_spawn_proc(int jobid, ompi_rte_node_schedule_t *sched,
                               ompi_list_t *nodelist);


bool
mca_pcm_rsh_can_spawn(void)
{
    /* we can always try to rsh some more...  Might not always work as
     * the caller hopes
     */
    return true;
}


int
mca_pcm_rsh_spawn_procs(int jobid, ompi_list_t *schedlist)
{
    ompi_list_item_t *sched_item, *node_item;
    ompi_rte_node_schedule_t *sched;
    ompi_list_t launch;
    ompi_list_t done;
    int ret, i;
    int width = 1;

    OBJ_CONSTRUCT(&launch, ompi_list_t);
    OBJ_CONSTRUCT(&done, ompi_list_t);

    for (sched_item = ompi_list_get_first(schedlist) ;
         sched_item != ompi_list_get_end(schedlist) ;
         sched_item = ompi_list_get_next(sched_item)) {
        sched = (ompi_rte_node_schedule_t*) sched_item;

        /*
         * make sure I'm the first node in the list and then start our
         * deal.  We rsh me just like everyone else so that we don't
         * have any unexpected environment oddities...
         */
        /* BWB - do front of list check! */
        node_item = ompi_list_get_first(sched->nodelist);

        while (node_item != ompi_list_get_end(sched->nodelist)) {
            /* find enough entries for this slice to go */
            for (i = 0 ;
                 i < width && node_item != ompi_list_get_end(sched->nodelist) ;
                 node_item = ompi_list_get_next(node_item)) { }
            /* if we don't have anyone, get us out of here.. */
            if (i ==  0) {
                continue;
            }

            /* make a launch list */
            ompi_list_splice(&launch, ompi_list_get_end(&launch),
                             sched->nodelist,
                             ompi_list_get_first(sched->nodelist),
                             node_item);

            /* do the launch to the first node in the list, passing
               him the rest of the list */
            ret = internal_spawn_proc(jobid, sched, &launch);
            if  (OMPI_SUCCESS != ret) {
                /* well, crap!  put ourselves back together, I guess.
                   Should call killjob */
                ompi_list_join(&done, ompi_list_get_end(&done), &launch);
                ompi_list_join(sched->nodelist, 
                               ompi_list_get_first(sched->nodelist),
                               &done);
                return ret;
            }

            /* copy the list over to the done part */
            ompi_list_join(&done, ompi_list_get_end(&done), &launch);
        }
    }

    OBJ_DESTRUCT(&done);
    OBJ_DESTRUCT(&launch);

    return OMPI_SUCCESS;
}


static int
internal_need_profile(ompi_rte_node_allocation_t *start_node,
                      int stderr_is_error, bool *needs_profile)
{
    struct passwd *p;
    char shellpath[PRS_BUFSIZE];
    char** cmdv = NULL;
    char *cmd0 = NULL;
    int cmdc = 0;
    char *printable = NULL;
    char *username = NULL;
    int ret;

    /*
     * Figure out if we need to source the .profile on the other side.
     *
     * The following logic is used:
     *
     * if mca_pcm_rsh_no_profile is 1, don't do profile
     * if mca_pcm_rsh_fast is 1, remote shell is assumed same as local
     * if shell is sh/ksh, run profile, otherwise don't
     */
    if (1 == mca_pcm_rsh_no_profile) {
        *needs_profile = false;
        return OMPI_SUCCESS;
    }

    if (1 == mca_pcm_rsh_fast) {
        p = getpwuid(getuid());
        if (NULL == p) return OMPI_ERROR;
            
        ompi_output_verbose(5, mca_pcm_rsh_output, 
                            "fast boot mode - "
                            "assuming same shell on remote nodes");
        ompi_output_verbose(5, mca_pcm_rsh_output, 
                            "getpwuid: got local shell %s", p->pw_shell);
        strncpy(shellpath, p->pw_shell, PRS_BUFSIZE - 1);
        shellpath[PRS_BUFSIZE - 1] = '\0';
    } else {
        /* we have to look at the other side  and get our shell */
        username = mca_pcm_base_get_username(start_node);

        cmdv = ompi_argv_split(mca_pcm_rsh_agent, ' ');
        cmdc = ompi_argv_count(cmdv);

        ompi_argv_append(&cmdc, &cmdv, start_node->hostname);
        if (NULL != username) {
            ompi_argv_append(&cmdc, &cmdv, "-l");
            ompi_argv_append(&cmdc, &cmdv, username);
        }

        ompi_argv_append(&cmdc, &cmdv, "echo $SHELL");
        printable = ompi_argv_join(cmdv, ' ');
        ompi_output_verbose(5, mca_pcm_rsh_output,
                            "attempting to execute: %s", printable);

        cmd0 = strdup(cmdv[0]);
        shellpath[sizeof(shellpath) - 1] = '\0';
        if (mca_pcm_base_ioexecvp(cmdv, 0, shellpath,
                                  sizeof(shellpath) - 1, 
                                  stderr_is_error)) {
            if (errno == EFAULT) {
                /* BWB - show_help */
                printf("show_help: something on stderr: %s %s %s",
                       start_node->hostname, cmd0, printable);
            } else {
                /* BWB - show_help */
                printf("show_help: fail to rsh: %s %s %s",
                       start_node->hostname, cmd0, printable);
            }

            ret = OMPI_ERROR;
            goto cleanup;
        }

        if ('\n' == shellpath[strlen(shellpath) - 1]) {
            shellpath[strlen(shellpath) - 1] = '\0';
        }
        ompi_output_verbose(5, mca_pcm_rsh_output,
                            "remote shell %s", shellpath);

        if (NULL == strstr(p->pw_shell, "csh") &&
            NULL == strstr(p->pw_shell, "bash")) {
            /* we are neither csh-derived nor bash.  This probably
               means old-school sh or ksh.  Either way, we
               probably want to run .profile... */
            *needs_profile = true;
        }
    }

    ret = OMPI_SUCCESS;

cleanup:

    /* free up everything we used on the way */
    if (NULL != printable) free(printable);
    if (NULL != cmd0) free(cmd0);
    if (NULL != username) free(username);
    ompi_argv_free(cmdv);
    cmdv = NULL;
    cmdc = 0;

    return ret;
}


static int
internal_spawn_proc(int jobid, ompi_rte_node_schedule_t *sched,
                    ompi_list_t *nodelist)
{
    int kidstdin[2];            /* child stdin pipe */
    bool needs_profile = false;
    ompi_rte_node_allocation_t *start_node;
    char** cmdv = NULL;
    char *cmd0 = NULL;
    int cmdc = 0;
    char *printable = NULL;
    int stderr_is_error = mca_pcm_rsh_ignore_stderr == 0 ? 1 : 0;
    char *username = NULL;
    int ret;
    pid_t pid;
    FILE *fp;
    int status;			/* exit status */
    int i;

    start_node = (ompi_rte_node_allocation_t*) ompi_list_get_first(nodelist);

    /*
     * Check to see if we need to do the .profile thing
     */
    ret = internal_need_profile(start_node, stderr_is_error,
                                &needs_profile);
    if (OMPI_SUCCESS != ret) {
        goto cleanup;
    }
    

    /*
     * Build up start array
     */

    /* build up the rsh command part */
    cmdv = ompi_argv_split(mca_pcm_rsh_agent, ' ');
    cmdc = ompi_argv_count(cmdv);

    ompi_argv_append(&cmdc, &cmdv, start_node->hostname);
    username = mca_pcm_base_get_username(start_node);
    if (NULL != username) {
        ompi_argv_append(&cmdc, &cmdv, "-l");
        ompi_argv_append(&cmdc, &cmdv, username);
    }

    /* add the start of .profile thing if required */
    if (needs_profile) {
        ompi_argv_append(&cmdc, &cmdv, "( ! [ -e ./.profile] || . ./.profile;");
    }

    /* build the command to start */
    ompi_argv_append(&cmdc, &cmdv, BOOTAGENT);
    /* BWB - turn on debugging for now */
    ompi_argv_append(&cmdc, &cmdv, "-v");

    /* add the end of the .profile thing if required */
    if (needs_profile) {
        ompi_argv_append(&cmdc, &cmdv, ")");
    }

    /*
     * Start the process already
     */
    
    if (pipe(kidstdin)) {
        ret = OMPI_ERROR;
        goto cleanup;
    }

    if ((pid = fork()) < 0) {
        ret = OMPI_ERROR;
        goto cleanup;
    } else if (pid == 0) {
        /* child */

        if ((dup2(kidstdin[1], 0) < 0)) {
            perror(cmdv[0]);
            exit(errno);
        }

        if (close(kidstdin[0]) || close(kidstdin[1])) {
            perror(cmdv[0]);
            exit(errno);
        }

        /* Ensure that we close all other file descriptors */

        for (i = 3; i < FD_SETSIZE; i++)
            close(i);

        execvp(cmdv[0], cmdv);
        exit(errno);

    } else {
        /* parent */

        if (close(kidstdin[1])) {
            kill(pid, SIGTERM);
            ret = OMPI_ERROR;
            goto proc_cleanup;
        }

        /* send our stuff down the wire */
        fp = fdopen(kidstdin[0], "w");
        ret = mca_pcm_base_send_schedule(fp, jobid, sched, nodelist);
        fclose(fp);
        if (OMPI_SUCCESS != ret) {
            kill(pid, SIGTERM);
            goto proc_cleanup;
        }
    }
    
    ret = OMPI_SUCCESS;

 proc_cleanup:
    /* Wait for the command to exit.  */
  do {
    if (waitpid(pid, &status, 0) < 0) {
        ret = OMPI_ERROR;
    }
  } while (!WIFEXITED(status));

  if (WEXITSTATUS(status)) {
    errno = WEXITSTATUS(status);

    ret = OMPI_ERROR;
  }

 cleanup:
    /* free up everything we used on the way */
    if (NULL != printable) free(printable);
    if (NULL != cmd0) free(cmd0);
    if (NULL != username) free(username);
    ompi_argv_free(cmdv);
    cmdv = NULL;
    cmdc = 0;

    return ret;
}
