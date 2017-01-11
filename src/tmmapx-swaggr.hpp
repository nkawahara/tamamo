#ifdef _TMMAPX_SWAGGR_
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
  UINT8 nm_gruop; // for GroupBy
}
  

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


//
// software aggregation
// データをグループに分けて解析する
// * 今回はグループごと最大値を取りだすプログラム．※？ほんまか？
// 
class aggrGroupby{
private:
  int NUM_THREADS = 8;
  int NB_TUPLE = (1000 * 1000 * 10); // 10 million
  int NB_GROUP = 80;

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
    
public:

  aggrGroupby(){

    
  }
  aggrGroupby(int threads){
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
  int getTupleList(){
    return TupleList;
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
      free(GroupUnitList[i].gval);
    }
    free(GroupUnitList);
  }

  void gu_finalize(){
    free(TupleList);
    for(int i=0;i<NUM_THREADS;i++){
      finalize(GULT[i]);
    }
    free(GULT);
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
  
  void printResult(){
    GroupUnit* GU;
    GU = (GroupUnit*)malloc(sizeof(GroupUnit)*NB_GROUP);
    makeGroup(GU);
    for(int i=0; i<NUM_THREADS; i++){
      for(int j=0;j<NB_GROUP;j++){
	GU[j].gval[0].val += GULT[i][j].gval[0].val; // COUNT
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


/*
int main(int argc, char** argv){
  struct timeval begin, end;
  
  aggrGroupby gb;
  gb.init();
  gettimeofday(&begin, NULL);
  gb.gu_makeGroup();
  gb.execGroupBy();
  gettimeofday(&end, NULL);
  gb.printResult();
  gb.gu_finalize();
  gb.printTime(begin, end);
  return 0;
};

*/


#endif
