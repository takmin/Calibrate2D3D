#include <iostream>
#include <opencv2/calib3d.hpp>


bool readIntCameraParameters(const cv::FileNode& cvfn, 
	cv::Mat& camera_matrix, cv::Mat& distortion, cv::Size& image_size, bool& fisheye)
{
	try {
		cvfn["camera_matrix"] >> camera_matrix;
		cvfn["distortion"] >> distortion;
		cvfn["image_size"] >> image_size;
		int f;
		cvfn["fisheye"] >> f;
		fisheye = f > 0;
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		return false;
	}
	return true;
}


bool loadIntCameraParameters(const std::string& filename,
	cv::Mat& camera_matrix, cv::Mat& distortion, cv::Size& image_size, bool& fisheye)
{
	cv::FileStorage cvfs(filename, cv::FileStorage::READ);
	if (!cvfs.isOpened())
		return false;
	return readIntCameraParameters(cvfs["IntParam"], camera_matrix, distortion, image_size, fisheye);
}

void load_point_pairs(const std::string& filename,
	std::vector<cv::Point2f>& point2d, std::vector<cv::Point3f>& point3d)
{
	cv::FileStorage fs(filename, cv::FileStorage::READ);
	cv::FileNode fn2d = fs["point2d"];
	for (cv::FileNodeIterator it = fn2d.begin();
		it != fn2d.end(); it++) {
		cv::Point2f pt;
		*it >> pt;
		point2d.push_back(pt);
	}

	cv::FileNode fn3d = fs["point3d"];
	for (cv::FileNodeIterator it = fn3d.begin();
		it != fn3d.end(); it++) {
		cv::Point3f pt;
		*it >> pt;
		point3d.push_back(pt);
	}
}


bool save_ext_param(const std::string& filename, const cv::Mat& R, const cv::Mat& T)
{
	cv::FileStorage fs(filename, cv::FileStorage::WRITE);
	if (!fs.isOpened())
		return false;

	fs << "ExtParam" << "{";
	fs << "rvec" << R;
	fs << "tvec" << T;
	fs << "}";

	return true;
}


int main(int argc, char * argv[])
{
	cv::String keys =
		"{help h usage |      | print this message  }"
		"{K param      |      | internal camera parameter file  }"
		"{P points     |      | 2D-3D coordinates pair file  }"
		"{O output     |      | external parameter file to save  }"
		;

	cv::CommandLineParser parser(argc, argv, keys);
	parser.about("Compute external parameters from 2D-3D matching pair.");

	if (parser.has("h")) {
		parser.printMessage();
		return 0;
	}

	cv::String int_param_file = parser.get<cv::String>("K");
	cv::String points2D3D_file = parser.get<cv::String>("P");
	cv::String output_file = parser.get<cv::String>("O");

	cv::Mat camera_matrix;
	cv::Mat distortion;
	cv::Size image_size;
	bool fisheye;
	loadIntCameraParameters(int_param_file, camera_matrix, distortion, image_size, fisheye);
	if (fisheye) {
		std::cerr << "This program does not support fisheye camera." << std::endl;
		return -1;
	}

	std::vector<cv::Point2f> points2d;
	std::vector<cv::Point3f> points3d;
	load_point_pairs(points2D3D_file, points2d, points3d);
	cv::Mat rvec, tvec;
	cv::solvePnP(points3d, points2d, camera_matrix, distortion, rvec, tvec);
	if (save_ext_param(output_file, rvec, tvec)) {
		std::cout << "save " << output_file << std::endl;
	}
	else {
		std::cerr << "Fail to save " << output_file << std::endl;
		return -1;
	}

	if (!parser.check()) {
		parser.printErrors();
		return -1;
	}
	return 0;
}