JsImgProc
=========

Jethro's Image Processing Software.Super-Resolution Reconstruction is the most import feature in this software. It’s achieved by the algorithm Single-Image Super Resolution, based on theory of Compressed Sensing. This algorithm uses OMP (Orthogonal Matching Pursuit) to compute the sparse coding of images, K-SVD (K-means Singular Value Decomposition) to learn over-complete dictionary. With the collaboration between OMP and over-complete dictionary, low resolution image can be scaled-up and its high frequency information (texture, edge) can be recovered at some level. The result is better than simple interpolation, eg, linear interpolation, bi-cubic interpolation, etc. This algorithm is implemented using C++&amp;OpenCV. The other features of this software are also implemented using C++&amp;OpenCV. The GUI is designed using QT.