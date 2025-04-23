/*
 * Copyright (C) 2025 pdnguyen of HCMC University of Technology VNU-HCM
 */

/* Sierra release
 * Source Code License Grant: The authors hereby grant to Licensee
 * personal permission to use and modify the Licensed Source Code
 * for the sole purpose of studying while attending the course CO2018.
 */
//#include "queue.h"
#include <stdlib.h> //them vao
#include <string.h>
#include "common.h"
#include "syscall.h"//them vao
#include "stdio.h"
#include "libmem.h"
#include "queue.h"//them vao
int __sys_killall(struct pcb_t *caller, struct sc_regs* regs)
{
    char proc_name[100];
    uint32_t data;

    //hardcode for demo only
    uint32_t memrg = regs->a1;
    
    /* TODO: Get name of the target proc */
    //proc_name = libread..
    int i = 0;
    data = 0;
    while(data != -1){
        libread(caller, memrg, i, &data);
        proc_name[i]= data;
        if(data == -1) proc_name[i]='\0';
        i++;
    }
    printf("The procname retrieved from memregionid %d is \"%s\"\n", memrg, proc_name);

    /* TODO: Traverse proclist to terminate the proc
     *       stcmp to check the process match proc_name
     */
    //caller->running_list
    //caller->mlq_ready_queu
     // Duyệt các hàng đợi để tìm và hủy tiến trình phù hợp
    
    struct pcb_t *proc;

     // 1. Duyệt running_list
    if (caller->running_list) {
        int j = 0;
        for (int i = 0; i < caller->running_list->size; i++) {
            proc = caller->running_list->proc[i];
            if (strcmp(proc->path, proc_name) != 0) {
                caller->running_list->proc[j++] = proc; // Giữ lại
            } else if(proc) {
               // printf("Terminating running process %s (pid=%d)\n", proc->path, proc->pid);
                free(proc->mm);
                free(proc);
                //libfree(proc,0);
            }
        }
        caller->running_list->size = j;
    }

    // 2. Duyệt mlq_ready_queue (nếu có)
#ifdef MLQ_SCHED
    if (caller->mlq_ready_queue) {
        int j = 0;
        for (int i = 0; i < caller->mlq_ready_queue->size; i++) {
            proc = caller->mlq_ready_queue->proc[i];
            if (strcmp(proc->path, proc_name) != 0) {
                caller->mlq_ready_queue->proc[j++] = proc; // Giữ lại
            } else if(proc) {
               // printf("Terminating ready process %s (pid=%d)\n", proc->path, proc->pid);
                //libfree(proc,0);
                free(proc->mm);
                free(proc);
            }
            
        }
        caller->mlq_ready_queue->size = j;
    }
#endif

    /* TODO Maching and terminating 
     *       all processes with given
     *        name in var proc_name
     */

    return 0; 
}
