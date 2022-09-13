

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <unistd.h>

#include <pthread.h>

#include "Console.h"
#include "hw7_1.h"

static char StateName[3][32] = {
	" thinking ",
	"==EATING==",
	" hunggry  "						// not used in this program
};

void Philosophers_Init(Philosophers *dp, int no_phil)
{
	dp->no_phil = no_phil;

	// TO DO: initialize mutex and condition variables
	for(int i=0;i<no_phil;i++){
		dp->state[i]=THINKING;
		//pthread_mutex_init(dp->self[i],NULL);
		pthread_mutex_init(&dp->mutex[i],NULL);
		pthread_cond_init(dp->self[i],NULL);
	}
	
	//pthread_mutex_init(&dp->mutex,NULL);
	//pthread_cond_init(dp->self,NULL);

}

void Philosophers_Destroy(Philosophers *dp)
{
	// TO DO: destroy mutex and condition vars
	pthread_mutex_destroy(&dp->mutex);
	pthread_cond_destroy(dp->self);

	for(int i = 0; i < dp->no_phil; i++){
		pthread_mutex_destroy(&dp->self[i]);
	}

	dp->no_phil = 0;		// for safety
}

void Philosophers_PickUp(Philosophers *dp, int i)
{
	// TO DO: implement this function
	// 		this function should be mutually exclusive
	dp->state[i] = HUNGRY; 

	if(i%2==0){
		pthread_mutex_lock(&dp->self[i]);
		pthread_mutex_lock(&dp->self[(i+1)%5]);
	}
	else{
		pthread_mutex_lock(&dp->self[(i+1)%5]);
		pthread_mutex_lock(&dp->self[i]);
	}

	Philosophers_Test(dp,i);
	if (dp->state[i] != EATING)
		pthread_cond_wait(dp->self,&dp->mutex);

	
	//dp->self[i].wait();
	
	
	/*if(i%2==0){
		pthread_mutex_lock(&dp->self[i]);
		pthread_mutex_lock(&dp->self[(i+1)%5]);
	}
	else{
		pthread_mutex_lock(&dp->self[(i+1)%5]);
		pthread_mutex_lock(&dp->self[i]);
	}
	usleep(100);
*/
	dp->state[i] = EATING;	

}

void Philosophers_PutDown(Philosophers *dp, int i)
{
	// TO DO: implement this function
	// 		this function should be mutually exclusive

	dp->state[i] = THINKING; 
	Philosophers_Test(dp,(i+4) % 5); 
	Philosophers_Test(dp,(i+1) % 5);

	pthread_mutex_unlock(&dp->self[i]);
	pthread_mutex_unlock(&dp->self[(i+1)%5]);

}

void Philosophers_Test(Philosophers *dp, int i)
{
	// TO DO: implement this function
	
	if((dp->state[(i+4) % 5] != EATING) &&
		(dp->state[i] == HUNGRY) && 
		(dp->state[(i+1) % 5] != EATING) ) {
			dp->state[i] = EATING; 
			pthread_cond_signal(dp->self);
		}


}



pthread_mutex_t disp_mutex = PTHREAD_MUTEX_INITIALIZER;

void Philosophers_Display(Philosophers *dp, int screen_width, int screen_height)
// DO NOT modify this function
{
	pthread_mutex_lock(&disp_mutex);

	int cx = (screen_width - 10) / 2;
	int cy = screen_height / 2;
	int radius = MIN(cx / 2, cy) * 4 / 5;


	for(int i = 0; i < dp->no_phil; i++){
		int orientation = i * 360.F / dp->no_phil;
		int dx = (int)(sin(orientation * PI/ 180) * radius * 2);
		int dy = -(int)(cos(orientation * PI/ 180) * radius);

		gotoxy(cx + dx, cy + dy);
		printf("%d:%s", i, StateName[dp->state[i]]);
	}

	gotoxy(screen_width, 1);
	fflush(stdout);

	pthread_mutex_unlock(&disp_mutex);
}

int Philosophers_Check(Philosophers *dp)
// DO NOT modify this function
{
	pthread_mutex_lock(&dp->mutex);

	int i = 0;

	int ret = TRUE;

	int no_phil = dp->no_phil;

	for(i = 0; i < no_phil; i++){
		int prev = (i + no_phil - 1) % no_phil;
		int next = (i + 1) % no_phil;

		// check if neighboring philosophers are eating together
		if(dp->state[i] == EATING && dp->state[next] == EATING){
			printf("Error! state[%d] = %s, state[%d] = %s\n", i, StateName[dp->state[i]], next, StateName[dp->state[next]]);
			ret = FALSE;
			break;
		}

		// check if a philosopher is waiting unnecessarily
		if(dp->state[i] == HUNGRY && dp->state[prev] != EATING && dp->state[next] != EATING){
			printf("Error! state[%d] = %s, state[%d] = %s, state[%d] = %s\n", prev, StateName[dp->state[prev]], i, StateName[dp->state[i]], next, StateName[dp->state[next]]);
			ret = FALSE;
			break;
		}
	}

	// check for deadlock
	for(i = 0; i < no_phil && dp->state[i] == HUNGRY; i++){}

	if(i == no_phil){
		printf("Error! Deadlock!\n");
		ret = FALSE;
	}

	pthread_mutex_unlock(&dp->mutex);

	if(ret == FALSE)
		MyPause();

	return ret;
}

