#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

using namespace std;

int rout_table[50][50][3];// index = 0:destination, 1 : next, 2 : cost
int node_num;
FILE *out_fp;
vector<string> msg_s;
vector<vector<int>> v;

void print_rout_table();
void print_msg();

int main(int argc, char *argv[])
{
    FILE *top_fp, *msg_fp, *chg_fp;
    int chg_start, chg_end, chg_cost;
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

    int x,y;
    while(fscanf(msg_fp,"%d %d %[^\n]%*c",&x,&y,temp) != EOF){
        vector<int> t = {x,y};
        v.push_back(t);
        string str(temp);
        msg_s.push_back(str);
    }

    fscanf(top_fp,"%d",&node_num);
    //while(1){
        int i, j, k,cost;
        for(i = 0;i<node_num;i++){
            for(j = 0;j<node_num;j++){
                for(k = 0;k<3;k++)
                    rout_table[i][j][k] = 0;
            }
        }
        while(fscanf(top_fp,"%d %d %d",&i,&j,&cost) != EOF){
            rout_table[i][j][2] = 1;
            rout_table[i][j][1] = cost;
            rout_table[i][j][0] = j;

            rout_table[j][i][2] = 1;
            rout_table[j][i][1] = cost;
            rout_table[j][i][0] = i;
        }

        for(int l = 0;l<2;l++){
            for(i = 0;i<node_num;i++){
                for(j = 0;j<node_num;j++){
                    if(i == j) {
                        rout_table[i][j][0] = i;
                        continue;
                    }
                    if(rout_table[i][j][2] == 1){
                        int cost = rout_table[i][j][1];
                        for(k = 0;k<node_num;k++){
                            if(i == k || j == k) continue;
                            int c, d;
                            c = rout_table[i][k][1];
                            d = rout_table[j][k][1];
                            //if(c == 0 && d == 0) continue;
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
        //if(fscanf(chg_fp,"%d %d %d",&chg_start, &chg_end,&chg_cost) == EOF) break;
    //}   
    

    fclose(top_fp);
    fclose(msg_fp);
    fclose(chg_fp);
    fclose(out_fp);
    return 0;
}

void print_rout_table()
{
    for(int i = 0;i<node_num;i++){
        for(int j = 0;j < node_num; j++){
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
        fprintf(out_fp,"from %d to %d cost %d hops ",start,end,rout_table[start][end][1]);
        
        for(j = start;;){
            fprintf(out_fp,"%d ",j);
            j = rout_table[j][end][0];
            if(j == end) break;
        }
        fprintf(out_fp,"%s\n",msg_s[i].c_str());
    }
}