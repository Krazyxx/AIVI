#include "blockmatching.hpp"

#include <opencv2/imgproc/imgproc.hpp>

#include "utils.hpp"
#include "limits.h"
#include <iostream>


//We want to traverse all blocks from the m1 image.
//For each block b1 in m1, we search the block b2 in m2 image
//such as computeMSE(b1, b2) is minimal.

//To call computeMSE we will need to create a cv::Mat corresponding to each block.
//You can for example use the rowRange()/colRange() methods of cv::Mat.

//Suppose b1 is at position (x1, y1)
// and the best b2 (that is the block that minimizes computeMSE(b1, b2)) is at (x2Best, y2Best)
// then the motion vector is (x2Best-x1, y2Best-y1).
//We can fill the motionVectors matrix with:
// motionVectors.at<cv::Vec2i>(i, j) = cv::Vec2i(x2Best-x1, y2Best-y1);

//Helper function for blockMatchingMulti
//Here motion vectors are used both as an input and an output.
//If they are non empty, then they are used to initialize block b2 search window position.

// m1 = frameY, m2 = prevY
// frameY = prevY + motionVectors;
void blockMatchingMono(const cv::Mat &m1, const cv::Mat &m2,
			int blockSize,
			int windowSize,
			cv::Mat &motionVectors)
{
	assert(m1.size() == m2.size()
	    && m1.type() == m2.type()
	    && m1.type() == CV_8UC1);
    assert(blockSize > 0);
	assert(windowSize > 0);

	//Here we suppose that m1.cols & m1.rows are multiple of blockSize
	const int blockXCount = m1.cols / blockSize;
	const int blockYCount = m1.rows / blockSize;

	bool first = motionVectors.empty();
	motionVectors.create(blockYCount, blockXCount, CV_32SC2);

	int windowHalfSize = windowSize/2;
	for (int y = 0; y < blockYCount; y++) {
		for (int x = 0; x < blockXCount; x++) {
			double minMSE = INT_MAX;
			int b1_x = x * blockSize, b1_y = y * blockSize;
			cv::Mat b1(m1, cv::Rect(b1_x, b1_y, blockSize, blockSize));

			int best_b2_x = - windowHalfSize, best_b2_y = - windowHalfSize;
			for (int dy = - windowHalfSize; dy <= windowHalfSize; dy++) {
				int b2_y = b1_y + dy;
				if (!first) { b2_y += motionVectors.at<cv::Vec2i>(y, x)[1]; }
				if (b2_y < 0 || b2_y + blockSize >= m2.rows) { continue; }

				for (int dx = - windowHalfSize; dx <= windowHalfSize; dx++) {
					int b2_x = b1_x + dx;
					if (!first) { b2_x += motionVectors.at<cv::Vec2i>(y, x)[0]; }
					if (b2_x < 0 || b2_x + blockSize >= m2.cols) { continue; }

					cv::Mat b2(m2, cv::Rect(b2_x, b2_y, blockSize, blockSize));
					double MSE = computeMSE(b1, b2);

					if (minMSE > MSE) {
						minMSE = MSE;
						best_b2_x = b2_x;
						best_b2_y = b2_y;
					}
				}
			}

			motionVectors.at<cv::Vec2i>(y, x) = cv::Vec2i(best_b2_x - b1_x, best_b2_y - b1_y);
		}
	}
}


void up(const cv::Mat &m, cv::Mat &m2)
{
  assert(m.type() == CV_32SC2);

  m2 = 2 * m;
}


void blockMatchingMulti(const cv::Mat &m1, const cv::Mat &m2,
			int blockSize,
			int windowSize,
			int nbLevels,
			std::vector<cv::Mat> &levels1,
			std::vector<cv::Mat> &levels2,
			std::vector<cv::Mat> &motionVectors)
{
  assert(m1.size() == m2.size()
	 && m1.type() == m2.type()
	 && m1.type() == CV_8UC1);
  assert(blockSize > 0);
  assert(windowSize > 0);
  assert(nbLevels > 0);

  motionVectors.resize(nbLevels);
  levels1.resize(nbLevels);
  levels2.resize(nbLevels);

  std::vector<int> blockSizes(nbLevels);
  levels1[0] = m1;
  levels2[0] = m2;
  blockSizes[0] = blockSize;
  for (int i=1; i<nbLevels; ++i) { //start from 1
    cv::pyrDown(levels1[i-1], levels1[i], cv::Size(levels1[i-1].cols/2, levels1[i-1].rows/2));
    cv::pyrDown(levels2[i-1], levels2[i], cv::Size(levels2[i-1].cols/2, levels2[i-1].rows/2));
    blockSizes[i] = blockSizes[i-1]/2;
  }

  for (int i = nbLevels-1; i >= 0; i--) {
    blockMatchingMono(levels1[i], levels2[i], blockSizes[i], windowSize, motionVectors[i]);
    if (i > 0) { up(motionVectors[i], motionVectors[i-1]); }
  }
}
