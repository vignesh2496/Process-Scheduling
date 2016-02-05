#include <bits/stdc++.h>
#define OVERHEAD 1
using namespace std;

struct process
{
    int arrival_time;
    int burst_time;
    int turnaround_time;
    int waiting_time;
    int flag;
    int prior;
    int left;
};

//n      : Number of processes
//q      : Time quantum for Round Robin
//sum[i] : Sum of CPU-burst times from process 1 to process i
//p[]    : Array of processes
//num    : Number of processes left to execute
//t      : Time instant
//o      : Context switch overhead

int n,q,o;

//hash table to mark time instants when a process arrived
int check_arrival[120];

process p[10000];
int sum[10000],totTATfcfs,totWTfcfs,totTATsjnNP,totWTsjnNP,totTATsjnP,totWTsjnP,totTATRR,totWTRR,totTATpriorNP,totWTpriorNP,totTATpriorP,totWTpriorP;
float avgTATfcfs,avgWTfcfs,avgTATsjnNP,avgWTsjnNP,avgTATsjnP,avgWTsjnP,avgTATRR,avgWTRR,avgTATpriorNP,avgWTpriorNP,avgTATpriorP,avgWTpriorP;

//First Come First Serve
void fcfs()
{
    sum[0]=0;
    for(int i=1;i<=n;i++)
        sum[i]=sum[i-1]+p[i-1].burst_time;
    for(int i=0;i<n;i++)
    {
        p[i].turnaround_time=sum[i+1]+o*(i+1)-p[i].arrival_time; //o*(i+1) to include overhead
        totTATfcfs+=p[i].turnaround_time;
    }
    for(int i=0;i<n;i++)
    {
        p[i].waiting_time=p[i].turnaround_time-p[i].burst_time;
        totWTfcfs+=p[i].waiting_time;
    }
    avgTATfcfs=(float)totTATfcfs/n;
    avgWTfcfs=(float)totWTfcfs/n;
}

//Shortest Job Next Non-Preemptive
void sjnNonPreemptive()
{
    int num=n,t=0;
    int pid=0;
    int min;
    while(num>0)
    {
        min=10000;
        for(int i=0;i<n;i++)
        {
            if(p[i].arrival_time<=t)
            {
                if(p[i].flag==0)
                {
                    if(p[i].burst_time<min)
                    {
                        min=p[i].burst_time;
                        pid=i;
                    }
                }
            }
            else
                break;
        }

        //If no process is available to process at that time
        if(min==10000)
        {
            while(1)
            {
                t++;
                if(check_arrival[t]==1)
                    break;
            }
        }

        else
        {
            t+=o; //To include overhead

            //Execute and finish
            t+=p[pid].burst_time;
            p[pid].left=0;
            p[pid].flag=1;
            p[pid].turnaround_time=t-p[pid].arrival_time;
            p[pid].waiting_time=p[pid].turnaround_time-p[pid].burst_time;
            num--;
            totTATsjnNP+=p[pid].turnaround_time;
            totWTsjnNP+=p[pid].waiting_time;
        }
    }
    avgTATsjnNP=(float)totTATsjnNP/n;
    avgWTsjnNP=(float)totWTsjnNP/n;
}

//Shortest Job Next Preemptive
void sjnPreemptive()
{
    int num=n,t=o;
    int pid=0;
    int pidprev=0; ////pidprev keeps track of the previous process which got the CPU time
    int min;
    while(num>0)
    {
        min=10000;
        for(int i=0;i<n;i++)
        {
            if(p[i].arrival_time<=t)
            {
                if(p[i].flag==0)
                {
                    if(p[i].left<min)
                    {
                        min=p[i].left;
                        pid=i;
                    }
                }
            }
            else
                break;
        }

        //If no process is available to process at that time
        if(min==10000)
        {
            while(1)
            {
                t++;
                if(check_arrival[t]==1)
                    break;
            }
        }

        else
        {
            if(pidprev!=pid) //To check if a context switch has occurred
            {
                t+=o; //To include overhead
                pidprev=pid;
            }

            //Execute till a new process arrives or till the current process finishes, which ever happens first
            for(int i=1;i<=p[pid].left;i++)
            {
                t++;
                p[pid].left--;
                if(check_arrival[t]==1)
                    break;
            }

            //Check for completion
            if(p[pid].left==0)
            {
                p[pid].flag=1;
                p[pid].turnaround_time=t-p[pid].arrival_time;
                p[pid].waiting_time=p[pid].turnaround_time-p[pid].burst_time;
                num--;
                totTATsjnP+=p[pid].turnaround_time;
                totWTsjnP+=p[pid].waiting_time;
            }
        }
    }
    avgTATsjnP=(float)totTATsjnP/n;
    avgWTsjnP=(float)totWTsjnP/n;
}

//Round Robin
void roundRobin()
{
    int num=n,t=o,pidprev=0;
    while(num>0)
    {
        int check=0; //check is set to 1 if there exists a process to process at that time
        for(int i=0;i<n;i++)
        {
            if(p[i].arrival_time<=t)
            {
                if(p[i].flag==0)
                {
                    check=1;
                    if(i!=pidprev) //To check if a context switch has occurred
                        t+=o; //To include overhead
                    t+=min(q,p[i].left);
                    p[i].left-=min(q,p[i].left);
                    if(p[i].left==0)
                    {
                        p[i].flag=1;
                        p[i].turnaround_time=t-p[i].arrival_time;
                        p[i].waiting_time=p[i].turnaround_time-p[i].burst_time;
                        num--;
                        totTATRR+=p[i].turnaround_time;
                        totWTRR+=p[i].waiting_time;
                    }
                    pidprev=i; //pidprev keeps track of the previous process which got the CPU time
                }
            }
            else
                break;
        }

        //If no process is available to process at that time
        if(check==0)
        {
            while(1)
            {
                t++;
                if(check_arrival[t]==1)
                    break;
            }
        }
    }
    avgTATRR=(float)totTATRR/n;
    avgWTRR=(float)totWTRR/n;
}

//Priority Scheduling Non-Preemptive
void priorSchedNonPreemptive()
{
    int num=n,t=0,pid=0;
    int min;
    while(num>0)
    {
        min=10000;
        for(int i=0;i<n;i++)
        {
            if(p[i].arrival_time<=t)
            {
                if(p[i].flag==0)
                {
                    if(p[i].prior<min)
                    {
                        pid=i;
                        min=p[i].prior;
                    }
                }
            }
            else
                break;
        }

        //If no process is available to process at that time
        if(min==10000)
        {
            while(1)
            {
                t++;
                if(check_arrival[t]==1)
                    break;
            }
        }

        else
        {
            t+=o; //To include overhead

            //Execute and finish
            t+=p[pid].burst_time;
            p[pid].left=0;
            p[pid].flag=1;
            p[pid].turnaround_time=t-p[pid].arrival_time;
            p[pid].waiting_time=p[pid].turnaround_time-p[pid].burst_time;
            num--;
            totTATpriorNP+=p[pid].turnaround_time;
            totWTpriorNP+=p[pid].waiting_time;
        }
    }

    avgTATpriorNP=(float)totTATpriorNP/n;
    avgWTpriorNP=(float)totWTpriorNP/n;
}

//Priority Scheduling Preemptive
void priorSchedPreemptive()
{
    int num=n,t=o;
    int pid=0,pidprev=0;
    int min;
    while(num>0)
    {
        min=10000;
        for(int i=0;i<n;i++)
        {
            if(p[i].arrival_time<=t&&p[i].flag==0)
            {
                if(p[i].prior<min)
                {
                    min=p[i].prior;
                    pid=i;
                }
            }
        }

        //If no process is available to process at that time
        if(min==10000)
        {
            while(1)
            {
                t++;
                if(check_arrival[t]==1)
                    break;
            }
        }

        else
        {
            if(pidprev!=pid) //To check if a context switch has occurred
            {
                t+=o; //To include overhead
                pidprev=pid;
            }

            //Execute till a new process arrives or till the current process finishes, which ever happens first
            for(int i=1;i<=p[pid].left;i++)
            {
                t++;
                p[pid].left--;
                if(check_arrival[t]==1)
                    break;
            }

            //Check for completion
            if(p[pid].left==0)
            {
                p[pid].flag=1;
                p[pid].turnaround_time=t-p[pid].arrival_time;
                p[pid].waiting_time=p[pid].turnaround_time-p[pid].burst_time;
                num--;
                totTATpriorP+=p[pid].turnaround_time;
                totWTpriorP+=p[pid].waiting_time;
            }
        }
    }
    avgTATpriorP=(float)totTATpriorP/n;
    avgWTpriorP=(float)totWTpriorP/n;
}

void printResults()
{
    cout<<"\n\nPROCESS NAME"<<"   ARRIVAL TIME   "<<"   CPU-BURST TIME   "<<"   PRIORITY   "<<endl;
    cout<<"------------"<<"   ------------   "<<"   --------------   "<<"   --------   "<<endl;
    for(int i=0;i<n;i++)
    {
        if(i<9)
            printf("Process %d%13d%19d%17d\n",i+1,p[i].arrival_time,p[i].burst_time,p[i].prior);
        else if(i<99)
            printf("Process %d%12d%19d%17d\n",i+1,p[i].arrival_time,p[i].burst_time,p[i].prior);
        else if(i<999)
            printf("Process %d%11d%19d%17d\n",i+1,p[i].arrival_time,p[i].burst_time,p[i].prior);
        else
            printf("Process %d%10d%19d%17d\n",i+1,p[i].arrival_time,p[i].burst_time,p[i].prior);
    }

    cout<<endl<<endl<<endl<<endl;
    printf("SCHEDULING ALGORITHM                     AVG WAITING TIME    AVG TURNAROUND TIME\n");
    printf("--------------------                     ----------------    -------------------");
    cout<<endl;
    printf("\nFirst Come First Serve %31f%22f",avgWTfcfs,avgTATfcfs);
    printf("\nShortest Job Next Non-Preemptive %21f%22f",avgWTsjnNP,avgTATsjnNP);
    printf("\nShortest Job Next Preemptive %25f%22f",avgWTsjnP,avgTATsjnP);
    printf("\nRound Robin %42f%22f ",avgWTRR,avgTATRR);
    printf("\nPriority Scheduling Non-Preemptive %19f%22f ",avgWTpriorNP,avgTATpriorNP);
    printf("\nPriority Scheduling Preemptive %23f%22f",avgWTpriorP,avgTATpriorP);
    cout<<endl<<endl;

}

int main()
{
    READ1:cout<<"Enter the number of processes (10-1000):";
    cin>>n;
    if(n<10)
    {
        cout<<"\nWhy would you want so many different varieties of scheduling algorithms for a handful of processes, it hardly makes a difference. Please enter more(at least 10) so that you do justice to my hardwork.\n";
        goto READ1;
    }
    if(n>1000)
    {
	cout<<"\nAt most 1000 processes are allowed. Enter again.\n";
	goto READ1;
    }

    READ2:cout<<"Enter the time quantum for Round Robin:";
    cin>>q;
    if(q<10)
    {
        cout<<"\nDo you realize how much time you will be wasting on a context switch when compared to your execution time of processes? Please enter a sensible number(at least 5).\n";
        goto READ2;
    }
    if(q>110)
    {
        cout<<"\nWhy do I even need to give you an option of Round Robin, you might have as well reduced the work for me! Anyway I will go ahead and show you what you have done by entering such a value.\n";
        cout<<"Press 'y' to proceed and 'n' to go back and enter the time quantum again:";
        char c;
        cin>>c;
        if(c=='n')
            goto READ2;
        cout<<endl;
    }

    cout<<"Do you want consider the overhead for a context switch?(y/n):";
    char c;
    cin>>c;
    if(c=='y')
        o=OVERHEAD;
    else
        o=0;

	
    srand (time(NULL));
    int arr[n];
    for(int i=0;i<n;i++)
    {
        arr[i]=random()%100;
        check_arrival[arr[i]]=1;
    }


    arr[0]=0;
    sort(arr,arr+n);

    for(int i=0;i<n;i++)
    {
        p[i].arrival_time=arr[i];
        p[i].burst_time=random()%100+10;
        p[i].flag=0;
        p[i].left=p[i].burst_time;
        p[i].prior=random()%n;
    }

    //First Come First Serve
    fcfs();

    //Shortest Job Next Non-Preemptive
    sjnNonPreemptive();
    for(int i=0;i<n;i++)
    {
        p[i].flag=0;
        p[i].left=p[i].burst_time;
    }

    //Shortest Job Next Preemptive
    sjnPreemptive();
    for(int i=0;i<n;i++)
    {
        p[i].flag=0;
        p[i].left=p[i].burst_time;
    }

    //Round Robin
    roundRobin();
    for(int i=0;i<n;i++)
    {
        p[i].flag=0;
        p[i].left=p[i].burst_time;
    }

    //Priority Scheduling Non-Preemptive
    priorSchedNonPreemptive();
    for(int i=0;i<n;i++)
    {
        p[i].flag=0;
        p[i].left=p[i].burst_time;
    }

    //Priority Scheduling Preemptive
    priorSchedPreemptive();

    //Print Statistics
    printResults();
    return 0;
}

