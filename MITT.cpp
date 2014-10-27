#include "MITT_vs_ERS.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <iostream.h >
#include <iomanip.h>
//opengl
#include <windows.h>
#include <GL/glut.h>

#define random(x) (rand()%x)

Tree tree;
Queue q;
Queue V1,V2,V3;
Queue testQ;
int k1,k2;
int PK=100;
bool isChanged=true;
double E_max=0;
double Weight_max=0;
double phi=0;
double c=(double)ETX/(double)(ETX+ERX);



long current_run_mitt_dir_round=0;
long current_run_mitt_arq_round=0;
long current_run_ers_round=0;

double current_run_mitt_dir_ro=0;
double current_run_mitt_arq_ro=0;
double current_run_ers_ro=0;

int REP=100;
int choice=1;
double cal_distance(int x0,int y0,int x1,int y1){
	return sqrt(pow(x1-x0,2)+pow(y1-y0,2));
}
void initTree(Tree *t);
void deployNodes(Tree *t);
void deployEnergy(Tree *t);
void clearTrace(Tree *t);
int isNodeDead(Tree *t);
//MITT
void setWeight(Tree *t);
void setNeighbors(Tree *t);
void constructTreeInitTree(Tree *t);
void appendNodeToFather(CSNode * father, CSNode * child);
void removeNodeFromFather(CSNode * father, CSNode * child);
void descendantAdded(CSNode *child);
void setchildren(Tree * t);
void classifyNodes(Tree *t);
void setCapacity(CSNode *node);
bool transfer(Tree *t,CSNode *node ,int k1);
//end of MITT

double Etx(double k){
	if(k==0)return 0;
	return k*(double)ETX;
}
double Erx(double k){
	if(k==0)return 0;
	return k*(double)ERX; 
}

/***********opengl*************/

void showTree();

void drawTree()
{ 
	int i;
	glClearColor(1.0,1.0,1.0,0.0);   //指定清除颜色缓存区时使用黑色
	glClear(GL_COLOR_BUFFER_BIT); 
	glColor3f ( 1.0f, 0.0f, 0.0f );
	glPointSize(6.0);
	glBegin(GL_POINTS);
	glVertex2i(tree.treeNode[tree.root].x, tree.treeNode[tree.root].y);
	glEnd();
	glColor3f ( 0.0f, 0.0f, 0.0f );
	glPointSize(3.0);	
//draw border
	glBegin(GL_LINE_LOOP);
	glVertex2i(0, 0);
	glVertex2i(0, LOC_SIZE);
	glVertex2f(LOC_SIZE, LOC_SIZE);
	glVertex2f(LOC_SIZE, 0);
	glEnd();
//end of draw border

//draw tree



	for(i=0;i<tree.size;i++){
		if(tree.treeNode[i].father){
			glBegin(GL_POINTS);
			glVertex2i(tree.treeNode[i].x, tree.treeNode[i].y);
			glVertex2i((tree.treeNode[i].father)->x, (tree.treeNode[i].father)->y);
			glEnd();
			
			glBegin(GL_LINES);
			glVertex2i(tree.treeNode[i].x, tree.treeNode[i].y);
			glVertex2i((tree.treeNode[i].father)->x, (tree.treeNode[i].father)->y);
			glEnd();
			glFlush();
			
		}
	}
	glFlush();

}
void init()
{ 
	glClearColor(1.0,1.0,1.0,0.0);   //指定清除颜色缓存区时使用黑色
	glClear(GL_COLOR_BUFFER_BIT); 
	glColor3f ( 0.0f, 0.0f, 0.0f );
    glMatrixMode(GL_PROJECTION);    //指定后续受变换函数影响的矩阵，设置为GL_PROJECTION（投影）
    glLoadIdentity();   //将当前矩阵初始化为一个单位矩阵
	glTranslatef(-1, -1, 0);
    gluOrtho2D(0, 0, 100.0, 100.0);   //指定二维平面上的裁剪窗口（Clipping window）
}
void changeSize(int w, int h) {

         // 防止除数即高度为0
         // (你可以设置窗口宽度为0).
         if(h == 0)
                 h = 1;
         float ratio = 1.0* w / h;
         // 单位化投影矩阵。
         glMatrixMode(GL_PROJECTION);
         glLoadIdentity();		 
         // 设置视口大小为增个窗口大小
         glViewport(0, 0, w, h);
 
         // 设置正确的投影矩阵
         gluPerspective(90,ratio,1,1000);
        //下面是设置模型视图矩阵
         glMatrixMode(GL_MODELVIEW);
         glLoadIdentity();
         gluLookAt(100.0,100.0,110.0, 100.0,100.0,-1.0,0.0f,1.0f,0.0f);
}

void showTree(){
		//glutInit(&argc, argv);  //初始化glut
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);    //设置显示属性为RGB颜色， 单缓冲
    glutInitWindowSize(1000,1000);   //设置窗口大小
    glutInitWindowPosition(0,0);    //设置窗口位置
    glutCreateWindow("line_loop");     //生成窗口
    glutDisplayFunc(drawTree);   //显示回调	
    init();
	glutReshapeFunc(changeSize);
    glutMainLoop(); //事件循环	
	
}
/***********opengl*************/


void E_RS_send_sim(CSNode *p){

	int arrivalDataCount=0;
	int randP;
	int reTryCount=MAX_RETRY+1;
	bool sendDone=false;
	//fragment
	int sendNum=0;		//s
	int sendCount=0;	
	int lastOneAlign=0;	//send last data with ARQ
	int systemCode_rx=0;	//system code receive count

	//after E-RS
	int M=0;
	int tmp=0;
	int rx_count=0;
	double energy=0;
	double bitSize=(MAC_HEADER+1+1+1+MAX_APPEND_COUNT*SENSOR_DATA_SIZE)*8;
	double lastOneBitSize=0;
	//p is root
	if(p->father==NULL) return ;
	//append
	//cout<<"dataCount of node "<<p->id<<" is "<<p->dataCount<<endl;
	lastOneAlign=(p->dataCount)%MAX_APPEND_COUNT;
	sendNum=(int)((p->dataCount-lastOneAlign)/MAX_APPEND_COUNT);
	if(sendNum==0){
	//append data less than one segment 
	//no send
	
	}else if(sendNum==1){
	//ARQ
	
		reTryCount=MAX_RETRY+1;
		while(reTryCount){
			reTryCount--;
			randP=random(PERCENT);
			energy+=Etx(bitSize);
			energy+=Erx(bitSize);
			if(randP<PK){
			//delivered
				energy+=Etx(ACK_SIZE*8);
				energy+=Erx(ACK_SIZE*8);
				reTryCount=0;
				arrivalDataCount+=sendNum*MAX_APPEND_COUNT;
			}
		}				
	}else{
	//E-RS	
		//cout<<"use ERS"<<endl;
		//encode
		M=LAMBDA*sendNum;
		//sendiing
		while(M&&!sendDone){
			randP=random(PERCENT);
			sendCount++;
			energy+=Etx(bitSize);
			energy+=Erx(bitSize);
			if(randP<PK){
				//delivered
				rx_count++;
				if(sendCount<=sendNum){
					systemCode_rx++;
				}
				if(rx_count==sendNum){
					sendDone=true;
				}
			}else{
				//lost
			}
			M--;
		}
		if(sendDone==true){
		// ALL decode SUCCESS
			//ACK energy
			energy+=Etx(ACK_SIZE*8);
			energy+=Erx(ACK_SIZE*8);
			arrivalDataCount+=sendNum*MAX_APPEND_COUNT;
		}else{
		//decode FAIl but system code SUCCESS
		//no ACK
			arrivalDataCount+=systemCode_rx*MAX_APPEND_COUNT;
		}
		
	}//end of else(sendNum)
	//send last align
	if(lastOneAlign){
		
		lastOneBitSize=(MAC_HEADER+lastOneAlign*SENSOR_DATA_SIZE)*8;
		reTryCount=MAX_RETRY+1;
		while(reTryCount){
			reTryCount--;
			randP=random(PERCENT);
			energy+=Etx(lastOneBitSize);
			energy+=Erx(lastOneBitSize);
			if(randP<PK){
			//delivered
				energy+=Etx(ACK_SIZE*8);
				energy+=Erx(ACK_SIZE*8);
				arrivalDataCount+=lastOneAlign;
				reTryCount=0;
			}else{
			//lost
			}
		}
	}
	if(p->father==NULL){
	//p is root
	}else{		
		p->father->dataCount+=arrivalDataCount;
		p->dataCount=DATA_COUNT_EACH_NODE;
		arrivalDataCount=0;
		p->father->energy+=energy;
	//NOTE energy is nj  and p->residual_energy is j
	
		p->residual_energy-=(energy/(double)1000000000);
	}
}
void DIR_send(CSNode *p){
	
	int arrivalDataCount=0;
	int randP;
	int reTryCount=MAX_RETRY+1;
	double energy=0;
	double bitSize=(MAC_HEADER+SENSOR_DATA_SIZE)*8;
	reTryCount=1;
	if(p->father==NULL)return ;
	for(int i=0;i<(p->dataCount);i++){
	//DIR send each data
		while(reTryCount){
			reTryCount--;
			randP=random(PERCENT);
			energy+=Etx(bitSize);
			energy+=Erx(bitSize);
			if(randP<=PK){
			//delivered
				energy+=Etx(ACK_SIZE*8);
				energy+=Erx(ACK_SIZE*8);
				arrivalDataCount++;
				reTryCount=0;
			}
		}
		reTryCount=1;
	}
	if(p->father==NULL){
	//this is root
	}else{
		/*
		if(p->dataCount!=p->descendant_count+1){
			
			cout<<"int ARQ_send debug node id "<<p->id<<endl;	
			cout<<"p->dataCount="<<p->dataCount<<"p->descendant_count+1="<<p->descendant_count+1<<endl;
		}
		*/
		p->father->dataCount+=arrivalDataCount;
		p->dataCount=DATA_COUNT_EACH_NODE;
		arrivalDataCount=0;
		p->father->energy+=energy;
	//NOTE energy is nj  and p->residual_energy is j
	
		p->residual_energy-=(energy/(double)1000000000);
		//cout<<"energy="<<energy<<endl;
		//cout<<setw(8)<<setprecision(10)<<setiosflags(ios::fixed)<<(energy/(double)1000000000)<<endl;
		//cout<<"node "<<p->id<<" residual_energy is "<<p->residual_energy<<endl;
		
		energy=0;
	}
}

void ARQ_send(CSNode *p){
	//ARQ,NOT append
	int arrivalDataCount=0;
	int randP;
	int reTryCount=MAX_RETRY+1;
	double energy=0;
	double bitSize=(MAC_HEADER+SENSOR_DATA_SIZE)*8;
	//check if is root
	//cout<<"node "<<p->id<<" dataCount is "<<p->dataCount<<endl;
	if(p->father==NULL)return ;
	for(int i=0;i<(p->dataCount);i++){
	//ARQ send each data
		while(reTryCount){
			reTryCount--;
			randP=random(PERCENT);
			energy+=Etx(bitSize);
			energy+=Erx(bitSize);
			if(randP<=PK){
			//delivered
				energy+=Etx(ACK_SIZE*8);
				energy+=Erx(ACK_SIZE*8);
				arrivalDataCount++;
				reTryCount=0;
			}
		}
		reTryCount=MAX_RETRY+1;
	}
	if(p->father==NULL){
	//this is root
	}else{
		/*
		if(p->dataCount!=p->descendant_count+1){
			
			cout<<"int ARQ_send debug node id "<<p->id<<endl;	
			cout<<"p->dataCount="<<p->dataCount<<"p->descendant_count+1="<<p->descendant_count+1<<endl;
		}
		*/
		p->father->dataCount+=arrivalDataCount;
		p->dataCount=DATA_COUNT_EACH_NODE;
		arrivalDataCount=0;
		p->father->energy+=energy;
	//NOTE energy is nj  and p->residual_energy is j
	
		p->residual_energy-=(energy/(double)1000000000);
		//cout<<"energy="<<energy<<endl;
		//cout<<setw(8)<<setprecision(10)<<setiosflags(ios::fixed)<<(energy/(double)1000000000)<<endl;
		//cout<<"node "<<p->id<<" residual_energy is "<<p->residual_energy<<endl;
		
		energy=0;
	}
}
void BFS(CSNode *n){
	CSNode *p;
	
	if(n==NULL)return ;
	if(n->firstchild!=NULL){
		BFS(n->firstchild);
		p=n->firstchild;
		while(p->nextsibling){
			BFS(p->nextsibling);
			p=p->nextsibling;
		}
	}
	if(choice==SEND_CHOICE_ARQ){
		ARQ_send(n);
	}else if(choice==SEND_CHOICE_E_RS){
		E_RS_send_sim(n);
	}else if(choice==SEND_CHOICE_DIR){
		DIR_send(n);	
	}
	else{
		cout<<"wrong choice"<<endl;
	}
	
}
void MITT(){
	CSNode *x;
	CSNode *p;
	bool flag;
	isChanged=true;
	k1=100;
	k2=1000;
	
	while(isChanged==true&&k2>0){
		
		//cout<<"debug k2== "<<k2<<endl;
		setWeight(&tree);
		classifyNodes(&tree);
		setCapacity(&tree.treeNode[tree.root]);
		//cout<<" V1 size=="<<V1.size()<<endl;

//k2=2;

		while((k2--)&&!V1.isEmpty()&&(isChanged==true)){
			isChanged=false;
			V1.sort();
			//cout<<"k2=="<<k2<<endl;
			//V1.showQueue();
			while(x=V1.dequeue()){
				//cout<<"Trying transfer descendant of node "<<x->id<<endl;
				flag=transfer(&tree,x,k1);
				if(flag){
					//cout<<"transfer succeed"<<endl;
					//cout<<"descendan number of x= "<<x->id<<" is "<<x->descendant_count<<endl;
					isChanged=true;
					break;	
				}else{
					//cout<<"transfer failed"<<endl;
				}
			}
		}
	}	
}

void checkLoop(Tree *t){
	int i;
	CSNode *debug_p;
	CSNode *p;
	for(i=1;i<=t->size;i++){
		debug_p=t->treeNode[i].firstchild;
		if(debug_p!=NULL){
			debug_p=debug_p->nextsibling;
			if(debug_p!=NULL){
				p=debug_p;
				while(debug_p=debug_p->nextsibling){
					if(debug_p==p)break;		
				}
				if(debug_p){
					cout<<"LOOP at node child***************************"<<p->id<<endl;	
				}else{
					cout<<"not LOOP"<<endl;;	
				}
			}
		}
						
	}
}

void run(){
	CSNode *x;
	CSNode *p;
	int index=0;
	long roundCount=0;
	long datacount=0;
	initTree(&tree);
	deployNodes(&tree);
	deployEnergy(&tree);
	setchildren(&tree);
	MITT();
	//cout<<"after MITT check loop**************"<<endl;
	//checkLoop(&tree);
	
	//run ERS with this tree after MITT
	choice=SEND_CHOICE_E_RS;
	while((index=isNodeDead(&tree))==0)
	{
		tree.treeNode[tree.root].dataCount=0;
		BFS(&tree.treeNode[tree.root]);
		datacount=tree.treeNode[tree.root].dataCount;
		current_run_ers_ro+=(double)datacount/(double)(DATA_COUNT_EACH_NODE*MAX_TREE_SIZE);
		roundCount++;
	}
	current_run_ers_round=roundCount;
	//current_run_ers_datacount=(tree.treeNode[tree.root]).dataCount;
	//averavge
	current_run_ers_ro=current_run_ers_ro/(current_run_ers_round);
	index=0;
	roundCount=0;
	datacount=0;
	
	// clear trace
	clearTrace(&tree);
		
	//run DIR with this tree after MITT
	choice=SEND_CHOICE_DIR;
	while((index=isNodeDead(&tree))==0)
	{
		tree.treeNode[tree.root].dataCount=0;
		BFS(&tree.treeNode[tree.root]);
		datacount=tree.treeNode[tree.root].dataCount;
		current_run_mitt_dir_ro+=(double)datacount/(double)(DATA_COUNT_EACH_NODE*MAX_TREE_SIZE);
		roundCount++;
	}
	
	current_run_mitt_dir_round=roundCount;
	//current_run_mitt_datacount=(tree.treeNode[tree.root]).dataCount;
	//averavge
	current_run_mitt_dir_ro=current_run_mitt_dir_ro/(current_run_mitt_dir_round);
	
	roundCount=0;
	datacount=0;
	index=0;
	
	// clear trace
	clearTrace(&tree);
		
	//run ARQ with this tree after MITT
	choice=SEND_CHOICE_ARQ;
	while((index=isNodeDead(&tree))==0)
	{
		tree.treeNode[tree.root].dataCount=0;
		BFS(&tree.treeNode[tree.root]);
		datacount=tree.treeNode[tree.root].dataCount;
		current_run_mitt_arq_ro+=(double)datacount/(double)(DATA_COUNT_EACH_NODE*MAX_TREE_SIZE);
		roundCount++;
	}
	
	current_run_mitt_arq_round=roundCount;
	//current_run_mitt_datacount=(tree.treeNode[tree.root]).dataCount;
	//averavge
	current_run_mitt_arq_ro=current_run_mitt_arq_ro/(current_run_mitt_arq_round);
	
	roundCount=0;
	datacount=0;
	index=0;
	
	
	
	/*
	cout<<"sink dataCount is "<<(tree.treeNode[tree.root]).dataCount<<endl;
	cout<<"roundCount= "<<roundCount<<endl;
	cout<<"node "<<index<<" run out of energy ="<<setw(8)<<setprecision(2)<<setiosflags(ios::fixed)<<tree.treeNode[index].residual_energy<<endl;
	cout<<"consumed energy is"<<setw(8)<<setprecision(2)<<setiosflags(ios::fixed)<<tree.treeNode[tree.root].energy<<endl;
	*/
}

int main(int argc, char **argv){
	int rep=0;
	double mitt_dir_round=0;
	double mitt_dir_ro=0;
	double mitt_arq_round=0;
	double mitt_arq_ro=0;
	double ers_round=0;
	double ers_ro=0;
	srand((unsigned)time(NULL));
	PK=80;
	REP=100;
/*
	long current_run_mitt_dir_round=0;
long current_run_ers_round=0;
long current_run_mitt_datacount=0;
long current_run_ers_datacount=0;
long current_run_mitt_dir_ro=0;
long current_run_ers_ro=0;
*/	
	
	for(rep=0;rep<REP;rep++){
		run();
		cout<<rep<<" run"<<endl; 
		mitt_dir_round+=current_run_mitt_dir_round;	
		mitt_dir_ro+=current_run_mitt_dir_ro;
		mitt_arq_round+=current_run_mitt_arq_round;	
		mitt_arq_ro+=current_run_mitt_arq_ro;
		ers_round+=current_run_ers_round;	
		ers_ro+=current_run_ers_ro;
	}
	
	
	mitt_dir_round=mitt_dir_round/(double)REP;
	mitt_dir_ro=mitt_dir_ro/(double)REP;
	mitt_arq_round=mitt_arq_round/(double)REP;
	mitt_arq_ro=mitt_arq_ro/(double)REP;
	ers_round=ers_round/(double)REP;
	ers_ro=ers_ro/(double)REP;
	
	cout<<"REP="<<REP<<setw(8)<<setprecision(2)<<setiosflags(ios::fixed)<<endl;
	cout<<"PK="<<PK<<endl;
	cout<<"NODE="<<MAX_TREE_SIZE<<endl;
	
	cout<<"mitt_dir_round="<<setw(8)<<setprecision(2)<<setiosflags(ios::fixed)<<mitt_dir_round<<endl;
	cout<<"mitt_dir_ro="<<setw(8)<<setprecision(6)<<setiosflags(ios::fixed)<<mitt_dir_ro<<endl;
	cout<<"mitt_arq_round="<<setw(8)<<setprecision(2)<<setiosflags(ios::fixed)<<mitt_arq_round<<endl;
	cout<<"mitt_arq_ro="<<setw(8)<<setprecision(6)<<setiosflags(ios::fixed)<<mitt_arq_ro<<endl; 
	cout<<"ers_round="<<setw(8)<<setprecision(2)<<setiosflags(ios::fixed)<<ers_round<<endl;
	cout<<"ers_ro="<<setw(8)<<setprecision(6)<<setiosflags(ios::fixed)<<ers_ro<<endl;
	
		/*
	long mitt_dir_round=0;
long ers_round=0;
long mitt_datacount=0;
long ers_datacount=0;
int REP=100;
	*/
	showTree();
	return 0;
}
/*
	int id;	//node id
	int visited;
	int descendant_count;
	int dataCount;	//
	double weight;
	double capacity;
	double energy;
	double residual_energy;
	int x;
	int y;
	double dis_to_father;	//dk
	int neighborIndexs[MAX_TREE_SIZE];
	struct Node * firstchild, *nextsibling,* father;
	*/
	
void initTree(Tree *t){
	t->root=0;
	t->size=MAX_TREE_SIZE;
	
	//init sink node
	t->treeNode[t->root].id=t->root;
	t->treeNode[t->root].visited=false;
	t->treeNode[t->root].descendant_count=0;
	t->treeNode[t->root].dataCount=0;
	t->treeNode[t->root].weight=0;
	t->treeNode[t->root].capacity=0;
	t->treeNode[t->root].energy=0;
	t->treeNode[t->root].init_energy=0;
	t->treeNode[t->root].residual_energy=0;
	t->treeNode[t->root].x=ROOT_LOC;
	t->treeNode[t->root].y=ROOT_LOC;
	t->treeNode[t->root].dis_to_father=0;
	
	for(int j=0;j<MAX_TREE_SIZE;j++){
		t->treeNode[t->root].neighborIndexs[j]=0;	
	}
	t->treeNode[t->root].father=NULL;
	t->treeNode[t->root].firstchild=NULL;
	t->treeNode[t->root].nextsibling=NULL;
	
	//init nodes except sink
	
	for(int i=1;i<=MAX_TREE_SIZE;i++){
		t->treeNode[i].id=i;
		t->treeNode[i].visited=false;
		t->treeNode[i].descendant_count=0;
		t->treeNode[i].dataCount=DATA_COUNT_EACH_NODE;
		t->treeNode[i].weight=0;
		t->treeNode[i].capacity=0;
		t->treeNode[i].energy=0;
		t->treeNode[i].init_energy=0;
		t->treeNode[i].residual_energy=0;
		t->treeNode[i].x=0;
		t->treeNode[i].y=0;
		t->treeNode[i].dis_to_father=0;
		for(int j=0;j<MAX_TREE_SIZE;j++){
			t->treeNode[i].neighborIndexs[j]=0;	
		}
		t->treeNode[i].father=NULL;
		t->treeNode[i].firstchild=NULL;
		t->treeNode[i].nextsibling=NULL;
	}
}
void clearTrace(Tree *t){
	t->root=0;
	t->treeNode[t->root].dataCount=0;
	t->treeNode[t->root].visited=false;
	t->treeNode[t->root].energy=0;
	for(int i=1;i<=MAX_TREE_SIZE;i++){
		t->treeNode[i].visited=false;
		t->treeNode[i].dataCount=DATA_COUNT_EACH_NODE;
		t->treeNode[i].energy=0;
		t->treeNode[i].residual_energy=t->treeNode[i].init_energy;
	}
}
void deployNodes(Tree *t){
	int i;
	//put <size> nodes in the zone LOC_SIZE*LOC_SIZE m2
	for(i=1;i<=t->size;i++){
		t->treeNode[i].x=random(LOC_SIZE);
		t->treeNode[i].y=random(LOC_SIZE);
	}
	setNeighbors(t);
}
void deployEnergy(Tree *t){
	double e;
	int i;
	for(i=1;i<=t->size;i++){
		//random energy
		
		while((e=rand()/(double)RAND_MAX)<0.5);
//debug
		//e=1;
		t->treeNode[i].residual_energy=e;
		t->treeNode[i].init_energy=e;
		if(E_max<e){
			E_max=e;
		}
	}
}
void setWeight(Tree *t){
	double e;
	int i;
	Weight_max=0;
	for(i=1;i<=t->size;i++){
		t->treeNode[i].visited=false;
		//set weight
		t->treeNode[i].weight=((t->treeNode[i].descendant_count)+c)/t->treeNode[i].residual_energy;
		//cout<<"descendant_count of node "<<i<<" is "<<((t->treeNode[i].descendant_count)+((double)ETX/(double)(ETX+ERX)))<<endl;
		//cout<<"node "<<i<<" weight is "<<t->treeNode[i].weight<<endl;
		if(Weight_max<t->treeNode[i].weight)
		{
			Weight_max=t->treeNode[i].weight;
		}
	}
	//cout<<"max weight is "<<Weight_max<<endl;
	phi=1/E_max;
}
void setNeighbors(Tree *t){	
	int i,j;
	int neighborIndex=0;
	int debug=3;
	for(i=0;i<=t->size;i++)
	{
		neighborIndex=0;
		for(j=1;j<=t->size;j++){
			if(i!=j){
				//j is i's neighbor
				if(cal_distance(t->treeNode[i].x,t->treeNode[i].y,t->treeNode[j].x,t->treeNode[j].y)<=DIS)
				{//add j to i's neighbor link
					t->treeNode[i].neighborIndexs[neighborIndex]=j;
					neighborIndex++;
				}
			}
		}
		while(neighborIndex==0){
			cout<<"repeat deploy node "<<i<<endl;
			t->treeNode[i].x=random(LOC_SIZE);
			t->treeNode[i].y=random(LOC_SIZE);
			for(j=1;j<=t->size;j++){
				if(i!=j){
					//j is i's neighbor
					if(cal_distance(t->treeNode[i].x,t->treeNode[i].y,t->treeNode[j].x,t->treeNode[j].y)<=DIS)
					{//add j to i's neighbor link
						t->treeNode[i].neighborIndexs[neighborIndex]=j;
						neighborIndex++;
					}
				}
			}
		}
	}		
}
void setchildren(Tree * t){
		int i;
		CSNode *p;
		//BFS
		//enqueue the root
		t->treeNode[t->root].visited=true;
		q.enqueue(&t->treeNode[t->root]);
		while(!q.isEmpty()){
			p=q.dequeue();
			int *neighborIndexs=p->neighborIndexs;
			for(i=0;neighborIndexs[i]!=0;i++){
					if((t->treeNode[neighborIndexs[i]]).visited==false){
						t->treeNode[neighborIndexs[i]].visited=true;
						appendNodeToFather(p,&t->treeNode[neighborIndexs[i]]);
						if(!q.isFull()){
							//cout<<"call enqueue"<<endl;
							q.enqueue(&(t->treeNode[neighborIndexs[i]]));
						}else{
							cout<<"Queue is full!!!"<<endl;	
						}
					}
			}
		}
}
void removeNodeFromFather(CSNode * father, CSNode * child){
	CSNode *p,*q;
	
	if(father->firstchild==child){
		father->firstchild=child->nextsibling;	
	}else{
		p=father->firstchild;
		p=p->nextsibling;
		while(p!=child){
			q=p;
			p=p->nextsibling;
		}	
		q->nextsibling=p->nextsibling;
	}
	child->father=NULL;
	child->nextsibling=NULL;
	p=father;
	while(p){
		p->descendant_count-=(child->descendant_count+1);
		p=p->father;
	}
}
void appendNodeToFather(CSNode * father, CSNode * child){
	CSNode *p;
	child->father=father;
//debug
CSNode *debug_p;
	if(father->firstchild==NULL){
		//the node index has no child ,current_node is the first one
		father->firstchild=child;
		//cout<<"p->firstchild==NULL"<<endl;
	}else{
		//visit all the sibling from first one
		p=(father->firstchild);
		while(p->nextsibling){
		//move to last sibling
			p=p->nextsibling;
		}
		//append to father
		p->nextsibling=child;
		/*
		//check
		debug_p=p;
		while(debug_p=debug_p->nextsibling){
			if(debug_p==p)break;		
		}
		if(debug_p){
			cout<<"LOOP at node child***************************"<<child->id<<endl;	
		}else{
			cout<<"not LOOP";	
		}
		*/
	}
	p=child;
	while(p=p->father){
		p->descendant_count+=(child->descendant_count+1);
	}
	
}

void classifyNodes(Tree *t){
	int i=0;
	CSNode *p;
	double w=0;
//cout<<"phi="<<phi<<endl;
	V1.clearQueue();
	V2.clearQueue();
	V3.clearQueue();
	
	//cout<<"classifyNodes with Weight_max="<<Weight_max<<endl;
	for(i=1;i<=t->size;i++){
		p=&t->treeNode[i];
		w=p->weight;		
		if(w>Weight_max-phi&&w<=Weight_max){
			//put to Queue V1
			//cout<<"node "<<i<<" put into V1"<<endl;
			V1.enqueue(p);
			p->nodeType=NODETYPE_V1;
		//}else if(w>(Weight_max-phi-1)&&w<=Weight_max-phi){
		}
		else if(w>((Weight_max-phi-1)/p->residual_energy)&&w<=Weight_max-phi){
			//cout<<"node "<<i<<"put into V2"<<endl;
			V2.enqueue(p);
			p->nodeType=NODETYPE_V2;
		}else{
			V3.enqueue(p);
			p->nodeType=NODETYPE_V3;
		}	
	}
	/*
	while(!V1.isEmpty()){
		p=V1.dequeue();
		cout<<"debug Queue: id="<<p->id<<endl;
		//V1.enqueue(p);	
	}
	*/
	/*
	cout<<"V1 size="<<V1.size()<<endl;
	cout<<"V2 size="<<V2.size()<<endl;
	cout<<"V3 size="<<V3.size()<<endl;
	*/
}
void setCapacity(CSNode *node){
	double tmp=0;
	CSNode *p;
	if(node->id==tree.root){
		tree.treeNode[tree.root].capacity=tree.size;
			
	}else{
		switch(node->nodeType){
			case NODETYPE_V1: 
				{
					node->capacity=-1;
				}break;
			case NODETYPE_V2: 
				{
					if((node->father->capacity)>0){
						node->capacity=0;
					}else{
						node->capacity=node->father->capacity;
					}
				}break;
			case NODETYPE_V3: 
				{
					tmp=ceil((Weight_max-node->weight)*node->residual_energy-c);
					if((node->father->capacity)>tmp){
						node->capacity=tmp;
					}else{
						node->capacity=node->father->capacity;
					}
				}break;
			default :{};	
		}
	}
	if(node->firstchild!=NULL){
		setCapacity(node->firstchild);
		p=node->firstchild;
		while(p->nextsibling){
			setCapacity(p->nextsibling);
			p=p->nextsibling;
		}
	}

}
bool isDescendantOf(CSNode * descendant,CSNode *node){
	CSNode *p;
	if(descendant==NULL)return false;
	p=descendant;
	while(p->father){
		if(p->father==node){
			return true;	
		}
		p=p->father;
			
	}
	return false;	
}
//private of exChange

CSNode * findChildeOfRoot(CSNode *node){
	CSNode *q;
	q=node;
	
	while(q){
		if(q->father==&tree.treeNode[tree.root]){
			return q;	
		}
		q=q->father;
	}
	return NULL;
		
}
/*
debug function
*/
int checkConnect(){
	int i;
	CSNode *p;
	for(i=1;i<=tree.size;i++){
		p=&tree.treeNode[i];
		while(p!=&tree.treeNode[tree.root]&&p){
			p=p->father;
		}
		if(p==NULL){
			cout<<"BIG BUG******************************/"<<endl;
			return 0;	
		}
		if(tree.treeNode[i].father==NULL){
			return i;	
		}	
	}	
	return 0;
}


void exChange(CSNode *y,CSNode *z){
	CSNode *w,*p;
	CSNode *vc1,*vc2;
	/*
	CSNode *debug_x,*debug_p;
	//cout<<"debug: if z is a descentdant of y's father"<<isDescendantOf(z,y->father)<<endl;
	debug_x=y->father;
	debug_p=z;
	
	while(debug_p){
		if(debug_p==debug_x){
			cout<<"node y is a descendant of x*******************"<<endl;
		}
		debug_p=debug_p->father;	
	}
	*/
	w=y->father;
	//remove y from w's child link
	//cout<<"remove node "<<y->id<<" from node "<<w->id<<endl;
	removeNodeFromFather(w,y);
	
	//add y to z's child link
	//cout<<"add node "<<y->id<<" to node "<<z->id<<endl;
	appendNodeToFather(z,y); 
	//checkLoop(&tree);

	p=y;
	
	while(p=p->father){
		p->visited=false;
	}
	p=w;
	p->visited=false;
	
	while(p=p->father){
		p->visited=false;
	}
	
	//find vc1
	vc1=findChildeOfRoot(y);

	//find vc2
	vc2=findChildeOfRoot(w);
	setCapacity(vc1);
	setCapacity(vc2);
	//cout<<"checkConnect ="<<checkConnect()<<endl;
	//cout<<"debug descendant_count in root is "<<tree.treeNode[tree.root].descendant_count<<endl; 

}
CSNode * highest_capacity0(CSNode *z){
	CSNode *p,*q;
	q=p=z;
	if(p==NULL)return &tree.treeNode[tree.root];
	while(p->father){
		p=p->father;	
		if(p->capacity==0){
			q=p;	
		}
	}
	return q;
}
bool transfer_p1(Tree *t,CSNode *y,CSNode *x){
	int i=0;
	CSNode *z,*vs;
	i=0;
	//cout<<"transfer_p1"<<endl;
	while(y->neighborIndexs[i]){
		z=&t->treeNode[y->neighborIndexs[i]];
		if(!isDescendantOf(z,x)){
				//cout<<"try y="<<y->id<<" 's neighbor "<<z->id<<endl;
				if(z->capacity > y->descendant_count){
					//cout<<" case 0: node y="<<y->id <<"who's father is x="<<x->id<<" is transfered to node z="<<z->id<<endl;
					//y->visited=true;
					exChange(y,z);
					return true;
				}else if(z->capacity==0&&k1>0){
					vs=highest_capacity0(z);
					//cout<<"case 1"<<endl;
					if(vs->visited==false){
						vs->visited=true;
						//cout<<"try to transfer z "<<z->id<<"s ascendant node "<<vs->id<<endl;
						if(z->capacity>y->descendant_count&&transfer(t,vs,k1-1)){
							/*
							cout<<"succeed"<<endl;
							cout<<" case 1: node "<<y->id <<"who's father is "<<x->id<<" is transfered to node"<<z->id<<endl;
							cout<<"descendan number of x= "<<x->id<<" is "<<x->descendant_count<<endl;
							*/
							exChange(y,z);
							return true;	
						}else{
							//cout<<"failed"<<endl;
						}	
					}else{
						//cout<<"node "<<vs->id<<" is visited"<<endl;	
					}
				}else{
					//cout<<"other capacity case"<<endl;
				}
		}
		//cout<<"interator"<<endl;		
		i++;	
	}
	//cout<<"no neigher is avaliable... try y="<<y->id<<"s decendant"<<endl;
	if(transfer(t,y,k1)){
		//cout<<"in descendan number of y= "<<y->id<<" is "<<y->descendant_count<<endl;
		return true;
	}
	return false;
}

bool transfer(Tree *t,CSNode *x ,int k1){
	CSNode *y;
	
	if(x->firstchild!=NULL){
			y=x->firstchild;
			if((y->capacity==x->capacity)&&y->visited==false){
				//cout<<"firstchilde"<<endl;
				return transfer_p1(t,y,x);
			}
			while(y->nextsibling){
				y=y->nextsibling;
				if((y->capacity==x->capacity)&&y->visited==false){
					return transfer_p1(t,y,x);
				}
			}
			//cout<<"return false"<<endl;
			return false;
	}
	return false;
}
int isNodeDead(Tree *t){
	for(int i=1;i<=t->size;i++){
		if(tree.treeNode[i].residual_energy<=0){
			return i;	
		}	
	}
	return 0;	
}