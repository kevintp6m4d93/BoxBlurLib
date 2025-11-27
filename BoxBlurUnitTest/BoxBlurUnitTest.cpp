#include "pch.h"
#include "CppUnitTest.h"
#include "BoxBlur.h"
#include <vector>
#include <opencv2/opencv.hpp>

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

            Assert::IsFalse(src.empty(), L"Failed to load test image (C:/Users/KevinYK_Chen/Desktop/CPP_repo/BoxBlurLib/BoxBlurUnitTest/test_image.jpg)");

            int width = src.cols;
            int height = src.rows;
            int numChannel = src.channels();
            int kernelSize = 5;

            cv::Mat dstBox(src.size(), src.type());

            Blur::BoxBlur boxBlur(Blur::PaddingMode::Replicate);

            boxBlur.Apply(src.data,
                dstBox.data,
                width,
                height,
                numChannel,
                kernelSize);

            cv::Mat dstRef;
            cv::blur(src,
                dstRef,
                cv::Size(kernelSize, kernelSize),
                cv::Point(-1, -1),
                cv::BORDER_REPLICATE);

            Assert::AreEqual(dstRef.rows, dstBox.rows);
            Assert::AreEqual(dstRef.cols, dstBox.cols);
            Assert::AreEqual(dstRef.channels(), dstBox.channels());

            const int total = width * height * numChannel;
            const uint8_t* pRef = dstRef.data;
            const uint8_t* pBox = dstBox.data;

            for (int i = 0; i < total; ++i)
            {
                int diff = std::abs(int(pRef[i]) - int(pBox[i]));
                Assert::IsTrue(
                    diff <= 1,
                    (L"Pixel difference too large at index " + std::to_wstring(i)).c_str()
                );
            }

            auto dotPos = imageName.find_last_of('.');
            std::string outName = (dotPos == std::string::npos)
                ? imageName + "_blurred.jpg"
                : imageName.substr(0, dotPos) + "_blurred.jpg";

            cv::imwrite(outName, dstBox);
        }
    };
}
