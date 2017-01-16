#ifndef _TMMAPX_SWAGGR_
#define _TMMAPX_SWAGGR_

#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <list>
#include <map>
#include <iostream>
#include <sys/time.h>
#include <float.h>

#ifdef _OPENMP
#include <omp.h>
#endif

using namespace std;


typedef struct aggropeItem{
  UINT64 uvsa_addr;
  UINT32 dram_addr;
  UINT32 dsize;
  UINT8 rwFlag;
  UINT8 stateFlag; //state transition
  UINT8 idc_flag;
  UINT8 nb_group; // for GroupBy
}aggropeItem;
  
//********************************
// クラスaggrGroupBy用ライブラリ
typedef enum {MAX, MIN, COUNT} TYPE;

typedef struct _Tuple{
  int key;
  int val1;
  int val2; // for max
  int val3; // for min
}Tuple;

typedef struct _GroupVal{
  TYPE type;
  double val;
}GroupVal;

typedef struct _GroupUnit
{
  int gkey;
  GroupVal* gval;
}GroupUnit;
//*********************************


void tmm_aggrOpeAsgmt(aggropeItem *temp,
		 UINT64 uvsa_addr,  UINT32 dram_addr,
		 UINT32 dsize,  UINT8 rwFlag,  UINT8 stateFlag, UINT8 idc_flag,
		 UINT8 nb_group){

  temp->uvsa_addr = uvsa_addr;
  temp->dram_addr = dram_addr;
  temp->dsize = dsize;
  temp->rwFlag = rwFlag;
  temp->stateFlag = stateFlag;
  temp->idc_flag = idc_flag;
  temp->nb_group = nb_group;
}


void tmm_aggrOpePrint(aggropeItem*temp){
  fprintf(stderr,
          "[tamamo DEBUG] uvsa:%lx addr(%x) size(%d) flag:%d flag:%d IDC:%d nb_group:%d\n",
          temp -> uvsa_addr,
          temp -> dram_addr,
          temp -> dsize,
          temp -> rwFlag,
          temp -> stateFlag,
          temp -> idc_flag,
          temp -> nb_group);
}

//
// software aggregation クラス
// データをグループに分けて解析する
// * 今回はグループごと最大値を取りだすプログラム．※？ほんまか？
// 
class aggrGroupby{ 
private:
  UINT8 NUM_THREADS;
  UINT32 NB_TUPLE ; //(1000 * 1000 * 10); // 10 million
  UINT8 NB_GROUP ; //80;

  Tuple* TupleList;
  GroupUnit** GULT; // Group Unit List Thread
  map<int, int> mp;
  
  vector<string> split(string& input, char delimiter)
  {
    istringstream stream(input);
    string field;
    vector<string> result;
    while (getline(stream, field, delimiter)) {
      result.push_back(field);
    }
    return result;
  }

  void makeGroup(GroupUnit* GroupUnitList){
    int i;
    i = 0;
    for(map<int, int>::iterator itr=mp.begin(); itr != mp.end(); ++itr){
      // for(int i=0;i<NB_GROUP; i++){
      // GroupUnitList[i].gkey = i;
      GroupUnitList[i].gkey = itr->first;
      GroupUnitList[i].gval = (GroupVal*)malloc(sizeof(GroupVal)*3);
      GroupUnitList[i].gval[0].type = COUNT;
      GroupUnitList[i].gval[0].val  = 0;
      GroupUnitList[i].gval[1].type = MAX;
      GroupUnitList[i].gval[1].val  = 0;
      GroupUnitList[i].gval[2].type = MIN;
      GroupUnitList[i].gval[2].val  = DBL_MAX;
      i++;
    }
  }

  void finalize(GroupUnit* GroupUnitList){
    for(int i=0;i<NB_GROUP; i++){
      //free(GroupUnitList[i].gval);
    }
    //free(GroupUnitList);
  }
  void printResult(GroupUnit* GroupUnitList){
    for(int j=0; j < NB_GROUP; j++){
      cout << GroupUnitList[j].gkey;
      for(int k=0;k<3;k++){
	printf(" : %8.7f", GroupUnitList[j].gval[k].val);
      }
      cout << endl;
    }
  }
  
  


    
public:
  aggrGroupby(){
    fprintf(stderr,"start\n");
  }
  aggrGroupby(int threads){
    fprintf(stderr,"start (threads = %d)\n",threads);
    NUM_THREADS = threads;
  }
  
  //
  // utility methods
  //
  int getNBTuple(){
    return NB_TUPLE;
  }
  int getNBGroup(){
    return NB_GROUP;
  }
  Tuple* getTupleList(){
    return TupleList;
  }
  void setNBTuple(UINT32 temp_nb){
    NB_TUPLE = temp_nb;
  }
  void setNBGroup(UINT8 temp_nb){
    NB_GROUP = temp_nb;
  }
  void setTupleList(Tuple * tempTupleList){
    TupleList = tempTupleList;
  }
  void printParam(){
    fprintf(stderr,"tuple:%d group:%d thread:%d\n",NB_TUPLE, NB_GROUP, NUM_THREADS);
  }

  
  //ローカルのファイルから読み取って
  void makeTuple(){
    
    ifstream f;
    f.open("./data/NASDAQ_merge.csv", std::ios::in);
    vector<vector<string> > values;
    string str;
    if(f.fail()){
      cerr << "failed." << endl;
      exit(1);
    }
    
    while(f && getline(f, str)){
      vector<string> strvec;
      strvec = split(str, ',');
      values.push_back(strvec);
    }
    
    NB_TUPLE = (int)values.size();
    TupleList = (Tuple*)malloc(sizeof(Tuple)*NB_TUPLE);
    
    for(int i=0;i<NB_TUPLE;i++){
      char* a;
      //TupleList[i].key  = std::atoi(values[i][0].c_str());
      a = (char*)values[i][0].c_str();
      TupleList[i].key  = a[0]+ (a[1]<<8) + (a[2]<<16) + (a[3]<<24);
      if(mp[TupleList[i].key] == 0){
	mp[TupleList[i].key] = 1;
      }
      TupleList[i].val1 = atoi(values[i][1].c_str());
      TupleList[i].val2 = atoi(values[i][2].c_str());
      TupleList[i].val3 = atoi(values[i][3].c_str());
    }
    NB_GROUP = mp.size();
    //NB_GROUP = 16;

  }

  // TODO 今はこれで動くけど
  // 将来的にはmpをtmm_sendできるようにする（して）
  void makeMP(){
    for(int i=0;i<NB_TUPLE;i++){
      if(mp[TupleList[i].key] == 0){
	mp[TupleList[i].key] = 1;
      }
    }
  }

  void printResult(){
    GroupUnit* GU;
    GU = (GroupUnit*)malloc(sizeof(GroupUnit)*NB_GROUP);
    makeGroup(GU);

    
    for(int i=0; i<NUM_THREADS; i++){
      for(int j=0;j<NB_GROUP;j++){
	GU[j].gval[0].val += GULT[i][j].gval[0].val;
	if(GU[j].gval[1].val < GULT[i][j].gval[1].val){
	  GU[j].gval[1].val = GULT[i][j].gval[1].val;
	}
	if(GU[j].gval[1].val < GULT[i][j].gval[1].val){
	  GU[j].gval[1].val = GULT[i][j].gval[1].val;
	}
	if(GU[j].gval[2].val > GULT[i][j].gval[2].val){
	  GU[j].gval[2].val = GULT[i][j].gval[2].val;
	}
      }
    }
    
    
    for(int j=0;j<NB_GROUP;j++){
      cout << GU[j].gkey;
      for(int k=0;k<3;k++){
	printf(" : %8.0f", GU[j].gval[k].val);
      }
      cout << endl;
    }
    
    finalize(GU);
  }

  void gu_finalize(){
    for(int i=0;i<NUM_THREADS;i++){
      finalize(GULT[i]);
    }
    free(GULT);
  }

  void printTime(struct timeval begin, struct timeval end){
    long usec = (end.tv_sec - begin.tv_sec) * 1000 * 1000
      + (end.tv_usec - begin.tv_usec);
    cout << "Latency:    " << usec << endl;
    cout << "Throughput: "
	 << (double)NB_TUPLE / ((double)usec / (double)(1000 * 1000)) << endl;
  }

  
  //
  //exec
  //
  void gu_makeGroup(){
    GULT = (GroupUnit**)malloc(sizeof(GroupUnit*) * NUM_THREADS);
    
    for(int i=0;i<NUM_THREADS;i++){
      GULT[i] = (GroupUnit*)malloc(sizeof(GroupUnit)*NB_GROUP);
      makeGroup(GULT[i]);
    }
  }
  
  void execGroupBy(){
    int th_id;
    omp_set_num_threads(NUM_THREADS);
    
#pragma omp parallel for
    for(int i=0;i<NB_TUPLE;i++){
      for(int j=0;j<NB_GROUP;j++){
	th_id = omp_get_thread_num();
	if(TupleList[i].key == GULT[th_id][j].gkey){
	  // cout << th_id << " " << i << " " << j << " "
	  //      << TupleList[i].val1 << endl;
	  for(int k=0;k<3;k++){
	    if(GULT[th_id][j].gval[k].type == COUNT){
	      GULT[th_id][j].gval[k].val += 1.0;
	    }
	    else if(GULT[th_id][j].gval[k].type == MAX){
	      if(GULT[th_id][j].gval[k].val < (double)TupleList[i].val2){
		GULT[th_id][j].gval[k].val = (double)TupleList[i].val2;
	      }
	    }
	    else if(GULT[th_id][j].gval[k].type == MIN){
	      if(GULT[th_id][j].gval[k].val > (double)TupleList[i].val3){
		GULT[th_id][j].gval[k].val = (double)TupleList[i].val3;
	      }
	    }
	    else{
	      exit(1);
	    }
	  }
	}
      }
    }
  }


};

void tamamo_swGB(aggropeItem* daemonOpe, Tuple * TupleList, int n_threads){
  struct timeval begin, end;

  aggrGroupby gb(n_threads);
  gb.printParam();

  //fprintf(stderr,"%d, %d\n",daemonOpe->dsize/sizeof(Tuple), daemonOpe->nb_group);
  gb.setNBTuple((daemonOpe->dsize)/sizeof(Tuple));
  gb.setNBGroup(daemonOpe->nb_group);
  gb.setTupleList(TupleList);
  gb.printParam();
  gb.makeMP();

  gettimeofday(&begin, NULL);
  gb.gu_makeGroup(); // 1 
  gb.execGroupBy(); // 2

  gettimeofday(&end, NULL);
  gb.printResult();
  gb.gu_finalize();
  gb.printTime(begin, end);  
}

/*
int main(int argc, char** argv){
  struct timeval begin, end;
  
  aggrGroupby gb;
  gb.init();

  gettimeofday(&begin, NULL);
  gb.gu_makeGroup(); // 1 
  gb.execGroupBy(); // 2

  gettimeofday(&end, NULL);
  gb.printResult();
  gb.gu_finalize();
  gb.printTime(begin, end);
  return 0;
};

*/


#endif
