#include "Yolo_origin.h"

const char* keys =
"{ help  h     | | Print help message. }"
"{ device      |  0 | camera device number. }"
"{ input i     | | Path to input image or video file. Skip this argument to capture frames from a camera. }"
"{ model m     | | Path to a binary file of model contains trained weights. "
"It could be a file with extensions .caffemodel (Caffe), "
".pb (TensorFlow), .t7 or .net (Torch), .weights (Darknet).}"
"{ config c    | | Path to a text file of model contains network configuration. "
"It could be a file with extensions .prototxt (Caffe), .pbtxt (TensorFlow), .cfg (Darknet).}"
"{ framework f | | Optional name of an origin framework of the model. Detect it automatically if it does not set. }"
"{ classes     | | Optional path to a text file with names of classes to label detected objects. }"
"{ mean        | | Preprocess input image by subtracting mean values. Mean values should be in BGR order and delimited by spaces. }"
"{ scale       |  1 | Preprocess input image by multiplying on a scale factor. }"
"{ width       | -1 | Preprocess input image by resizing to a specific width. }"
"{ height      | -1 | Preprocess input image by resizing to a specific height. }"
"{ rgb         |    | Indicate that model works with RGB input images instead BGR ones. }"
"{ thr         | .5 | Confidence threshold. }"
"{ nms         | .4 | Non-maximum suppression threshold. }"
"{ backend     |  0 | Choose one of computation backends: "
"0: automatically (by default), "
"1: Halide language (http://halide-lang.org/), "
"2: Intel's Deep Learning Inference Engine (https://software.intel.com/openvino-toolkit), "
"3: OpenCV implementation }"
"{ target      | 0 | Choose one of target computation devices: "
"0: CPU target (by default), "
"1: OpenCL, "
"2: OpenCL fp16 (half-float precision), "
"3: VPU }";

static float gConfThreshold, gNmsThreshold;
static float gScale;
static cv::Scalar gMean;
static bool gSwapRB;
static int gInpWidth;
static int gInpHeight;
static std::vector<std::string> gClasses;
static cv::dnn::Net gNet;
bool Yolo_origin::ready(int argc,const char* cmd[])
{
	cv::CommandLineParser parser(argc, cmd, keys);
	parser.about("Use this script to run object detection deep learning networks using OpenCV.");
	if (argc == 1 || parser.has("help"))
	{
		parser.printMessage();
		return false;
	}

	gConfThreshold = parser.get<float>("thr");
	gNmsThreshold = parser.get<float>("nms");
	gScale = parser.get<float>("scale");
	gMean = parser.get<cv::Scalar>("mean");
	gSwapRB = parser.get<bool>("rgb");
	gInpWidth = parser.get<int>("width");
	gInpHeight = parser.get<int>("height");

	// Open file with classes names.
	if (parser.has("classes"))
	{
		std::string file = parser.get<cv::String>("classes");
		std::ifstream ifs(file.c_str());
		if (!ifs.is_open())
			CV_Error(cv::Error::StsError, "File " + file + " not found");
		std::string line;
		while (std::getline(ifs, line))
		{
			gClasses.push_back(line);
		}
	}

	// Load a model.
	CV_Assert(parser.has("model"));
	gNet = cv::dnn::readNet(parser.get<cv::String>("model"), parser.get<cv::String>("config"), parser.get<cv::String>("framework"));
	gNet.setPreferableBackend(parser.get<int>("backend"));
	gNet.setPreferableTarget(parser.get<int>("target"));
	return true;
}

Yolo_Result Yolo_origin::YOLO(cv::Mat & input)
{	
	// Create a 4D blob from a input.
	cv::Mat blob;
	cv::Size inpSize(gInpWidth > 0 ? gInpWidth : input.cols,
		gInpHeight > 0 ? gInpHeight : input.rows);
	cv::dnn::blobFromImage(input, blob, gScale, inpSize, gMean, gSwapRB, false);

	// Run a model.
	gNet.setInput(blob);
	if (gNet.getLayer(0)->outputNameToIndex("im_info") != -1)  // Faster-RCNN or R-FCN
	{
		resize(input, input, inpSize);
		cv::Mat imInfo = (cv::Mat_<float>(1, 3) << inpSize.height, inpSize.width, 1.6f);
		gNet.setInput(imInfo, "im_info");
	}
	std::vector<cv::Mat> outs;
	gNet.forward(outs, getOutputsNames(gNet));

	return postprocess(input, outs, gNet);

	// Put efficiency information.
	/*std::vector<double> layersTimes;
	double freq = cv::getTickFrequency() / 1000;
	double t = gNet.getPerfProfile(layersTimes) / freq;
	std::string label = cv::format("Inference time: %.2f ms", t);
	putText(input, label, cv::Point(0, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0));*/
}

std::vector<std::string> Yolo_origin::GetClasses()
{
	return gClasses;
}

Yolo_Result Yolo_origin::postprocess(cv::Mat& frame, const std::vector<cv::Mat>& outs, cv::dnn::Net& net)
{
	static std::vector<int> outLayers = net.getUnconnectedOutLayers();
	static std::string outLayerType = net.getLayer(outLayers[0])->type;

	std::vector<int> classIds;
	std::vector<float> confidences;
	std::vector<cv::Rect> boxes;
	if (net.getLayer(0)->outputNameToIndex("im_info") != -1)  // Faster-RCNN or R-FCN
	{
		// Network produces output blob with a shape 1x1xNx7 where N is a number of
		// detections and an every detection is a vector of values
		// [batchId, classId, confidence, left, top, right, bottom]
		CV_Assert(outs.size() == 1);
		float* data = (float*)outs[0].data;
		for (size_t i = 0; i < outs[0].total(); i += 7)
		{
			float confidence = data[i + 2];
			if (confidence > gConfThreshold)
			{
				int left = (int)data[i + 3];
				int top = (int)data[i + 4];
				int right = (int)data[i + 5];
				int bottom = (int)data[i + 6];
				int width = right - left + 1;
				int height = bottom - top + 1;
				classIds.push_back((int)(data[i + 1]) - 1);  // Skip 0th background class id.
				boxes.push_back(cv::Rect(left, top, width, height));
				confidences.push_back(confidence);
			}
		}
	}
	else if (outLayerType == "DetectionOutput")
	{
		// Network produces output blob with a shape 1x1xNx7 where N is a number of
		// detections and an every detection is a vector of values
		// [batchId, classId, confidence, left, top, right, bottom]
		CV_Assert(outs.size() == 1);
		float* data = (float*)outs[0].data;
		for (size_t i = 0; i < outs[0].total(); i += 7)
		{
			float confidence = data[i + 2];
			if (confidence > gConfThreshold)
			{
				int left = (int)(data[i + 3] * frame.cols);
				int top = (int)(data[i + 4] * frame.rows);
				int right = (int)(data[i + 5] * frame.cols);
				int bottom = (int)(data[i + 6] * frame.rows);
				int width = right - left + 1;
				int height = bottom - top + 1;
				classIds.push_back((int)(data[i + 1]) - 1);  // Skip 0th background class id.
				boxes.push_back(cv::Rect(left, top, width, height));
				confidences.push_back(confidence);
			}
		}
	}
	else if (outLayerType == "Region")
	{
		for (size_t i = 0; i < outs.size(); ++i)
		{
			// Network produces output blob with a shape NxC where N is a number of
			// detected objects and C is a number of classes + 4 where the first 4
			// numbers are [center_x, center_y, width, height]
			float* data = (float*)outs[i].data;
			for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
			{
				cv::Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
				cv::Point classIdPoint;
				double confidence;
				minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);

				if (confidence > gConfThreshold)
				{
					int centerX = (int)(data[0] * frame.cols);
					int centerY = (int)(data[1] * frame.rows);
					int width = (int)(data[2] * frame.cols);
					int height = (int)(data[3] * frame.rows);
					int left = centerX - width / 2;
					int top = centerY - height / 2;

					classIds.push_back(classIdPoint.x);
					confidences.push_back((float)confidence);
					boxes.push_back(cv::Rect(left, top, width, height));
				}
			}
		}
	}
	else
		CV_Error(cv::Error::StsNotImplemented, "Unknown output layer type: " + outLayerType);

	std::vector<int> indices;
	cv::dnn::NMSBoxes(boxes, confidences, gConfThreshold, gNmsThreshold, indices);
	Yolo_Result result;
	result.indices = indices;
	result.boxes = boxes;
	result.confidences = confidences;
	result.classIds = classIds;
	return result;
	
}

std::vector<cv::String> Yolo_origin::getOutputsNames(const cv::dnn::Net & net)
{
	static std::vector<cv::String> names;
	if (names.empty())
	{
		std::vector<int> outLayers = net.getUnconnectedOutLayers();
		std::vector<cv::String> layersNames = net.getLayerNames();
		names.resize(outLayers.size());
		for (size_t i = 0; i < outLayers.size(); ++i)
			names[i] = layersNames[outLayers[i] - 1];
	}
	return names;
}

