#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <stdio.h>
using namespace cv;

// ʵ�ּ����ľ��δ��ڵķǼ���ֵ����
// numBoxes��������Ŀ
// points���������Ͻ������
// oppositePoints���������½������
// score�����ڵ÷�
// overlapThreshold���ص���ֵ����
// numBoxesOut�����������Ŀ
// pointsOut������������Ͻ������
// oppositePoints������������½������
// scoreOut��������ڵ÷� 
int nonMaximumSuppression(int numBoxes, const CvPoint *points,
	const CvPoint *oppositePoints, const float *score,
	float overlapThreshold,
	int *numBoxesOut, CvPoint **pointsOut,
	CvPoint **oppositePointsOut, float **scoreOut)
{
	int i, j, index;
	float* box_area = (float*)malloc(numBoxes * sizeof(float));   // ���崰���������������ռ� 
	int* indices = (int*)malloc(numBoxes * sizeof(int));          // ���崰������������ռ� 
	int* is_suppressed = (int*)malloc(numBoxes * sizeof(int));    // �����Ƿ����Ʊ��־������ռ� 
	// ��ʼ��indices��is_supperssed��box_area��Ϣ 
	for (i = 0; i < numBoxes; i++)
	{
		indices[i] = i;
		is_suppressed[i] = 0;
		box_area[i] = (float)( (oppositePoints[i].x - points[i].x + 1) *
			(oppositePoints[i].y - points[i].y + 1));
	}
	// �����봰�ڰ��շ�����ֵ�������������ı�ŷ���indices�� 
	sort(numBoxes, score, indices);
	for (i = 0; i < numBoxes; i++)                // ѭ�����д��� 
	{
		if (!is_suppressed[indices[i]])           // �жϴ����Ƿ����� 
		{
			for (j = i + 1; j < numBoxes; j++)    // ѭ����ǰ����֮��Ĵ��� 
			{
				if (!is_suppressed[indices[j]])   // �жϴ����Ƿ����� 
				{
					int x1max = max(points[indices[i]].x, points[indices[j]].x);                     // �������������Ͻ�x�������ֵ 
					int x2min = min(oppositePoints[indices[i]].x, oppositePoints[indices[j]].x);     // �������������½�x������Сֵ 
					int y1max = max(points[indices[i]].y, points[indices[j]].y);                     // �������������Ͻ�y�������ֵ 
					int y2min = min(oppositePoints[indices[i]].y, oppositePoints[indices[j]].y);     // �������������½�y������Сֵ 
					int overlapWidth = x2min - x1max + 1;            // �����������ص��Ŀ�� 
					int overlapHeight = y2min - y1max + 1;           // �����������ص��ĸ߶� 
					if (overlapWidth > 0 && overlapHeight > 0)
					{
						float overlapPart = (overlapWidth * overlapHeight) / box_area[indices[j]];    // �����ص��ı��� 
						if (overlapPart > overlapThreshold)          // �ж��ص������Ƿ񳬹��ص���ֵ 
						{
							is_suppressed[indices[j]] = 1;           // ������j���Ϊ���� 
						}
					}
				}
			}
		}
	}

	*numBoxesOut = 0;    // ��ʼ�����������Ŀ0 
	for (i = 0; i < numBoxes; i++)
	{
		if (!is_suppressed[i]) (*numBoxesOut)++;    // ͳ�����������Ŀ 
	}

	*pointsOut = (CvPoint *)malloc((*numBoxesOut) * sizeof(CvPoint));           // ��������������Ͻ�����ռ� 
	*oppositePointsOut = (CvPoint *)malloc((*numBoxesOut) * sizeof(CvPoint));   // ��������������½�����ռ� 
	*scoreOut = (float *)malloc((*numBoxesOut) * sizeof(float));                // ����������ڵ÷ֿռ� 
	index = 0;
	for (i = 0; i < numBoxes; i++)                  // �����������봰�� 
	{
		if (!is_suppressed[indices[i]])             // ��δ�������ƵĴ�����Ϣ���浽�����Ϣ�� 
		{
			(*pointsOut)[index].x = points[indices[i]].x;
			(*pointsOut)[index].y = points[indices[i]].y;
			(*oppositePointsOut)[index].x = oppositePoints[indices[i]].x;
			(*oppositePointsOut)[index].y = oppositePoints[indices[i]].y;
			(*scoreOut)[index] = score[indices[i]];
			index++;
		}

	}

	free(indices);          // �ͷ�indices�ռ� 
	free(box_area);         // �ͷ�box_area�ռ� 
	free(is_suppressed);    // �ͷ�is_suppressed�ռ� 

	return 1;
}

// ���������������н�������indices�е�����
// n����������
// x����������
// indices����ʼΪ0~n-1��Ŀ 
static void sort(int n, const float* x, int* indices)
{
	int i, j;
	for (i = 0; i < n; i++)
		for (j = i + 1; j < n; j++)
		{
			if (x[indices[j]] > x[indices[i]])
			{
				//float x_tmp = x[i];
				int index_tmp = indices[i];
				//x[i] = x[j];
				indices[i] = indices[j];
				//x[j] = x_tmp;
				indices[j] = index_tmp;
			}
		}
}