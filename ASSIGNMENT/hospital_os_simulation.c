NAME: S.ROHINI
REG.NO: 192424413
CODE:
#include <stdio.h>
#include <stdlib.h>

#define N 5
#define DISK_N 8
#define FRAMES 3
#define REF_SIZE 16
#define MAX_CYLINDER 200
#define QUANTUM 4

typedef struct {
    int id;
    int arrival;
    int burst;
    int priority;
    int remaining;
    int completion;
    int turnaround;
    int waiting;
    int completed;
} Process;

void copyProcesses(Process source[], Process destination[]) {
    int i;

    for (i = 0; i < N; i++) {
        destination[i] = source[i];
    }
}

void printLine() {
    printf("\n------------------------------------------------------------\n");
}

void calculateMetrics(Process p[]) {
    int i;

    for (i = 0; i < N; i++) {
        p[i].turnaround = p[i].completion - p[i].arrival;
        p[i].waiting = p[i].turnaround - p[i].burst;
    }
}

void printCPUResults(Process p[], const char *algorithm) {
    int i;
    float averageWaiting = 0.0;
    float averageTurnaround = 0.0;

    printLine();
    printf("%s CPU Scheduling Results\n", algorithm);
    printLine();

    printf("Process\tAT\tBT\tPriority\tCT\tTAT\tWT\n");

    for (i = 0; i < N; i++) {
        printf("P%d\t%d\t%d\t%d\t\t%d\t%d\t%d\n",
               p[i].id,
               p[i].arrival,
               p[i].burst,
               p[i].priority,
               p[i].completion,
               p[i].turnaround,
               p[i].waiting);

        averageWaiting += p[i].waiting;
        averageTurnaround += p[i].turnaround;
    }

    printf("\nAverage Waiting Time = %.2f\n", averageWaiting / N);
    printf("Average Turnaround Time = %.2f\n",
           averageTurnaround / N);
}

void fcfs(Process original[]) {
    Process p[N];
    int i, j;
    int time = 0;

    copyProcesses(original, p);

    for (i = 0; i < N - 1; i++) {
        for (j = 0; j < N - i - 1; j++) {
            if (p[j].arrival > p[j + 1].arrival) {
                Process temp = p[j];
                p[j] = p[j + 1];
                p[j + 1] = temp;
            }
        }
    }

    printLine();
    printf("FCFS Gantt Chart:\n");

    for (i = 0; i < N; i++) {
        if (time < p[i].arrival) {
            time = p[i].arrival;
        }

        printf("| P%d ", p[i].id);
        time += p[i].burst;
        p[i].completion = time;
    }

    printf("|\n");

    calculateMetrics(p);
    printCPUResults(p, "FCFS");
}

void sjf(Process original[]) {
    Process p[N];
    int time = 0;
    int completed = 0;
    int i;
    int selected;
    int minimumBurst;

    copyProcesses(original, p);

    for (i = 0; i < N; i++) {
        p[i].completed = 0;
    }

    printLine();
    printf("SJF Non-Preemptive Gantt Chart:\n");

    while (completed < N) {
        selected = -1;
        minimumBurst = 9999;

        for (i = 0; i < N; i++) {
            if (p[i].arrival <= time &&
                p[i].completed == 0 &&
                p[i].burst < minimumBurst) {
                minimumBurst = p[i].burst;
                selected = i;
            }
        }

        if (selected == -1) {
            time++;
        } else {
            printf("| P%d ", p[selected].id);
            time += p[selected].burst;
            p[selected].completion = time;
            p[selected].completed = 1;
            completed++;
        }
    }

    printf("|\n");

    calculateMetrics(p);
    printCPUResults(p, "SJF Non-Preemptive");
}

void priorityScheduling(Process original[]) {
    Process p[N];
    int time = 0;
    int completed = 0;
    int i;
    int selected;
    int highestPriority;

    copyProcesses(original, p);

    for (i = 0; i < N; i++) {
        p[i].completed = 0;
    }

    printLine();
    printf("Priority Scheduling Gantt Chart:\n");

    while (completed < N) {
        selected = -1;
        highestPriority = 9999;

        for (i = 0; i < N; i++) {
            if (p[i].arrival <= time &&
                p[i].completed == 0 &&
                p[i].priority < highestPriority) {
                highestPriority = p[i].priority;
                selected = i;
            }
        }

        if (selected == -1) {
            time++;
        } else {
            printf("| P%d ", p[selected].id);
            time += p[selected].burst;
            p[selected].completion = time;
            p[selected].completed = 1;
            completed++;
        }
    }

    printf("|\n");

    calculateMetrics(p);
    printCPUResults(p, "Priority Non-Preemptive");
}

void roundRobin(Process original[]) {
    Process p[N];
    int queue[100];
    int front = 0;
    int rear = 0;
    int time = 0;
    int completed = 0;
    int visited[N] = {0};
    int i;
    int current;
    int executionTime;

    copyProcesses(original, p);

    for (i = 0; i < N; i++) {
        p[i].remaining = p[i].burst;
    }

    queue[rear++] = 0;
    visited[0] = 1;

    printLine();
    printf("Round Robin Gantt Chart, Quantum = %d:\n", QUANTUM);

    while (completed < N) {
        if (front == rear) {
            for (i = 0; i < N; i++) {
                if (visited[i] == 0 && p[i].arrival <= time) {
                    queue[rear++] = i;
                    visited[i] = 1;
                }
            }

            if (front == rear) {
                time++;
                continue;
            }
        }

        current = queue[front++];

        if (p[current].remaining > QUANTUM) {
            executionTime = QUANTUM;
        } else {
            executionTime = p[current].remaining;
        }

        printf("| P%d ", p[current].id);

        time += executionTime;
        p[current].remaining -= executionTime;

        for (i = 0; i < N; i++) {
            if (visited[i] == 0 && p[i].arrival <= time) {
                queue[rear++] = i;
                visited[i] = 1;
            }
        }

        if (p[current].remaining > 0) {
            queue[rear++] = current;
        } else {
            p[current].completion = time;
            completed++;
        }
    }

    printf("|\n");

    calculateMetrics(p);
    printCPUResults(p, "Round Robin");
}

int pagePresent(int frames[], int page) {
    int i;

    for (i = 0; i < FRAMES; i++) {
        if (frames[i] == page) {
            return 1;
        }
    }

    return 0;
}

void printFrames(int frames[]) {
    int i;

    printf("[");

    for (i = 0; i < FRAMES; i++) {
        if (frames[i] == -1) {
            printf("- ");
        } else {
            printf("%d ", frames[i]);
        }
    }

    printf("]");
}

void fifoPageReplacement(int reference[]) {
    int frames[FRAMES] = {-1, -1, -1};
    int pointer = 0;
    int faults = 0;
    int i;

    printLine();
    printf("FIFO Page Replacement\n");
    printLine();

    printf("Page\tFrames\t\tFault\n");

    for (i = 0; i < REF_SIZE; i++) {
        printf("%d\t", reference[i]);

        if (!pagePresent(frames, reference[i])) {
            frames[pointer] = reference[i];
            pointer = (pointer + 1) % FRAMES;
            faults++;

            printFrames(frames);
            printf("\tYes\n");
        } else {
            printFrames(frames);
            printf("\tNo\n");
        }
    }

    printf("\nTotal FIFO Page Faults = %d\n", faults);
}

void lruPageReplacement(int reference[]) {
    int frames[FRAMES] = {-1, -1, -1};
    int lastUsed[FRAMES] = {-1, -1, -1};
    int faults = 0;
    int i, j;
    int position;
    int leastUsedIndex;

    printLine();
    printf("LRU Page Replacement\n");
    printLine();

    printf("Page\tFrames\t\tFault\n");

    for (i = 0; i < REF_SIZE; i++) {
        position = -1;

        for (j = 0; j < FRAMES; j++) {
            if (frames[j] == reference[i]) {
                position = j;
                break;
            }
        }

        printf("%d\t", reference[i]);

        if (position != -1) {
            lastUsed[position] = i;
            printFrames(frames);
            printf("\tNo\n");
        } else {
            faults++;

            position = -1;

            for (j = 0; j < FRAMES; j++) {
                if (frames[j] == -1) {
                    position = j;
                    break;
                }
            }

            if (position == -1) {
                leastUsedIndex = 0;

                for (j = 1; j < FRAMES; j++) {
                    if (lastUsed[j] < lastUsed[leastUsedIndex]) {
                        leastUsedIndex = j;
                    }
                }

                position = leastUsedIndex;
            }

            frames[position] = reference[i];
            lastUsed[position] = i;

            printFrames(frames);
            printf("\tYes\n");
        }
    }

    printf("\nTotal LRU Page Faults = %d\n", faults);
}

void optimalPageReplacement(int reference[]) {
    int frames[FRAMES] = {-1, -1, -1};
    int faults = 0;
    int i, j, k;
    int position;
    int farthest;
    int replaceIndex;
    int nextUse;

    printLine();
    printf("Optimal Page Replacement\n");
    printLine();

    printf("Page\tFrames\t\tFault\n");

    for (i = 0; i < REF_SIZE; i++) {
        printf("%d\t", reference[i]);

        if (pagePresent(frames, reference[i])) {
            printFrames(frames);
            printf("\tNo\n");
            continue;
        }

        faults++;
        position = -1;

        for (j = 0; j < FRAMES; j++) {
            if (frames[j] == -1) {
                position = j;
                break;
            }
        }

        if (position != -1) {
            frames[position] = reference[i];
            printFrames(frames);
            printf("\tYes\n");
            continue;
        }

        farthest = -1;
        replaceIndex = -1;

        for (j = 0; j < FRAMES; j++) {
            nextUse = 9999;

            for (k = i + 1; k < REF_SIZE; k++) {
                if (frames[j] == reference[k]) {
                    nextUse = k;
                    break;
                }
            }

            if (nextUse > farthest) {
                farthest = nextUse;
                replaceIndex = j;
            }
        }

        frames[replaceIndex] = reference[i];

        printFrames(frames);
        printf("\tYes\n");
    }

    printf("\nTotal Optimal Page Faults = %d\n", faults);
}

int absoluteValue(int value) {
    if (value < 0) {
        return -value;
    }

    return value;
}

void diskFCFS(int queue[]) {
    int current = 100;
    int movement = 0;
    int i;

    printLine();
    printf("FCFS Disk Scheduling\n");
    printLine();

    printf("Sequence: %d", current);

    for (i = 0; i < DISK_N; i++) {
        movement += absoluteValue(current - queue[i]);
        current = queue[i];
        printf(" -> %d", current);
    }

    printf("\nTotal Head Movement = %d cylinders\n", movement);
}

void diskSSTF(int queue[]) {
    int visited[DISK_N] = {0};
    int current = 100;
    int movement = 0;
    int completed = 0;
    int i;
    int selected;
    int minimumDistance;

    printLine();
    printf("SSTF Disk Scheduling\n");
    printLine();

    printf("Sequence: %d", current);

    while (completed < DISK_N) {
        selected = -1;
        minimumDistance = 9999;

        for (i = 0; i < DISK_N; i++) {
            if (visited[i] == 0 &&
                absoluteValue(queue[i] - current) < minimumDistance) {
                minimumDistance = absoluteValue(queue[i] - current);
                selected = i;
            }
        }

        movement += absoluteValue(current - queue[selected]);
        current = queue[selected];
        visited[selected] = 1;
        completed++;

        printf(" -> %d", current);
    }

    printf("\nTotal Head Movement = %d cylinders\n", movement);
}

void sortAscending(int array[], int size) {
    int i, j, temp;

    for (i = 0; i < size - 1; i++) {
        for (j = 0; j < size - i - 1; j++) {
            if (array[j] > array[j + 1]) {
                temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

void diskSCAN(int queue[]) {
    int lower[DISK_N];
    int upper[DISK_N];
    int lowerCount = 0;
    int upperCount = 0;
    int current = 100;
    int movement = 0;
    int i;

    for (i = 0; i < DISK_N; i++) {
        if (queue[i] < current) {
            lower[lowerCount++] = queue[i];
        } else {
            upper[upperCount++] = queue[i];
        }
    }

    sortAscending(lower, lowerCount);
    sortAscending(upper, upperCount);

    printLine();
    printf("SCAN Disk Scheduling, Direction Toward 0\n");
    printLine();

    printf("Sequence: %d", current);

    for (i = lowerCount - 1; i >= 0; i--) {
        movement += absoluteValue(current - lower[i]);
        current = lower[i];
        printf(" -> %d", current);
    }

    movement += absoluteValue(current - 0);
    current = 0;
    printf(" -> %d", current);

    for (i = 0; i < upperCount; i++) {
        movement += absoluteValue(current - upper[i]);
        current = upper[i];
        printf(" -> %d", current);
    }

    printf("\nTotal Head Movement = %d cylinders\n", movement);
}

void diskCSCAN(int queue[]) {
    int lower[DISK_N];
    int upper[DISK_N];
    int lowerCount = 0;
    int upperCount = 0;
    int current = 100;
    int movement = 0;
    int i;

    for (i = 0; i < DISK_N; i++) {
        if (queue[i] < current) {
            lower[lowerCount++] = queue[i];
        } else {
            upper[upperCount++] = queue[i];
        }
    }

    sortAscending(lower, lowerCount);
    sortAscending(upper, upperCount);

    printLine();
    printf("C-SCAN Disk Scheduling, Direction Toward 0\n");
    printLine();

    printf("Sequence: %d", current);

    for (i = lowerCount - 1; i >= 0; i--) {
        movement += absoluteValue(current - lower[i]);
        current = lower[i];
        printf(" -> %d", current);
    }

    movement += absoluteValue(current - 0);
    current = 0;
    printf(" -> %d", current);

    movement += absoluteValue(current - MAX_CYLINDER);
    current = MAX_CYLINDER;
    printf(" -> %d", current);

    for (i = upperCount - 1; i >= 0; i--) {
        movement += absoluteValue(current - upper[i]);
        current = upper[i];
        printf(" -> %d", current);
    }

    printf("\nTotal Head Movement = %d cylinders\n", movement);
}

int main() {
    Process processes[N] = {
        {1, 0, 8, 2, 0, 0, 0, 0, 0},
        {2, 1, 4, 3, 0, 0, 0, 0, 0},
        {3, 2, 9, 4, 0, 0, 0, 0, 0},
        {4, 3, 5, 1, 0, 0, 0, 0, 0},
        {5, 4, 2, 5, 0, 0, 0, 0, 0}
    };

    int referenceString[REF_SIZE] = {
        1, 2, 3, 1, 4, 2, 5, 1,
        2, 3, 6, 2, 1, 4, 5, 2
    };

    int diskQueue[DISK_N] = {
        23, 89, 132, 42, 187, 11, 175, 56
    };

    printf("\n============================================================\n");
    printf("HOSPITAL PATIENT QUEUE MANAGEMENT SYSTEM\n");
    printf("OPERATING SYSTEMS ALGORITHM SIMULATION\n");
    printf("============================================================\n");

    fcfs(processes);
    sjf(processes);
    priorityScheduling(processes);
    roundRobin(processes);

    fifoPageReplacement(referenceString);
    lruPageReplacement(referenceString);
    optimalPageReplacement(referenceString);

    diskFCFS(diskQueue);
    diskSSTF(diskQueue);
    diskSCAN(diskQueue);
    diskCSCAN(diskQueue);

    printLine();
    printf("Simulation Completed Successfully.\n");
    printLine();

    return 0;
}
OUTPUT:
============================================================
HOSPITAL PATIENT QUEUE MANAGEMENT SYSTEM
OPERATING SYSTEMS ALGORITHM SIMULATION
============================================================

------------------------------------------------------------
FCFS Gantt Chart:
| P1 | P2 | P3 | P4 | P5 |

------------------------------------------------------------
FCFS CPU Scheduling Results

------------------------------------------------------------
Process	AT	BT	Priority	CT	TAT	WT
P1	0	8	2		8	8	0
P2	1	4	3		12	11	7
P3	2	9	4		21	19	10
P4	3	5	1		26	23	18
P5	4	2	5		28	24	22

Average Waiting Time = 11.40
Average Turnaround Time = 17.00

------------------------------------------------------------
SJF Non-Preemptive Gantt Chart:
| P1 | P5 | P2 | P4 | P3 |

------------------------------------------------------------
SJF Non-Preemptive CPU Scheduling Results

------------------------------------------------------------
Process	AT	BT	Priority	CT	TAT	WT
P1	0	8	2		8	8	0
P2	1	4	3		14	13	9
P3	2	9	4		28	26	17
P4	3	5	1		19	16	11
P5	4	2	5		10	6	4

Average Waiting Time = 8.20
Average Turnaround Time = 13.80

------------------------------------------------------------
Priority Scheduling Gantt Chart:
| P1 | P4 | P2 | P3 | P5 |

------------------------------------------------------------
Priority Non-Preemptive CPU Scheduling Results

------------------------------------------------------------
Process	AT	BT	Priority	CT	TAT	WT
P1	0	8	2		8	8	0
P2	1	4	3		17	16	12
P3	2	9	4		26	24	15
P4	3	5	1		13	10	5
P5	4	2	5		28	24	22

Average Waiting Time = 10.80
Average Turnaround Time = 16.40

------------------------------------------------------------
Round Robin Gantt Chart, Quantum = 4:
| P1 | P2 | P3 | P4 | P5 | P1 | P3 | P4 | P3 |

------------------------------------------------------------
Round Robin CPU Scheduling Results

------------------------------------------------------------
Process	AT	BT	Priority	CT	TAT	WT
P1	0	8	2		22	22	14
P2	1	4	3		8	7	3
P3	2	9	4		28	26	17
P4	3	5	1		27	24	19
P5	4	2	5		18	14	12

Average Waiting Time = 13.00
Average Turnaround Time = 18.60

------------------------------------------------------------
FIFO Page Replacement

------------------------------------------------------------
Page	Frames		Fault
1	[1 - - ]	Yes
2	[1 2 - ]	Yes
3	[1 2 3 ]	Yes
1	[1 2 3 ]	No
4	[4 2 3 ]	Yes
2	[4 2 3 ]	No
5	[4 5 3 ]	Yes
1	[4 5 1 ]	Yes
2	[2 5 1 ]	Yes
3	[2 3 1 ]	Yes
6	[2 3 6 ]	Yes
2	[2 3 6 ]	No
1	[1 3 6 ]	Yes
4	[1 4 6 ]	Yes
5	[1 4 5 ]	Yes
2	[2 4 5 ]	Yes

Total FIFO Page Faults = 13

------------------------------------------------------------
LRU Page Replacement

------------------------------------------------------------
Page	Frames		Fault
1	[1 - - ]	Yes
2	[1 2 - ]	Yes
3	[1 2 3 ]	Yes
1	[1 2 3 ]	No
4	[1 4 3 ]	Yes
2	[1 4 2 ]	Yes
5	[5 4 2 ]	Yes
1	[5 1 2 ]	Yes
2	[5 1 2 ]	No
3	[3 1 2 ]	Yes
6	[3 6 2 ]	Yes
2	[3 6 2 ]	No
1	[1 6 2 ]	Yes
4	[1 4 2 ]	Yes
5	[1 4 5 ]	Yes
2	[2 4 5 ]	Yes

Total LRU Page Faults = 13

------------------------------------------------------------
Optimal Page Replacement

------------------------------------------------------------
Page	Frames		Fault
1	[1 - - ]	Yes
2	[1 2 - ]	Yes
3	[1 2 3 ]	Yes
1	[1 2 3 ]	No
4	[1 2 4 ]	Yes
2	[1 2 4 ]	No
5	[1 2 5 ]	Yes
1	[1 2 5 ]	No
2	[1 2 5 ]	No
3	[1 2 3 ]	Yes
6	[1 2 6 ]	Yes
2	[1 2 6 ]	No
1	[1 2 6 ]	No
4	[4 2 6 ]	Yes
5	[5 2 6 ]	Yes
2	[5 2 6 ]	No

Total Optimal Page Faults = 9

------------------------------------------------------------
FCFS Disk Scheduling

------------------------------------------------------------
Sequence: 100 -> 23 -> 89 -> 132 -> 42 -> 187 -> 11 -> 175 -> 56
Total Head Movement = 880 cylinders

------------------------------------------------------------
SSTF Disk Scheduling

------------------------------------------------------------
Sequence: 100 -> 89 -> 56 -> 42 -> 23 -> 11 -> 132 -> 175 -> 187
Total Head Movement = 265 cylinders

------------------------------------------------------------
SCAN Disk Scheduling, Direction Toward 0

------------------------------------------------------------
Sequence: 100 -> 89 -> 56 -> 42 -> 23 -> 11 -> 0 -> 132 -> 175 -> 187
Total Head Movement = 287 cylinders

------------------------------------------------------------
C-SCAN Disk Scheduling, Direction Toward 0

------------------------------------------------------------
Sequence: 100 -> 89 -> 56 -> 42 -> 23 -> 11 -> 0 -> 200 -> 187 -> 175 -> 132
Total Head Movement = 368 cylinders

------------------------------------------------------------
Simulation Completed Successfully.

------------------------------------------------------------


=== Code Execution Successful ===
