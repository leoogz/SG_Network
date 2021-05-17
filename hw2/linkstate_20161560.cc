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
    
    //파일 실행 시 입력 조건에 맞는지 확인.
    if(argc != 4){
        printf("usage: linkstate topolygyfile messagefile changesfile\n");
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
    //End: 입력 조건 확인.

    out_fp = fopen("output_ls.txt","w");//출력 파일 생성.

    //message file의 내용을 받아서 from-to에 대한 int vector와 message문자열에 대한 string vector에 저장.
    int x,y,z;
    while(fscanf(msg_fp,"%d %d %[^\n]%*c",&x,&y,temp) != EOF){
        vector<int> t = {x,y};
        v.push_back(t);
        string str(temp);
        msg_s.push_back(str);
    }
    //End: message file 내용 입력.

    //changes file의 내용을 받아서 vector에 문자열로 저장.
    while(fscanf(chg_fp,"%d %d %d",&x,&y,&z) != EOF){
        vector<int> t = {x,y,z};
        ch.push_back(t);
    }
    //End: changes file 내용 입력.

    
    fscanf(top_fp,"%d",&node_num);//node의 갯수 입력.

    //배열 초기화.
    int i, j, k,cost;
    for(i = 0;i<node_num;i++){
        for(j = 0;j<node_num;j++){
            for(k = 0;k<3;k++){
                rout_table[i][j][k] = 0;
                top_table[i][j][k] = 0;
            }//rout_table과 top_table 초기화
            //update시에 link 상태를 파악하기 위해 그래프 정보를 top_table에 저장.
            if(i == j) rout_table[i][j][0] = i;
        }
    }
    

    
    //topology file을 입력 받아서 내용을 배열에 저장.
    while(fscanf(top_fp,"%d %d %d",&i,&j,&cost) != EOF){
        top_table[i][j][2] = 1;
        top_table[i][j][1] = cost;
        top_table[i][j][0] = j; 

        top_table[j][i][2] = 1;
        top_table[j][i][1] = cost;
        top_table[j][i][0] = i;
    }
    //End: topology file 내용 입력.
    
    //Dijkstra algorithm을 통해 rout_table 생성.
    for(int l = 0; l <= (int)ch.size();l++){
        for(i = 0;i < node_num;i++){
            vector<int> SPT;
            vector<int> visit(node_num,0);
            vector<int> d(node_num, 10000);
            vector<int> p(node_num, -1);
            SPT.push_back(i);
            int check = 0;
            
            for(j = 0;j<node_num;j++){
                if(top_table[i][j][2] == 1){
                    d[j] = top_table[i][j][1];
                    p[j] = i;
                    check++;
                }
            }
            if(check == 0) continue;
            visit[i] = 1;
            while((int)SPT.size() < node_num-1){
                int next, t = 100000;
                for(k = 0;k<node_num;k++){
                    if(visit[k] == 1) continue;
                    if(p[k] != -1){
                        if(t > d[k]){
                            t = d[k];
                            next = k;
                        }
                    }
                }
                SPT.push_back(next);
                visit[next] = 1;
                for(k = 0;k < node_num;k++){
                    if(visit[k] == 1) continue;
                    
                    if(top_table[next][k][2] == 1){
                        if(d[k] > top_table[next][k][1] + d[next] || d[k] == 10000){
                            d[k] = top_table[next][k][1] + d[next];
                            p[k] = next;
                        }
                    }      
                    
                }
                
            }
            
            //converge to rout table.
            
            for(j = 0;j<node_num;j++){
                if(j == i) {
                    rout_table[i][j][0] = j; 
                    continue;
                }
                if(p[j] == -1) continue;//만일 update를 통해 network에서 link가 끊어져 있는 경우.

                x = p[j];
                y = j;
                while(x != i){
                    y = x;
                    x = p[y];
                }
                
                rout_table[i][j][0] = y;
                rout_table[i][j][1] = d[j];                  
            }
        }

        print_rout_table();//rout_table을 출력파일에 출력.
        
        
        if(l == (int)ch.size()) break;//changes 파일을 통해 update되는 횟수가 모두 끝났다면 반복문 종료.
        
        
        rout_init(ch[l][0],ch[l][1],ch[l][2]);//update와 배열 초기화.
        
    }
    
    fclose(top_fp);
    fclose(msg_fp);
    fclose(chg_fp);
    fclose(out_fp);

    printf("Complete. Output file written to output_ls.txt.\n");
    return 0;
}

//배열의 update와 초기화.
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
            for(int k = 0; k < 2;k++){
                rout_table[i][j][k] = top_table[i][j][k];
            }
        }
    }
}

//출력파일에 양식에 맞게 출력.
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

//message 출력.
void print_msg(){
    //char temp[100];
    for(int i = 0;i < (int)v.size();i++){
        int start, end,j;
        start = v[i][0];
        end = v[i][1];
        fprintf(out_fp,"from %d to %d cost ",start,end);
        if(rout_table[start][end][1] == 0)
            fprintf(out_fp,"infinite hops unreachable ");//전달받은 경로로 이동이 불가한 경우의 메세지.
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