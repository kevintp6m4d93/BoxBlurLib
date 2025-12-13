#include "pch.h"
#include "CppUnitTest.h"
#include "BoxBlur.h"
#include "ImageBuffer.h"
#include <vector>
#include <opencv2/opencv.hpp>
#include <chrono>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace BoxBlurUnitTest
{
    TEST_CLASS(BoxBlurUnitTest)
    {
    public:
        TEST_METHOD(BoxBlur_CompareWithOpenCV)
        {
            std::string imageName = "C:/Users/KevinYK_Chen/Desktop/CPP_repo/BoxBlurLib/BoxBlurUnitTest/test_image.jpg";
            cv::Mat src = cv::imread(imageName, cv::IMREAD_COLOR);

            Assert::IsFalse(src.empty(), L"Failed to load test image");

            int width = src.cols;
            int height = src.rows;
            int numChannel = src.channels();
            int kernelSize = 51;
			ImageCore::ImageBuffer srcBuffer(src);
            ImageCore::ImageBuffer dstBuffer(width, height, ImageCore::PixelFormat::RGB);

            Blur::BoxBlur boxBlur(Blur::PaddingMode::Mirror, 4);
            
            auto start = std::chrono::high_resolution_clock::now();
            boxBlur.Apply(srcBuffer, dstBuffer, kernelSize);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            std::wstring timeMessage = L"BoxBlur execution time: " + std::to_wstring(duration.count()) + L" ms";
            Logger::WriteMessage(timeMessage.c_str());

            cv::Mat dstMat(height,
                width,
                dstBuffer.GetNumChannels() == 3 ? CV_8UC3 : CV_8UC4,
                dstBuffer.GetBufferPtr(),
                dstBuffer.GetStride());

            cv::Mat gtMat;
            cv::blur(src,
                gtMat,
                cv::Size(kernelSize, kernelSize),
                cv::Point(-1, -1),
                cv::BORDER_REFLECT);

            Assert::AreEqual(gtMat.rows, dstMat.rows);
            Assert::AreEqual(gtMat.cols, dstMat.cols);
            Assert::AreEqual(gtMat.channels(), dstMat.channels());

            const int total = width * height * numChannel;
            const uint8_t* gtMatData = gtMat.data;
            const uint8_t* dstMatData = dstMat.data;

            for (int i = 0; i < total; ++i)
            {
                int diff = std::abs(int(gtMatData[i]) - int(dstMatData[i]));
                Assert::IsTrue(
                    diff <= 1,
                    (L"Pixel difference too large at index " + std::to_wstring(i)).c_str()
                );
            }

            auto dotPos = imageName.find_last_of('.');
            std::string outName = (dotPos == std::string::npos)
                ? imageName + "_blurred.jpg"
                : imageName.substr(0, dotPos) + "_blurred.jpg";

            cv::imwrite(outName, dstMat);
        }
    };
}
