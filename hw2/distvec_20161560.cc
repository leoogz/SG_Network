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
    out_fp = fopen("output_dv.txt","w");

    int x,y,z;
    while(fscanf(msg_fp,"%d %d %[^\n]%*c",&x,&y,temp) != EOF){
        vector<int> t = {x,y};
        v.push_back(t);
        string str(temp);
        msg_s.push_back(str);
    }

    while(fscanf(chg_fp,"%d %d %d",&x,&y,&z) != EOF){
        vector<int> t = {x,y,z};
        ch.push_back(t);
    }

    fscanf(top_fp,"%d",&node_num);
    int i, j, k,cost;
    for(i = 0;i<node_num;i++){
        for(j = 0;j<node_num;j++){
            for(k = 0;k<3;k++){
                rout_table[i][j][k] = 0;
                top_table[i][j][k] = 0;
            }
        }
    }
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
    }
    for(x = 0;x <= (int)ch.size();x++){
        for(int l = 0;l<3;l++){
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
                            }
                        }
                    }
                }
            }
        }

        print_rout_table();
        if(x == (int)ch.size()) break;

        rout_init(ch[x][0],ch[x][1],ch[x][2]);
    }   

    fclose(top_fp);
    fclose(msg_fp);
    fclose(chg_fp);
    fclose(out_fp);
    return 0;
}

void rout_init(int start, int end, int cost)
{
    printf("%d %d %d\n",start, end, cost);
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