#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

using namespace std;

int rout_table[100][100][3];// index = 0:next, 1 : cost, 2 : link
int top_table[100][100][3];
int node_num;
FILE *out_fp;
vector<string> msg_s;
vector<vector<int>> v;
vector<vector<int>> ch;

void rout_init(int start, int end, int cost);
void print_rout_table();
void print_msg();

int main(int argc, char *argv[])
{
    FILE *top_fp, *msg_fp, *chg_fp;

    char temp[100];

    //실행 시 입력 조건에 맞게 입력하였는지 확인.
    if(argc != 4){
        printf("usage: distvec topolygyfile messagefile changesfile\n");
        exit(1);
    }
    top_fp = fopen(argv[1],"r");
    msg_fp = fopen(argv[2],"r");
    chg_fp = fopen(argv[3],"r");
    if(top_fp == NULL || msg_fp == NULL|| chg_fp == NULL){
        printf("Error: open input file\n");
        if(top_fp != NULL)
            fclose(top_fp);
        if(msg_fp != NULL)
            fclose(msg_fp);
        if(chg_fp != NULL)
            fclose(chg_fp);
        exit(1);
    }
    //End: 입력 조건, 입력 파일의 정상적인 실행여부 확인.

    out_fp = fopen("output_dv.txt","w");//출력파일 생성.

    int x,y,z;

    //message file을 입력 받아서 from-to의 숫자 정보를 int vector에, message를 string vector에 저장.
    while(fscanf(msg_fp,"%d %d %[^\n]%*c",&x,&y,temp) != EOF){
        vector<int> t = {x,y};
        v.push_back(t);
        string str(temp);
        msg_s.push_back(str);
    }
    //End: message file 내용 입력.

    //changes file를 입력 받아서 update정보를 int vector에 저장.
    while(fscanf(chg_fp,"%d %d %d",&x,&y,&z) != EOF){
        vector<int> t = {x,y,z};
        ch.push_back(t);
    }//End: changes file의 내용 입력.

    fscanf(top_fp,"%d",&node_num);//node의 갯수 입력.

    int i, j, k,cost;

    //배열 초기화.
    for(i = 0;i<node_num;i++){
        for(j = 0;j<node_num;j++){
            for(k = 0;k<3;k++){
                rout_table[i][j][k] = 0;//rout_table: distvec의 과정과 결과를 담기 위한 배열
                top_table[i][j][k] = 0;//top_table: neighbor간의 정보 교환이 되더라도 neighbor간의 link상태와 cost 등을 바로 파악하기 위한 배열.
            }
        }
    }//End: 배열 초기화.

    //topology file 내용 입력.
    while(fscanf(top_fp,"%d %d %d",&i,&j,&cost) != EOF){
        rout_table[i][j][2] = 1;
        rout_table[i][j][1] = cost;
        rout_table[i][j][0] = j;

        rout_table[j][i][2] = 1;
        rout_table[j][i][1] = cost;
        rout_table[j][i][0] = i;

        top_table[i][j][2] = 1;
        top_table[i][j][1] = cost;
        top_table[i][j][0] = j; 

        top_table[j][i][2] = 1;
        top_table[j][i][1] = cost;
        top_table[j][i][0] = i;
    }//End: topology 내용 입력

    //distvector 방식을 이용해 neighbor간의 정보 교환 및 rout_table 생성 과정.
    for(x = 0;x <= (int)ch.size();x++){
        //한번의 교환으로 모든 정보가 제대로 교환 될 수 없기 때문에 node 갯수의 절반만큼 교환 과정을 반복.
        for(int l = 0;l<node_num/2;l++){
            for(i = 0;i<node_num;i++){
                for(j = 0;j<node_num;j++){
                    if(i == j) {
                        rout_table[i][j][0] = i;
                        continue;
                    }
                    if(rout_table[i][j][2] == 1){
                        int cost = top_table[i][j][1];
                        for(k = 0;k<node_num;k++){
                            if(i == k || j == k) continue;
                            int c, d;
                            c = rout_table[i][k][1];
                            d = rout_table[j][k][1];
                            if(c == 0 && d == 0) continue;
                            if(c == 0){
                                rout_table[i][k][1] = cost + d;
                                rout_table[i][k][0] = j;
                            }
                            else if(d == 0){
                                rout_table[j][k][1] = cost + c;
                                rout_table[j][k][0] = i;
                            }
                            else{
                                if(c > cost + d){
                                    rout_table[i][k][1] = cost + d;
                                    rout_table[i][k][0] = j;
                                }
                                else if(c + cost <  d){
                                    rout_table[j][k][1] = cost + c;
                                    rout_table[j][k][0] = i;
                                }
                                else if(c == cost + d){
                                    if(rout_table[i][k][0] > j)
                                        rout_table[i][k][0] = j;
                                }
                                else if(c + cost == d){
                                    if(i < rout_table[j][k][0]){
                                        rout_table[j][k][0] = i;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }//End: neighbor간의 정보 교환

        print_rout_table();// 출력파일에 rout_table 및 message 출력.

        if(x == (int)ch.size()) break;//모든 update가 진행 되면 반복문 종료.

        rout_init(ch[x][0],ch[x][1],ch[x][2]);//update정보에 따라서 배열을 초기화.
    }//End: 모든 update에 따른 결과를 출력파일에 출력.   
    
    fclose(top_fp);
    fclose(msg_fp);
    fclose(chg_fp);
    fclose(out_fp);

    printf("Complete. Output file written to output_dv.txt.\n");
    return 0;
}

void rout_init(int start, int end, int cost)
{
    if(cost == -999)
    {
        top_table[start][end][2] = 0;
        top_table[start][end][1] = 0;
        top_table[start][end][0] = 0;

        top_table[end][start][2] = 0;
        top_table[end][start][1] = 0;
        top_table[end][start][0] = 0;
    }
    else{
        top_table[start][end][2] = 1;
        top_table[start][end][1] = cost;
        top_table[start][end][0] = end;

        top_table[end][start][2] = 1;
        top_table[end][start][1] = cost;
        top_table[end][start][0] = start;
    }

    for(int i = 0;i < node_num;i++){
        for(int j = 0;j < node_num;j++){
            for(int k = 0; k < 3;k++){
                rout_table[i][j][k] = top_table[i][j][k];
            }
        }
    }
}

void print_rout_table()
{
    for(int i = 0;i<node_num;i++){
        for(int j = 0;j < node_num; j++){
            if(i != j && rout_table[i][j][1] == 0) continue;
            fprintf(out_fp,"%d ",j);
            for(int k = 0;k<2;k++){
                fprintf(out_fp, "%d ",rout_table[i][j][k]);
            }
            fprintf(out_fp,"\n");
        }
        fprintf(out_fp,"\n");
    }

    print_msg();
}

void print_msg(){
    //char temp[100];
    for(int i = 0;i < (int)v.size();i++){
        int start, end,j;
        start = v[i][0];
        end = v[i][1];
        fprintf(out_fp,"from %d to %d cost ",start,end);
        if(rout_table[start][end][1] == 0)
            fprintf(out_fp,"infinite hops unreachable ");
        else{
            fprintf(out_fp,"%d hops ",rout_table[start][end][1]);
            for(j = start;;){
                fprintf(out_fp,"%d ",j);
                j = rout_table[j][end][0];
                if(j == end) break;
            }
         }
        fprintf(out_fp,"message %s\n",msg_s[i].c_str());
    }
    fprintf(out_fp,"\n");
}