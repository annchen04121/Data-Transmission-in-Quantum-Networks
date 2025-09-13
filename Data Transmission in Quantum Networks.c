#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAXN 100 // 最大節點數

// 計算初始fidelity，l為距離，beta為參數
double link_fidelity(double l, double beta) {
    return 0.5 + 0.5 * exp(-beta * l);
}

// 純化後fidelity，F為原始的fidelity，n為該hop entangled links的數量
double purify_fidelity(double F, int n) {
    double f = F;
    for (int i = 1; i < n; i++)
        f = (f * f) / (f * f + (1 - f) * (1 - f)); // 因為公式：F1=F2
    return f;
}

int main() {
    int N; // 節點數
    double alpha, beta, swapProb; // 傳輸衰減參數與swapping機率
    int nodeMem[MAXN];      // 每個節點的量子記憶體數
    double linkDist[MAXN];  // 各鏈路的距離
    int linkNum[MAXN];      // 每條link分配的 entangled links 數量
    int nodeUsage[MAXN];    // 每個節點目前用掉幾條記憶體

    // 讀取第一行：節點數、alpha、beta、swapProb
    printf("請依序輸入節點數 alpha beta swapProb\n""接者依序輸入節點編號 記憶體數\n""最後再輸入link編號 距離\n");
    scanf("%d %lf %lf %lf", &N, &alpha, &beta, &swapProb);

    // 讀取每個節點的記憶體數量
    for (int i = 0; i < N; i++) {
        int id, mem;
        scanf("%d %d", &id, &mem); // 節點編號、記憶體數
        nodeMem[i] = mem;
        nodeUsage[i] = 0; // 初始化已分配記憶體為0
    }

    // 讀取每條link（N-1條）：link編號、距離
    for (int i = 0; i < N-1; i++) {
        int id;
        double dist;
        scanf("%d %lf", &id, &dist); // link編號、距離
        linkDist[i] = dist;
        linkNum[i] = 1; // 初始每條分配1條entangled link
        nodeUsage[i]++;      // 兩端節點各占用一格記憶體
        nodeUsage[i+1]++;
    }

    // Greedy策略：只要還有剩餘記憶體，就分配給提升fidelity最多的link
    while (1) {
        double bestGain = 0.0; // 目前最大提升
        int bestHop = -1;      // 最佳link編號
        for (int i = 0; i < N-1; i++) {
            // 檢查此鏈路兩端節點記憶體是否還有空間
            if (nodeUsage[i] < nodeMem[i] && nodeUsage[i+1] < nodeMem[i+1]) {
                double F = link_fidelity(linkDist[i], beta);    // 此link的原始fidelity
                int nlink = linkNum[i];                         // 現在的link數
                double f_cur = purify_fidelity(F, nlink);       // 純化後fidelity
                double f_new = purify_fidelity(F, nlink+1);     // 多分配1條link後的fidelity
                double gain = f_new - f_cur;                    // 增益
                if (gain > bestGain) {
                    bestGain = gain;
                    bestHop = i;
                }
            }
        }
        // 若所有link都無法再分配記憶體，或增益極小(<1e-8)則停止
        if (bestHop == -1 || bestGain < 1e-8) break;
        // 對最佳link多分配一條entangled link
        linkNum[bestHop]++;
        nodeUsage[bestHop]++;
        nodeUsage[bestHop+1]++;
    }

    // 輸出每條link的分配結果
    printf("=== 每條link的分配結果如下 ===\n");
    for (int i = 0; i < N-1; i++) {
        printf("%d %d %d\n", i, i+1, linkNum[i]);
    }

    return 0;
}