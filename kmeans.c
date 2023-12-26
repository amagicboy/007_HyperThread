#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N   10000 // 增加数据点数量
#define k   5

typedef struct {
    float x;
    float y;
} Point;

Point point[N];
Point mean[k];
int center[N];

float getdistance(Point point1, Point point2);
void cluster();
float gete();
void getmean(int center[N]);

int main()
{
    int number = 0;
    float temp1, temp2;

    // 随机生成数据点
    for (int i = 0; i < N; i++) {
        point[i].x = (float)(rand() % 100) / 10.0;
        point[i].y = (float)(rand() % 100) / 10.0;
    }

    // 随机生成初始中心点
    for (int i = 0; i < k; i++) {
        mean[i].x = (float)(rand() % 100) / 10.0;
        mean[i].y = (float)(rand() % 100) / 10.0;
    }

    // 进行聚类和迭代
    while (number < 100) {
        cluster();
        number++;
        temp1 = gete();
        getmean(center);
        cluster();
        number++;
        temp2 = gete();

        if (fabs(temp1 - temp2) <= 0.1) {
            break;
        }
    }

    printf("the total number of clusters is: %d\n", number);

    return 0;
}


//计算距离函数，欧式距离
float getdistance(Point point1, Point point2)
{
    float d;

    d = sqrt((point1.x - point2.x)*(point1.x - point2.x) + (point1.y - point2.y)*(point1.y - point2.y));

    return d;
}

//聚类函数
void cluster()
{
    float distance[N][k];
    float min;
    int i, j;

    for(i=0; i<N; i++)
    {
        for(j=0; j<k; j++)
        {
            distance[i][j] = getdistance(point[i], mean[j]);
        }

        min = 9999.0;

        for(j=0; j<k; j++)
        {
            if(distance[i][j] < min)
            {
                min = distance[i][j];
                center[i] = j;
            }
        }

        printf("(%.0f,%.0f)\t in cluster-%d\n", point[i].x, point[i].y, center[i] + 1);
    }
}

//聚类后误差计算函数
float gete()
{
    float cnt=0, sum=0;
    int i, j;

    for(i=0; i<N; i++)
    {
        for(j=0; j<k; j++)
        {
            if(center[i] == j)
            {
                cnt = getdistance(point[i], mean[j]);
            }
        }

        sum += cnt;     
    }

    return sum;
}

//重新计算聚类中心
void getmean(int center[N])
{
    Point sum;
    int count;
    int i, j;

    for(i=0; i<k; i++)
    {
        sum.x = 0.0;
        sum.y = 0.0;
        count = 0;

        for(j=0; j<N; j++)
        {
            if(center[j] == i)
            {
                sum.x += point[j].x;
                sum.y += point[j].y;
                count++;
            }
        }

        mean[i].x = sum.x / count;
        mean[i].y = sum.y / count;
    }

    for(i=0; i<k; i++)
    {
        printf("the new center point of %d is:\t(%f,%f)\n", i + 1, mean[i].x, mean[i].y);
    }
}