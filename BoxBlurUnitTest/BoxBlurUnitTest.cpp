#include "pch.h"
#include "CppUnitTest.h"
#include "BoxBlur.h"
#include "GaussianBlur.h"
#include "MedianBlur.h"
#include "OpenCVAdapter.h"
#include "ImageBuffer.h"
#include <vector>
#include <opencv2/opencv.hpp>
#include <chrono>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

long long RunAndMeasure(
    BoxBlur& blur,
    const ImageCore::ImageBuffer& src,
    ImageCore::ImageBuffer& dst,
    int kernelSize)
{
    auto start = std::chrono::high_resolution_clock::now();
    blur.Apply(src, dst, kernelSize);
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

long long TrimmedMean(std::vector<long long>& times)
{
    assert(times.size() >= 5);
    std::sort(times.begin(), times.end());

    long long sum = 0;
    for (size_t i = 1; i < times.size() - 1; ++i) {
        sum += times[i];
    }
    return sum / static_cast<long long>(times.size() - 2);
}

namespace BoxBlurUnitTest
{
    TEST_CLASS(BoxBlurUnitTest)
    {
    public:
        TEST_METHOD(Test_BoxBlur_Performace)
        {
            std::string imageName = "C:/Users/KevinYK_Chen/Desktop/CPP_repo/BoxBlurLib/BoxBlurUnitTest/4k_image.jpg";
            cv::Mat src = cv::imread(imageName, cv::IMREAD_COLOR);
            Assert::IsFalse(src.empty(), L"Failed to load test image");

            int width = src.cols;
            int height = src.rows;

            int kernelSizeList[] = { 3 };
            int numMultiThreadList[] = { 1, 2, 4, 6, 8, 10 };
            constexpr int kRunCount = 1;

            ImageCore::ImageBuffer srcBuffer(src);
            ImageCore::ImageBuffer dstBuffer(width, height, ImageCore::PixelFormat::BGR);

            BoxBlur naiveBoxBlur(0, true);

            for (int kernelSize : kernelSizeList)
            {
                {
                    std::ostringstream oss;
                    oss << "\nStart testing performance of kernel size = " << kernelSize;
                    Logger::WriteMessage(oss.str().c_str());
                }

                // ===== Naive =====
                std::vector<long long> naiveTimes;
                for (int i = 0; i < kRunCount; ++i) {
                    naiveTimes.push_back(
                        RunAndMeasure(naiveBoxBlur, srcBuffer, dstBuffer, kernelSize)
                    );
                }

                long long naiveAvg = TrimmedMean(naiveTimes);
                {
                    std::wstring msg = L"Naive BoxBlur execution time (trimmed mean, 5 runs): "
                        + std::to_wstring(naiveAvg) + L" ms";
                    Logger::WriteMessage(msg.c_str());
                }

                // ===== Optimized =====
                for (int numThreads : numMultiThreadList)
                {
                    BoxBlur optimizedBoxBlur(numThreads);

                    std::vector<long long> optTimes;
                    for (int i = 0; i < kRunCount; ++i) {
                        optTimes.push_back(
                            RunAndMeasure(optimizedBoxBlur, srcBuffer, dstBuffer, kernelSize)
                        );
                    }

                    long long optAvg = TrimmedMean(optTimes);
                    std::wstring msg =
                        L"Optimized BoxBlur execution time "
                        L"(kernel size " + std::to_wstring(kernelSize) +
                        L", threads " + std::to_wstring(numThreads) +
                        L", trimmed mean, 5 runs): " +
                        std::to_wstring(optAvg) + L" ms";

                    Logger::WriteMessage(msg.c_str());
                }
            }
        }

        TEST_METHOD(BoxBlur_CompareWithOpenCV)
        {
            std::string imageName = "C:/Users/KevinYK_Chen/Desktop/CPP_repo/BoxBlurLib/BoxBlurUnitTest/4k_image.jpg";
            cv::Mat src = cv::imread(imageName, cv::IMREAD_COLOR);

            Assert::IsFalse(src.empty(), L"Failed to load test image");

            int width = src.cols;
            int height = src.rows;
            int numChannel = src.channels();
            int kernelSize = 3;
            ImageCore::ImageBuffer srcBuffer(src);
            ImageCore::ImageBuffer optimizedDstBuffer(width, height, ImageCore::PixelFormat::BGR);
            BoxBlur optimizedBoxBlur(8);

            auto start = std::chrono::high_resolution_clock::now();
            optimizedBoxBlur.Apply(srcBuffer, optimizedDstBuffer, kernelSize);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            std::wstring timeMessage = L"Optimized BoxBlur execution time: " + std::to_wstring(duration.count()) + L" ms";
            Logger::WriteMessage(timeMessage.c_str());

            BoxBlur naiveBoxBlur(0, true);
            ImageCore::ImageBuffer naiveDstBuffer(width, height, ImageCore::PixelFormat::BGR);
            start = std::chrono::high_resolution_clock::now();
            naiveBoxBlur.Apply(srcBuffer, naiveDstBuffer, kernelSize);
            end = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            timeMessage = L"Naive BoxBlur execution time: " + std::to_wstring(duration.count()) + L" ms";
            Logger::WriteMessage(timeMessage.c_str());

            const int total = width * height * numChannel;
            const uint8_t* gtPtr = naiveDstBuffer.GetBufferPtr();
            const uint8_t* dstPtr = optimizedDstBuffer.GetBufferPtr();

            for (int i = 0; i < total; ++i)
            {
                int diff = std::abs(int(gtPtr[i]) - int(dstPtr[i]));
                Assert::IsTrue(
                    diff <= 1,
                    (L"Pixel difference too large at index " + std::to_wstring(i)).c_str()
                );
            }

            cv::Mat dstMat(height,
                width,
                optimizedDstBuffer.GetNumChannels() == 3 ? CV_8UC3 : CV_8UC4,
                optimizedDstBuffer.GetBufferPtr(),
                optimizedDstBuffer.GetStride());

            auto dotPos = imageName.find_last_of('.');
            std::string outName = (dotPos == std::string::npos)
                ? imageName + "_blurred.jpg"
                : imageName.substr(0, dotPos) + "_blurred.jpg";

            cv::imwrite(outName, dstMat);
        }

        TEST_METHOD(TestOpenCVBlur)
        {
            std::string imageName = "C:/Users/KevinYK_Chen/Desktop/CPP_repo/BoxBlurLib/BoxBlurUnitTest/4k_image.jpg";
            cv::Mat src = cv::imread(imageName, cv::IMREAD_COLOR);

            Assert::IsFalse(src.empty(), L"Failed to load test image");

            int width = src.cols;
            int height = src.rows;
            int numChannel = src.channels();
            int kernelSize = 31;
            ImageCore::ImageBuffer srcBuffer(src);
            ImageCore::ImageBuffer gaussianDstBuffer(width, height, ImageCore::PixelFormat::BGR);
            ImageCore::ImageBuffer medianDstBuffer(width, height, ImageCore::PixelFormat::BGR);
            ImageCore::ImageBuffer boxDstBuffer(width, height, ImageCore::PixelFormat::BGR);
            GaussianBlur gaussianBlur;
			MedianBlur medianBlur;
            BoxBlur boxBlur(8);
            GaussianBlurSpecificParam gaussianBlurSpecificParam{5.0, 5.0};

            auto start = std::chrono::high_resolution_clock::now();
            gaussianBlur.Apply(srcBuffer, gaussianDstBuffer, kernelSize, &gaussianBlurSpecificParam);
            medianBlur.Apply(srcBuffer, medianDstBuffer, kernelSize, &gaussianBlurSpecificParam);
            boxBlur.Apply(srcBuffer, boxDstBuffer, kernelSize, &gaussianBlurSpecificParam);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            std::wstring timeMessage = L"Gaussian Blur execution time: " + std::to_wstring(duration.count()) + L" ms";
            Logger::WriteMessage(timeMessage.c_str());

            cv::Mat gaussianDstMat = OpenCVAdapter::ToMatView(gaussianDstBuffer);
            cv::Mat medianDstMat = OpenCVAdapter::ToMatView(medianDstBuffer);
            cv::Mat boxDstMat = OpenCVAdapter::ToMatView(boxDstBuffer);

            auto dotPos = imageName.find_last_of('.');
            std::string gaussianOutName = (dotPos == std::string::npos)
                ? imageName + "_gaussian_blurred.jpg"
                : imageName.substr(0, dotPos) + "_gaussian_blurred.jpg";
            std::string medianOutName = (dotPos == std::string::npos)
                ? imageName + "_median_blurred.jpg"
                : imageName.substr(0, dotPos) + "_median_blurred.jpg";
            std::string boxOutName = (dotPos == std::string::npos)
                ? imageName + "_box_blurred.jpg"
                : imageName.substr(0, dotPos) + "_box_blurred.jpg";

            cv::imwrite(gaussianOutName, gaussianDstMat);
            cv::imwrite(medianOutName, medianDstMat);
            cv::imwrite(boxOutName, boxDstMat);
        }
    };
}
