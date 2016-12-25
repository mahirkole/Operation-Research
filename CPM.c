#include<stdio.h>
#include<stdlib.h>
#include<conio.h>

/**
* @author Mahir Köle
*		  mahirkole@gmail.com
*/

/**
* Structure that represents activities
* @param aNumber {int} - Activity name/no
* @param aDuration {int} - Activity duration
* @param eStart {int} - Earliest Start time
* @param eFinish {int} - Earliest Finish time
* @param lStart {int} - Latest Start time
* @param lFinish {int} - Latest Finish time
* @param beforeCount {int} - Number of predecessors
* @param afterCount {int} - Number of successor
* @param before {**struct cpmnode_} - Adresses of predecessor
*				activities
* @param after {**struct cpmnode_} - Adresses of successor
*				activities
*/
typedef struct cpmnode_{
	int aNumber;
	int aDuration;
	int eStart;
	int eFinish;
	int lStart;
	int lFinish;
	int beforeCount;
	int afterCount;
	struct cpmnode_ **before;
	struct cpmnode_ **after;
} cpmnode;

/**
* @param diagram {*cpmnode} - Diagram that holds activities
* @param diagramCounter {int} - Activitie number
*/
cpmnode *diagram;
int diagramCounter = 0;

/**
* Allocate memory for diagramCounter
* to hold activities
* @param limit {int} - Activity number
*/
void allocateDiagram(int limit){
	diagram = (cpmnode *)malloc(limit * sizeof(cpmnode));	
}

/**
* Add activity to diagram
* @param activity {cpmnode} - Activity that will be added
*					to diagram
*/
void addActivty(cpmnode activity){
	diagram[diagramCounter++] = activity;
}

/**
* Dev Metho - Not Used
* @param aNumber {int} - Bulunmasý istenen aktivite numarasý
*/
cpmnode* findActivity(int aNumber){
	int i;
	aNumber--;
	for(i=0;i<diagramCounter;i++){
		if(diagram[i].aNumber == aNumber){
			return &diagram[i];	
		}
	}
	return NULL;
}

/**
* Run Critical Path Method
*/
void cpm(){
	int i,j;
	int total = 0; //Minimum time for project
	
	printf("CPM:");
	
	for(i=0;i<diagramCounter;i++)
		if(diagram[i].lFinish - diagram[i].eFinish == 0)//if Slack time is 0
		{
			printf("%d->", diagram[i].aNumber+1);//This is on Critical Path
			total += diagram[i].aDuration;//Add this duration to total duration
			
			//Look for successor activities and find activities that is on
			//Critical Path and has 0 slack time, and proceed
			for(j=0;j<diagram[i].afterCount;j++){
				if(diagram[i].after[j]->lFinish-diagram[i].after[j]->eFinish == 0)
				{	
					printf("%d->", diagram[i].after[j]->aNumber+1);
					total += diagram[i].after[j]->aDuration;
					i = diagram[i].after[j]->aNumber;
				}
			}
		}
	
	printf("Total: %d", total);
}

/**
* Calculates the activity times for
* Earliest Finish/Start and Latest Finish/Start
*/
void calculateTimes(){
	int i,j;
	int maxEF = 0;
	int minLS = 32000;
	
	//For each activity
	for(i=0;i<diagramCounter;i++){
		
		//Look to predeccessors and find
		//one that has greatest Earliest Finish
		for(j=0;j<diagram[i].beforeCount;j++){
			if(diagram[i].before[j]->eFinish > maxEF)
				maxEF = diagram[i].before[j]->eFinish;
		}
		//Set the Earliest Start and Earliest Finish times
		//for activity
		diagram[i].eStart = maxEF;
		diagram[i].eFinish = maxEF + diagram[i].aDuration;
		
		//Reset the value for the next process and proceed.
		maxEF = 0;
	}
	
	//Latest Finish time of last activity 
	//is it's own Earliest Finish time
	diagram[diagramCounter-1].lFinish = diagram[diagramCounter-1].eFinish;
	diagram[diagramCounter-1].lStart = diagram[diagramCounter-1].lFinish - diagram[diagramCounter-1].aDuration;
	
	//By descending order, for each activity
	for(i=diagramCounter-2;i>-1;i--){
		
		for(j=0;j<diagram[i].afterCount;j++){
			//find minimum Latest Start time of predecessors
			if(diagram[i].after[j]->lStart < minLS)
				minLS = diagram[i].after[j]->lStart;
			
		}
		//If there isn't any
		if(minLS == 32000)
			diagram[i].lFinish = diagram[diagramCounter-1].lFinish;
		else
			diagram[i].lFinish = minLS;
		
		diagram[i].lStart = diagram[i].lFinish - diagram[i].aDuration;
		
		printf("\n LF:%d, LS:%d", diagram[i].lFinish, diagram[i].lStart);
		
		minLS = 32000;
	}
}

/**
* Print Diagram to screen
*/
void printDiagram(){
	int i,j;
	
	for(i=0;i<diagramCounter;i++){
		
		printf("\n\t+---+---+---+");
		printf("\n\t|%2d |%2d |%2d |", i+1, diagram[i].eStart, diagram[i].eFinish);
		printf("\n\t+---+---+---+");
		printf("\n\t|%2d |%2d |%2d |", diagram[i].aDuration, diagram[i].lStart, diagram[i].lFinish);
		printf("\n\t+---+---+---+");
		
		printf("\n\t Predecessors: ");
		
		for(j=0;j<diagram[i].beforeCount;j++){
			printf("%d ", diagram[i].before[j]->aNumber+1);
		}
		printf("\n");
		
		printf("\n\t Successors: ");
		
		for(j=0;j<diagram[i].afterCount;j++){
			printf("%d ", diagram[i].after[j]->aNumber+1);
		}
		printf("\n");
	}
}

/**
* Define successors
*/
void setAfters(){
	
	int i,j;
	
	int afterCount, beforeCount;
	
	cpmnode **after;
		
	//For each activity (Main Activity)
	for(i=0;i<diagramCounter;i++){
		
		//for each predeccessor of activity (Sub Activity)
		for(j=0;j<diagram[i].beforeCount;j++)
		{
			//add a new successor
			diagram[i].before[j]->afterCount++;//Increase the after count for sub activity
			after = diagram[i].before[j]->after;//Take successors
			after = (cpmnode **)realloc(after, diagram[i].before[j]->afterCount*sizeof(cpmnode *));//Allocate one more space
			//after[diagram[i].before[j]->afterCount-1] = findActivity(i+1);
			after[diagram[i].before[j]->afterCount-1] = &diagram[i];//Put adress to new space
			diagram[i].before[j]->after = after;//Update successors
		}
		
	}
}

main(int argc, char *argv[])
{
	printf("\tCritical Path Method");
	printf("\n\t Step 1:");
	printf("\n\t\tEnter total activity number.");
	printf("\n\t Step 2:");
	printf("\n\t\tEnter duration for current activity.");
	printf("\n\t Step 3:");
	printf("\n\t\tEnter predecessor number for current activity");
	printf("\n\t\tif there is not any enter 0.");
	printf("\n\t Step 4:");
	printf("\n\t\tEnter activity numbers by order for predecessors");
	printf("\n\t\tfor current activity.");
	printf("\n\n\tCPM: Program calculates Critical Path");
	printf("\n\t\tand print it to screen.");
	printf("\n\tTotal: Time for project to end");
	printf("\n\t***Activity numbers starts from 1.");
	
	int activityNum;//Activity Number
	
	int aDuration;
	int beforeCount;
	int i,j,activity;
	cpmnode **before;
	
	printf("\n\nActivity Number: ");
	scanf("%d", &activityNum);
	
	allocateDiagram(activityNum);
	
	for(i=0;i<activityNum;i++){
		printf("\n For %d. activity: ", i+1);
		
		printf("\n\t[1] - Activity Duration: ");
		scanf("%d", &aDuration);
		
		printf("\n\t[2] - Predecessors number: ");
		scanf("%d", &beforeCount);
		
		if(beforeCount>0)
			printf("\n\t\tActivity numbers for each predecessor");
		
		before = (cpmnode **)malloc(beforeCount * sizeof(cpmnode *));
		
		//Set predeccessors
		for(j=0;j<beforeCount;j++)
		{
			printf("\n\t\t[2-Predecessor %d]: ",j+1);
			scanf("%d", &activity);
			before[j] = findActivity(activity);
			if(before[j] == NULL)
			{
				printf("ERROR: You entered an unsaved activity number!");
				exit(1);
			}
		}
		
		//Create new activity and add it to the diagram
		cpmnode newActivity;
			
		newActivity.aNumber = i;
		newActivity.aDuration = aDuration;
		newActivity.before = before;
		newActivity.beforeCount = beforeCount;
		newActivity.after = (cpmnode **)malloc(1);
		newActivity.afterCount = 0;
		diagram[diagramCounter++] = newActivity;
		
	}
	
	setAfters();
	calculateTimes();
	printDiagram();
	cpm();
	
	getch();
}

