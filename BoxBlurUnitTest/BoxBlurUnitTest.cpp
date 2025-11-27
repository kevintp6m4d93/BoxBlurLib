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

        // ---- 測試 OpenCV 是否正常 ----
        TEST_METHOD(TestOpenCV)
        {
            cv::Mat img = cv::Mat::zeros(100, 100, CV_8UC1);
            Assert::IsFalse(img.empty(), L"OpenCV Mat should not be empty");
        }

        // ---- 測試你的 BoxBlur 函式 ----
        TEST_METHOD(BoxBlur_CompareWithOpenCV)
        {
            // 1. 讀圖片（確保 test.png 有被複製到輸出目錄）
            std::string imageName = "C:/Users/KevinYK_Chen/Desktop/CPP_repo/BoxBlurLib/BoxBlurUnitTest/test_image.jpg";
            cv::Mat src = cv::imread(imageName, cv::IMREAD_COLOR);  // 3-channel BGR

            Assert::IsFalse(src.empty(), L"Failed to load test image (C:/Users/KevinYK_Chen/Desktop/CPP_repo/BoxBlurLib/BoxBlurUnitTest/test_image.jpg)");

            int width = src.cols;
            int height = src.rows;
            int numChannel = src.channels();
            int kernelSize = 5;   // 例如 5x5 的 box blur 

            // 2. 準備給 BoxBlur 用的輸出 buffer
            cv::Mat dstBox(src.size(), src.type());

            // 建 BoxBlur（選一種 padding，這裡用 Mirror 對應 OpenCV 的 BORDER_REFLECT_101）
            Blur::BoxBlur boxBlur(Blur::PaddingMode::Replicate);

            // 呼叫你的 BoxBlur：直接用 Mat 的 data 當 buffer
            boxBlur.Apply(src.data,
                dstBox.data,
                width,
                height,
                numChannel,
                kernelSize);

            // 3. 用 OpenCV 的 box blur 當 ground truth
            cv::Mat dstRef;
            cv::blur(src,
                dstRef,
                cv::Size(kernelSize, kernelSize),
                cv::Point(-1, -1),
                cv::BORDER_REPLICATE);   // 對應 Mirror padding

            // 4. 先確認尺寸 / channel 一致
            Assert::AreEqual(dstRef.rows, dstBox.rows);
            Assert::AreEqual(dstRef.cols, dstBox.cols);
            Assert::AreEqual(dstRef.channels(), dstBox.channels());

            // 5. 逐像素比對（允許誤差 1，避免四捨五入差異）
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

            // 6. 另外把 BoxBlur 結果存成 <image_name>_blurred.jpg，方便人工查看
            auto dotPos = imageName.find_last_of('.');
            std::string outName = (dotPos == std::string::npos)
                ? imageName + "_blurred.jpg"
                : imageName.substr(0, dotPos) + "_blurred.jpg";

            cv::imwrite(outName, dstBox);
        }
    };
}
