#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef uint32_t u32;
typedef int32_t i32;

struct process {
  u32 pid;
  u32 arrival_time;
  u32 burst_time;

  TAILQ_ENTRY(process) pointers;

  /* Additional fields here */
  u32 remaining_time; // time left in "execution"
  u32 start_exec_time; // time at which "execution" starts
  u32 waiting_time; // end_time - arrival_time - burst_time
  u32 response_time; // start_exec_time – arrival_time
  /* End of "Additional fields here" */
};

TAILQ_HEAD(process_list, process);

u32 next_int(const char **data, const char *data_end) {
  u32 current = 0;
  bool started = false;
  while (*data != data_end) {
    char c = **data;

    if (c < 0x30 || c > 0x39) {
      if (started) {
	return current;
      }
    }
    else {
      if (!started) {
	current = (c - 0x30);
	started = true;
      }
      else {
	current *= 10;
	current += (c - 0x30);
      }
    }

    ++(*data);
  }

  printf("Reached end of file while looking for another integer\n");
  exit(EINVAL);
}

u32 next_int_from_c_str(const char *data) {
  char c;
  u32 i = 0;
  u32 current = 0;
  bool started = false;
  while ((c = data[i++])) {
    if (c < 0x30 || c > 0x39) {
      exit(EINVAL);
    }
    if (!started) {
      current = (c - 0x30);
      started = true;
    }
    else {
      current *= 10;
      current += (c - 0x30);
    }
  }
  return current;
}

void init_processes(const char *path,
                    struct process **process_data,
                    u32 *process_size)
{
  int fd = open(path, O_RDONLY);
  if (fd == -1) {
    int err = errno;
    perror("open");
    exit(err);
  }

  struct stat st;
  if (fstat(fd, &st) == -1) {
    int err = errno;
    perror("stat");
    exit(err);
  }

  u32 size = st.st_size;
  const char *data_start = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (data_start == MAP_FAILED) {
    int err = errno;
    perror("mmap");
    exit(err);
  }

  const char *data_end = data_start + size;
  const char *data = data_start;
  

  *process_size = next_int(&data, data_end);

  *process_data = calloc(sizeof(struct process), *process_size);
  if (*process_data == NULL) {
    int err = errno;
    perror("calloc");
    exit(err);
  }

  for (u32 i = 0; i < *process_size; ++i) {
    (*process_data)[i].pid = next_int(&data, data_end);
    (*process_data)[i].arrival_time = next_int(&data, data_end);
    (*process_data)[i].burst_time = next_int(&data, data_end);
  }
  
  munmap((void *)data, size);
  close(fd);
}

int main(int argc, char *argv[])
{
  if (argc != 3) {
    return EINVAL;
  }
  struct process *data;
  u32 size;
  init_processes(argv[1], &data, &size);

  u32 quantum_length = next_int_from_c_str(argv[2]);

  struct process_list list;
  TAILQ_INIT(&list);

  u32 total_waiting_time = 0;
  u32 total_response_time = 0;

  /* Your code here */
  printf("PID\tArrival\tBurst\n");
  for(u32 i=0; i <size; ++i){
    printf("%d\t", (int)data[i].pid);
    printf("%d\t", (int)data[i].arrival_time);
    printf("%d\n", (int)data[i].burst_time);
  }

  // begin simulating time
  int time = 0;
  int numCompleted = 0;
  int q = quantum_length;
  struct process *p;
  while (time < 20 && numCompleted < size){
    // if arrival time, add to the RR queue
    printf("time = %d\n", time);
    for (u32 i=0; i<size; ++i){
      if (time == data[i].arrival_time){
        p = &data[i];
        TAILQ_INSERT_TAIL(&list, p, pointers);
        p->remaining_time = p->burst_time;
        printf("PID %d arrived\n", p->pid);
      }
    }

    // "execute" first node in RR queue
    struct process *head = TAILQ_FIRST(&list);
    printf("PID %d executing with %d time remaining\n", head->pid, head->remaining_time);
    if (head->remaining_time == head->burst_time){
      head->start_exec_time = time; // first time executing
    }
    head->remaining_time--;
    q--;
    printf("q = %d\n", q);

    // check if "execution" is finished, then set waiting_time
    if (head->remaining_time == 0){
      head->waiting_time = time - head->arrival_time - head->burst_time;
      printf("PID %d finished executing with %d wait time\n", head->pid, head->waiting_time);
      TAILQ_REMOVE(&list, head, pointers);
      numCompleted++;
    }
    // check if quantum is reached, then move node to the back of the queue
    else if (q == 0){
      printf("ROTATING QUEUE\n");
      TAILQ_REMOVE(&list, head, pointers);
      TAILQ_INSERT_TAIL(&list, head, pointers);
      q = quantum_length; // reset quantum
    }
    printf("==========\n");
    time++;
  }

  /* End of "Your code here" */

  printf("Average waiting time: %.2f\n", (float) total_waiting_time / (float) size);
  printf("Average response time: %.2f\n", (float) total_response_time / (float) size);

  free(data);
  return 0;
}
