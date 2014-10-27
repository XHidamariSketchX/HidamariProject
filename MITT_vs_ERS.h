/*
 * MITT_vs_ERS.h
 *
 *  Created date: 2014Äê8ÔÂ8ÈÕ
 *      Author: xuji
 */

#ifndef MITT_vs_ERS_H_
#define MITT_vs_ERS_H_
#include <stdlib.h>

#include <iostream.h >

#define MAX_TREE_SIZE 350


#define QUEUE_SIZE MAX_TREE_SIZE+1
#define DEGREE 10

#define LOC_SIZE 100 //100 x 100 m2
#define ROOT_LOC LOC_SIZE/2 //root position (500,500)
#define DIS 25 //transmission distance
//#define MIN_DIS 30
#define DATA_COUNT_EACH_NODE 1
#define MAC_HEADER 12

#define PERCENT 100
//#define PK 50//NOTE PK to M should consider later
#define MAX_RETRY 3 	//sigma
#define LAMBDA 3			//lambda
#define MAX_APPEND_COUNT 4	//m segment size
#define SENSOR_DATA_SIZE 4 //cita in Bytes
//NOTE SENSOR_DARA_SIZE*MAX_APPEND_COUNT<=100

#define ACK_SIZE 5//5 Bytes

#define RE_EXP 2

#define SEND_CHOICE_ARQ 1
#define SEND_CHOICE_E_RS 2
#define SEND_CHOICE_DIR 3
// MITT define the TX and RX one bit energy is Etx and Erx = Etx/2

#define ETX 100 //nj/bit
#define ERX ETX/2


#define NODETYPE_V1 1
#define NODETYPE_V2 2
#define NODETYPE_V3 3

typedef struct Node {
	int id;	//node id
	int nodeType; //
	bool visited;
	int descendant_count;
	int dataCount;	//x
	double weight; //omega
	double capacity;
	double energy;
	double init_energy;
	double residual_energy;
	int x;
	int y;
	double dis_to_father;	//dk
	int neighborIndexs[MAX_TREE_SIZE];
	struct Node * firstchild, *nextsibling,* father;

}CSNode;
typedef struct t{
	int size;
	int root;
	double max_weight;
	CSNode treeNode[MAX_TREE_SIZE+1]; //root is index 0
}Tree;

class Queue{
	private :
		int front;
		int rear;
		CSNode *q[QUEUE_SIZE];
		
	public :
		Queue(){
			rear=front=0;	
		}
		void showQueue(){
			int i;
			cout<<"showQueue id list:"<<endl;
			for(i=front;i!=rear;i=(i+1)%(QUEUE_SIZE)){
				cout<<q[i]->id<<"with weight="<<q[i]->weight<<endl;	
			}
			cout<<"showQueue ENd"<<endl;	
		}
		void clearQueue(){
			front=rear=0;	
		}
		bool isEmpty(){
			if(front==rear) return true;
			else return false;
		}	
		bool isFull(){
			if(((rear+1)%(QUEUE_SIZE))==front)return true;
			else return false;	
		}
		CSNode * dequeue(){
			CSNode * frontNode;
			
			if(!isEmpty()){
				frontNode=q[front];
				front++;
				front%=QUEUE_SIZE;
				return 	frontNode;
			}
			else{
				return NULL;	
			}
		}
		bool enqueue(CSNode *p){
			//cout<<"enqueue(): size="<<size()<<endl;
			//cout<<"rear= "<<rear<<"front= "<<front<<endl;
			if(!isFull()){
				q[rear]=p;
				//cout<<"be debug++:rear="<<rear<<endl;
				rear++;
				rear%=QUEUE_SIZE;
				//rear=(rear+1)%QUEUE_SIZE;
				return true;
			}else{
				cout<<"enqueue() QUEUE IS FULL"<<endl;
				return false;	
			}
		}
		int size(){
			/*
			cout<<"rear="<<rear<<endl;
			cout<<"front="<<front<<endl;
			cout<<"r-f+QUEUE_SIZE="<<rear-front+QUEUE_SIZE<<endl;
			cout<<"(rear-front+QUEUE_SIZE)%QUEUE_SIZE="<<((rear-front+QUEUE_SIZE)%(QUEUE_SIZE))<<endl;
			cout<<"QUEUE_SIZE ="<<QUEUE_SIZE<<endl;
			cout<<"56%51="<<56%51<<endl;
			*/
			return (rear-front+QUEUE_SIZE)%(QUEUE_SIZE);
		}
		void sort(){
			CSNode *tmp[QUEUE_SIZE];
			CSNode * ex;
			int i,j=0;
			int len=0;
			for(i=front;i!=rear;i=(i+1)%(QUEUE_SIZE)){
				tmp[j]=q[i];
				j++;
			}
			if(j!=size()){
				cout<<"need DEBUG!!"<<endl;	
			}	
			//sort tmp
			if(size()>=2){
				for(i=0;i<size();i++){
					for(j=size()-1;j>i;j--)
					{
						if((tmp[j-1]->weight) > (tmp[j]->weight)){
								
						}else{
							ex=tmp[j];
							tmp[j]=tmp[j-1];
							tmp[j-1]=ex;
						}
					}	
				}
				len=size();
				//cout<<"len="<<len<<endl;
				for(i=0;i<size();i++){
					q[i]=tmp[i];
				}
				front=0;
				rear=len;
				//cout<<"len="<<size()<<endl;
			}
		}
};

#endif /* TREEENERGY_H_ */
