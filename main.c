
#include <time.h>
#include <string.h>
//#include "smm_object.h"
#include "smm_database.h"
#include "smm_common.h"

#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH "marbleFoodConfig.txt"
#define FESTFILEPATH "marbleFestivalConfig.txt"



//board configuration parameters
static int board_nr;
static int food_nr;
static int festival_nr;

static int player_nr;


typedef struct player {
        int energy;
        int position;
        char name[MAX_CHARNAME];
        int accumCredit;
        int flag_graduate;
} player_t;

static player_t *cur_player;
//static player_t cur_player[MAX_PLAYER];

#if 0
static int player_energy[MAX_PLAYER];
static int player_position[MAX_PLAYER];
static char player_name[MAX_PLAYER][MAX_CHARNAME];
#endif

//function prototypes
#if 0
int isGraduated(void); //check if any player is graduated
 //print grade history of the player
void goForward(int player, int step); //make player go "step" steps on the board (check if player is graduated)
void printPlayerStatus(void); //print all player status at the beginning of each turn
float calcAverageGrade(int player); //calculate average grade of the player
smmGrade_e takeLecture(int player, char *lectureName, int credit); //take the lecture (insert a grade of the player)
void* findGrade(int player, char *lectureName); //find the grade from the player's grade history
void printGrades(int player); //print all the grade history of the player
#endif


void printGrades(int player)// player 매개변수에 해당하는 플레이어의 성적을 출력
{
     int i;
     void *gradePtr;
     for (i=0;i<smmdb_len(LISTNO_OFFSET_GRADE + player);i++)//성적 데이터를 순회하고 특정 플레이어의 성적 리스트의 길이를 반환
     {
         gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);//현재 반복 인덱스 i에 해당하는 특정 플레이어의 성적 데이터를 가져와서 gradePtr에 저장
         printf("%s : %i\n", smmObj_getNodeName(gradePtr), smmObj_getNodeGrade(gradePtr));//성적 데이터이름,성적 데이터 점수 반환
     }
}

void printPlayerStatus(void)
{
     int i;
     
     for (i=0;i<player_nr;i++)
     {
         printf("%s : credit %i, energy %i, position %i\n",cur_player[i].name,cur_player[i].accumCredit,cur_player[i].energy,cur_player[i].position);
     }
}

void generatePlayers(int n, int initEnergy) //generate a new player
{
     int i;
     //n time loop
     for (i=0;i<n;i++)
     {
         //input name
         printf("Input player %i's name:", i); //플레이어 이름 입력 
         scanf("%s", cur_player[i].name);
         fflush(stdin);
         
         //set position
         //player_position[i] = 0;
         cur_player[i].position = 0;
         
         //set energy
         //player_energy[i] = initEnergy;
         cur_player[i].energy = initEnergy;
         cur_player[i].accumCredit = 0;
         cur_player[i].flag_graduate = 0;
     }
}


int rolldie(int player)
{
    char c;
    printf(" Press any key to roll a die (press g to see grade): ");//주사위 굴리기 
    c = getchar();
    fflush(stdin);
    
#if 0
    if (c == 'g')//g를 누르면 성적출력 
        printGrades(player);
#endif
    
    return (rand()%MAX_DIE + 1);
}

//action code when a player stays at a node
void actionNode(int player)
{
    void *boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position );
    //int type = smmObj_getNodeType( cur_player[player].position );
    int type = smmObj_getNodeType( boardPtr );
    char *name = smmObj_getNodeName( boardPtr );
    void *gradePtr;
    
    switch(type)
    {
        //case lecture:
        case SMNODE_TYPE_LECTURE:
            if(1)
            cur_player[player].accumCredit += smmObj_getNodeCredit( boardPtr );//특정 플레이어의 accumCredit에 해당 성적 데이터의 크레딧 값을 추가하여, 플레이어의 누적 크레딧 업데이트
            cur_player[player].energy -= smmObj_getNodeEnergy( boardPtr );//특정 플레이어의 energy에서 해당 성적 데이터의 에너지 값을 빼서, 플레이어의 에너지를 업데이트
            
            //grade generation
            //gradePtr = smmObj_genObject(name, smmObjType_grade, 0, smmObj_getNodeCredit( boardPtr ), 0, ??);//주어진 정보를 사용하여 새로운 성적 객체를 생성하고, 그 객체에 대한 포인터를 gradePtr에 저장
            smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
            
            break;
            
        default:
            break;
     /*   
    {
    case 0:
        printf("lecture"); //전공과목듣기 
        break;
    case 1:
        printf("restaurant"); //에너지 보충
        break;
    case 2:
        printf("laboratory"); //실험
        break;
    case 3:
        printf("home"); //시작점이자 종료점 (지날때마다 에너지가 보충됨)
        break;
    case 4:
        printf("experiment"); //실험을 하러 가는 노드 (실험실로 이동)
        break;
    case 5:
        printf("foodChance"); //랜덤으로 음식카드를 골라 에너지를 보충할 수 있는 노드
        break;
    case 6:
        printf("festival"); //랜덤으로 미션을 수행하는 노드
        break;
    default:
        break;
    }
    }*/
	}

void goForward(int player, int step)//앞으로 이동 
{
     void *boardPtr;
     cur_player[player].position += step;//player의 position을 이동함 
     boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position );
     
     printf("%s go to node %i (name: %s)\n", //이동한 상황 출력 
                cur_player[player].name, cur_player[player].position,
                smmObj_getNodeName(boardPtr));
}


int main(int argc, const char * argv[]) {
    
    FILE* fp;
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;
    int i;
    int initEnergy;
    int turn=0;
    
    board_nr = 0;
    food_nr = 0;
    festival_nr = 0;
    
    srand(time(NULL));
    
    
    //1. import parameters ---------------------------------------------------------------------------------
    //1-1. boardConfig 
    if ((fp = fopen(BOARDFILEPATH,"r")) == NULL)//보드 열기 실패하였을때 
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", BOARDFILEPATH);
        getchar();
        return -1;
    }
    
    printf("Reading board component......\n");
    while ( fscanf(fp, "%s %i %i %i", name, &type, &credit, &energy) == 4 ) //read a node parameter set
    {
        //store the parameter set
        //(char* name, smmObjType_e objType, int type, int credit, int energy, smmObjGrade_e grade)
        void *boardObj = smmObj_genObject(name, smmObjType_board, type, credit, energy, 0);
        smmdb_addTail(LISTNO_NODE, boardObj);
        
        if (type == SMMNODE_TYPE_HOME)
           initEnergy = energy;
        board_nr++;
    }
    fclose(fp);
    printf("Total number of board nodes : %i\n", board_nr);
    
    
    for (i = 0;i<board_nr;i++)
    {
        void *boardObj = smmdb_getData(LISTNO_NODE, i);
        
        printf("node %i : %s, %i(%s), credit %i, energy %i\n", 
                     i, smmObj_getNodeName(boardObj), 
                     smmObj_getNodeType(boardObj), smmObj_getTypeName(smmObj_getNodeType(boardObj)),
                     smmObj_getNodeCredit(boardObj), smmObj_getNodeEnergy(boardObj));
    }
    //printf("(%s)", smmObj_getTypeName(SMMNODE_TYPE_LECTURE));
    
    #if 0
    //2. food card config 
    if ((fp = fopen(FOODFILEPATH,"r")) == NULL)//파일 오픈 실패시 
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
        return -1;
    }
    
    printf("\n\nReading food card component......\n");
    while (fgets(line, sizeof(line), fp) != NULL) //read a food parameter set 
    {
        //store the parameter set
        
        //읽어오기
        fscanf(fp, "%s %d", &name, &energy);
        printf("%s / %d\n", name, energy);

        //저장하기
        smmObj_genNode(); //object.c에 있는 구조체에 정보저장
        //이후 get함수로 정보를 가져와사용하기  
        
        food_nr++;
    }
    fclose(fp);
    printf("Total number of food cards : %i\n", food_nr);
    
    
    
    
    //3. festival card config 
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)//파일 내용 없을때 
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
        return -1;
    }
    
    printf("\n\nReading festival card component......\n");
    while (fgets(line, sizeof(line), fp) != NULL) //read a festival card string
    {
        //store the parameter set
        
        //읽어오기
        fscanf(fp, "%s", &name);
        printf("%s\n", name);

        //저장하기
        smmObj_genNode(); //object.c에 있는 구조체에 정보저장
        //이후 get함수로 정보를 가져와사용하기  
        
        festival_nr++;
    }
    fclose(fp);
    printf("Total number of festival cards : %i\n", festival_nr);
    #endif
    
    
    //2. Player configuration ---------------------------------------------------------------------------------
   
   
    do
    {
        //input player number to player_nr
        printf("input player no.:");
        scanf("%d", &player_nr);
        fflush(stdin);
    }
    while (player_nr < 0 || player_nr >  MAX_PLAYER); //플레이어수가 0보다 크고 최대 플레이어수보다 작은경우동안 
    
    cur_player = (player_t*)malloc(player_nr * sizeof(player_t));//동적 할당을 사용하여 플레이어 구조체(player_t) 배열 생성
    generatePlayers(player_nr, initEnergy);
    
    
    
    
    
    //3. SM Marble game starts ---------------------------------------------------------------------------------
    //3. festival card config 
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
        return -1;
    }
    
    printf("\n\nReading festival card component......\n");
    char line; 
	while (fgets(line, sizeof(line), fp) != NULL) //read a festival card string
    {
        //store the parameter set
        
        //읽어오기
        fscanf(fp, "%s", &name);
        printf("%s\n", name);

        //저장하기
        smmObj_genNode(); //object.c에 있는 구조체에 정보저장
        //이후 get함수로 정보를 가져와사용하기  
        
        festival_nr++;
    }
    fclose(fp);
    printf("Total number of festival cards : %i\n", festival_nr);
    
    
    
    
    while (1) //is anybody graduated?
    {
        int die_result;
        
        
        //4-1. initial printing
        printPlayerStatus();
        
        //4-2. die rolling (if not in experiment)        
        die_result = rolldie(turn);
        
        //4-3. go forward
        goForward(turn, die_result);

      //4-4. take action at the destination node of the board
        actionNode(turn);
        
        //4-5. next turn
        turn = (turn + 1)%player_nr;
    }
    
    
    free(cur_player);
    system("PAUSE");
    return 0;
}
