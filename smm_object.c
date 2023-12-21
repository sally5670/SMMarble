
#include "smm_common.h"
#include "smm_object.h"
#include <string.h>

#define MAX_NODETYPE    7
#define MAX_GRADE       9
#define MAX_NODE        100


static char smmNodeName[SMMNODE_TYPE_MAX][MAX_CHARNAME] = {//노드 이름 정의 
       "강의",
       "식당",
       "실험실",
       "집",
       "실험실로이동",
       "음식찬스",
       "축제시간"
};

char* smmObj_getTypeName(int type)
{
      return (char*)smmNodeName[type];
}



typedef enum smmObjGrade {//성적 정의 
    smmObjGrade_Ap = 0,
    smmObjGrade_A0,
    smmObjGrade_Am,
    smmObjGrade_Bp,
    smmObjGrade_B0,
    smmObjGrade_Bm,
    smmObjGrade_Cp,
    smmObjGrade_C0,
    smmObjGrade_Cm
} smmObjGrade_e;

//1. 구조체 형식 정의
typedef struct smmObject {
       char name[MAX_CHARNAME];
       smmObjType_e objType; 
       int type;
       int credit;
       int energy;
       smmObjGrade_e grade;
} smmObject_t;

//static smmObject_t smm_node[MAX_NODE];
//static int smmObj_noNode = 0;

//3. 관련 함수 변경 
//object generation
void* smmObj_genObject(char* name, smmObjType_e objType, int type, int credit, int energy, smmObjGrade_e grade)// SM Marble 게임의 객체 생성
{    
    smmObject_t* ptr;
    // 동적 메모리 할당을 통해 smmObject_t 구조체 크기만큼의 메모리를 할당하고 
    ptr = (smmObject_t*)malloc(sizeof(smmObject_t));
    // 매개변수로 전달된 값들을 새로 생성한 객체에 설정
    strcpy(ptr->name, name);
    ptr->objType = objType;
    ptr->type = type;
    ptr->credit = credit;
    ptr->energy = energy;
    ptr->grade = grade;
    // 생성한 객체에 대한 포인터를 반환
    return ptr;
}

//3. 관련 함수 변경 
char* smmObj_getNodeName(void* obj)
{
    smmObject_t* ptr = (smmObject_t*)obj;
    
    return ptr->name;
}

//3. 관련 함수 변경 
int smmObj_getNodeType(int node_nr)
{
    return smm_node[node_nr].type;
}

int smmObj_getNodeCredit(int node_nr)
{
    return smm_node[node_nr].credit;
}

int smmObj_getNodeEnergy(int node_nr)
{
    return smm_node[node_nr].energy;
}
