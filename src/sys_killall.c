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
void extract_proc_name(const char *path, char *proc_name) {
    const char *last_slash = strrchr(path, '/'); // Tìm dấu '/' cuối cùng
    if (last_slash) {
        strcpy(proc_name, last_slash + 1); // Sao chép phần sau dấu '/'
    } else {
        strcpy(proc_name, path); // Nếu không có '/', sao chép toàn bộ chuỗi
    }
}
int __sys_killall(struct pcb_t *caller, struct sc_regs* regs)
{
    char proc_name[100];
    char mlq_proc_name[100];
    uint32_t data;
    uint32_t memrg = regs->a1;

    // Lấy tên của tiến trình cần hủy
    int i = 0;
    data = 0;
    while (data != -1) {
        libread(caller, memrg, i, &data);
        proc_name[i] = data;
        if (data == -1) proc_name[i] = '\0';
        i++;
    }
    printf("The procname retrieved from memregionid %d is \"%s\"\n", memrg, proc_name);

    // Duyệt các danh sách để tìm và hủy tiến trình
    struct queue_t *running_list = caller->running_list;
    // Biến tạm để chứa tên tiến trình
    char proc_temp[100];
    char mlq_proc_temp[100];

    // 1. Duyệt qua running_list
    if (running_list) {
        int j = 0;
        for (int i = 0; i < running_list->size; i++) {
            struct pcb_t *proc = running_list->proc[i];
            if (proc && proc->path) {
                extract_proc_name(proc->path, proc_temp);
                if (strcmp(proc_temp, proc_name) != 0) {
                    running_list->proc[j++] = proc; // Giữ lại tiến trình không bị hủy
                } else {
                    printf("Killing process %s (pid=%d)\n", proc->path, proc->pid);
#ifdef MM_PAGING
                    libfree(proc, memrg); // Giải phóng bộ nhớ nếu có sử dụng paging
#else
                    free_data(proc, memrg); // Giải phóng dữ liệu của tiến trình
#endif     
                    proc = NULL;
                }
            }
        }
        running_list->size = j; // Cập nhật lại kích thước của running_list
    }

    // 2. Duyệt qua mlq_ready_queue
#ifdef MLQ_SCHED
    if (caller->mlq_ready_queue) {
        for (int i = 0; i<MAX_PRIO; i++) {
            int k = 0;
            for(int l=0;l<caller->mlq_ready_queue[i].size;l++){
            struct pcb_t *mlq_proc = caller->mlq_ready_queue[i].proc[l];
            if (mlq_proc && mlq_proc->path) {
                extract_proc_name(mlq_proc->path, mlq_proc_name);
                if (strcmp(mlq_proc_name, proc_name) != 0) {
                    caller->mlq_ready_queue->proc[k++] = mlq_proc; // Giữ lại tiến trình không bị hủy
                } else {
                    printf("Killing process mlq %s (pid=%d)\n", mlq_proc->path, mlq_proc->pid);
                    // Giải phóng bộ nhớ của tiến trình trong mlq_ready_queue
                   mlq_proc=NULL;
                   
                    
                }
            }
        }
        caller->mlq_ready_queue[i].size=k;
        

        }
    }
#endif

    return 0;
}

