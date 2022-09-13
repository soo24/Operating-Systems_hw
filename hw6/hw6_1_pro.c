/*

	This program drops down multiple balls by multiple threads.
	There is a critical region specified by the top and bottom coordinates.

	Mission: 
		[IMPORTANT] Read this code carefully and fully understand.
		Modify the code to ensure that only one ball can enter the critical region at a time.

	Hints) 
		1. Pass the top and bottom coordinates of the critical section to the threads through the ThreadParam structure
		2. Create and initialize pthread_mutex_t as a global variable
		3. When a ball enters the critical region, lock the mutex (entry section)
		4. When a ball leaves the critical region, unlock the mutex (exit section)
		5. If the thread breaks the loop while the ball is in the critical region, unlock the mutex to release other threads.
		6. After the threads finishes, destroy all mutexes.

	이 프로그램은 여러 개의 스레드만큼 여러 개의 볼을 떨어뜨립니다.
    상단 및 하단 좌표에 의해 지정된 임계 영역이 있습니다.
    미션:
        [중요] 이 코드를 주의 깊게 읽고 완전히 이해하십시오.
        한 번에 하나의 볼만 임계 영역에 들어갈 수 있도록 코드를 수정합니다.
    힌트)
        1. ThreadParam 구조를 통해 중요 섹션의 상단 및 하단 좌표를 스레드에 전달합니다.
        2. 글로벌 변수로 pthread_mutex_t 생성 및 초기화
        3. 볼이 임계 영역에 들어오면 뮤텍스를 잠급니다(엔트리 섹션).
        4. 볼이 임계 영역을 벗어나면 뮤텍스의 잠금을 해제합니다(섹션 종료).
        5. 볼이 임계 영역에 있는 동안 나사산이 루프를 끊으면 뮤텍스의 잠금을 해제하여 다른 나사산을 해제합니다.
        6. 스레드가 완료되면 모든 뮤텍스를 제거합니다.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <unistd.h>

#include <pthread.h>

#include "Console.h"

#define TRUE 1
#define FALSE 0

#define MAX_THREAD 30

typedef struct {
	int thread_idx;
	int no_thread;
	int screen_height;
	int x;
	int delay;

	// TO DO: add fields to pass the vertical coordinates of the critical region
	//필드(구조체 원소)를 추가하여 임계 영역의 수직 좌표를 통과합니다.
	int stop;
	int restart;

} ThreadParam;
int thread_cont = TRUE;

void* ThreadFn(void *vParam);

// TO DO: declare and initialize a mutex as a global variable
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[])
{
	srand(time(NULL));
	clrscr();

	int no_thread = 0;
	if(argc > 1)
		no_thread = atoi(argv[1]);

	if(no_thread > MAX_THREAD)
		no_thread = MAX_THREAD;
	else if(no_thread == 0)
		no_thread = 5;

	int screen_width = getWindowWidth();
	int screen_height = getWindowHeight() - 3;

	// the top and bottom coordinates of the critical region
	int critical_top = screen_height / 3;
	int critical_bottom = screen_height * 2 / 3;

	// mark the critical region by horizontal lines
	DrawLine(1, critical_top, screen_width, critical_top, '-');
	DrawLine(1, critical_bottom, screen_width, critical_bottom, '-');
	
	pthread_t tid[MAX_THREAD];
	ThreadParam param[MAX_THREAD];

	for(int i = 0; i < no_thread; i++){
		param[i].thread_idx = i;
		param[i].no_thread = no_thread;
		param[i].screen_height = screen_height;
		param[i].x = screen_width * (i + 1) / (no_thread + 1);
		param[i].delay = rand() % 300;

		// TO DO: add code to store the vertical coordinates of the critical region in param[i]
		//param[i]에 임계 영역의 수직 좌표를 저장하는 코드를 추가합니다.
		param[i].stop = critical_top;
		param[i].restart = critical_bottom;

		pthread_create(&tid[i], NULL, ThreadFn, &param[i]);
	}

	PrintXY(1, screen_height + 1, "Press Enter to quit!\n");
	getchar();

	// termainte the threads
	thread_cont = FALSE;

	for(int i = 0; i < no_thread; i++)
		pthread_join(tid[i], NULL);

	// TO DO: destroy mutex
	pthread_mutex_destroy(&mutex);

	clrscr();
	gotoxy(1, 1);
	printf("Bye!\n");

	return 0;
}

void* ThreadFn(void *vParam)
{
	ThreadParam *param = (ThreadParam*)vParam;

	int y = 1;
	int oldy = 1;
	while(thread_cont){
		// TO DO: implement entry section here 
		if(y == param->stop) {
			pthread_mutex_lock(&mutex);
		}

		gotoxy(param->x, oldy);
		putchar(' ');

		gotoxy(param->x, y);
		putchar('*');

		fflush(stdout);
		
		// TO DO: implement exit section here 
		if(y == param ->restart) pthread_mutex_unlock(&mutex);

		oldy = y;
		y++;
		if(y > param->screen_height)
			y = 1;

		gotoxy(1, 1);
		fflush(stdout);
		usleep(param->delay * 1000);
	}

	// TO DO: if current broke loop in the critical region, unlock mutex
	pthread_mutex_unlock(&mutex);

	return NULL;
}

